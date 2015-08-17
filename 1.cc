#include <iostream>
#include <fstream>
#include <sstream>

#define GLM_FORCE_RADIANS
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>


// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "healpix_base.h"
#include "healpix_map.h"
#include "healpix_map_fitsio.h"

#include "color_map.hh"

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;


class ViewManager
{
    GLFWwindow* window;
    GLuint vertexArrayObject;    
    GLuint vertexBuffer;
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint shaderProgram;    
    int order;
    int nside;
    int npix;
    int nring;
    GLfloat viewAngle;
    GLfloat r, g, b;
    std::vector<GLfloat> vertices;
    Healpix_Base HP;

    glm::vec3 cameraPos;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;

public:
    // ViewManager();
    void setupHealpix(std::string &filename);
    void setupWindow();
    void compileVertexShader(const char * filename);
    void compileFragmentShader(const char * filename);
    void createProgram();
    void drawHealpix();
    GLuint compileShader(const char * filename, GLuint shaderType);
    void sendMatrix(const char * name, glm::mat4 &M);
    void checkGLerror(const char * where);    
    // ~ViewManager();
    void runLoop();
    void handleInput(int key, int scancode, int action, int mode);
    ViewManager(int nside);
    /* data */
};

ViewManager::ViewManager(int nside) : nside(nside), HP(nside, RING, SET_NSIDE), viewAngle(90.0) {
    npix = HP.Npix();
    order = (int)log2(nside);
    nring = 4*nside-1;
    std::cout << "Map order = " << order << std::endl;
}

void ViewManager::compileVertexShader(const char * filename){
    vertexShader = compileShader(filename, GL_VERTEX_SHADER);
}


void ViewManager::compileFragmentShader(const char * filename){
    fragmentShader = compileShader(filename, GL_FRAGMENT_SHADER);
}

GLuint ViewManager::compileShader(const char * filename, GLuint shaderType){
    std::string shaderCode;
    std::ifstream infile(filename);
    if ( !infile.good() ) throw "shader error";
    std::stringstream buffer;
    buffer << infile.rdbuf();
    shaderCode = buffer.str();
    const char * code = shaderCode.c_str();

    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &(code), NULL);
    glCompileShader(shader);

    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "Error in shader in " << filename << " \n" << infoLog << std::endl;
    }
    else{
        std::cerr << "Compiled shader. " << std::endl;
    }

    return shader;
}


void ViewManager::checkGLerror(const char * where){
   GLuint err = glGetError();
    if (err!=GL_NO_ERROR){
        const GLubyte* err_string = gluErrorString(err);
        std::cerr << where << ":" << err_string << std::endl;
        throw "GLError";
    }

}


void push_triangle(std::vector<GLfloat> &v, vec3 &p1, vec3 &p2, vec3 &p3, glm::vec4 &col, GLfloat scale)
{
    // push each of three pixels in turn
    v.push_back((GLfloat) scale * p1.x);
    v.push_back((GLfloat) scale * p1.y);
    v.push_back((GLfloat) scale * p1.z);
    v.push_back(1.0f);
    v.push_back(col.x);
    v.push_back(col.y);
    v.push_back(col.z);
    v.push_back(col.w);

    v.push_back((GLfloat) scale * p2.x);
    v.push_back((GLfloat) scale * p2.y);
    v.push_back((GLfloat) scale * p2.z);
    v.push_back(1.0f);
    v.push_back(col.x);
    v.push_back(col.y);
    v.push_back(col.z);
    v.push_back(col.w);

    v.push_back((GLfloat) scale * p3.x);
    v.push_back((GLfloat) scale * p3.y);
    v.push_back((GLfloat) scale * p3.z);
    v.push_back(1.0f);
    v.push_back(col.x);
    v.push_back(col.y);
    v.push_back(col.z);
    v.push_back(col.w);

}


void ViewManager::setupHealpix(std::string &filename){
    // Transfer the vertex info to the GPU


    cameraPos   = glm::vec3(0.0f, 0.0f,  0.0f);
    cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

    Healpix_Map<double> fullResMap;
    read_Healpix_map_from_fits(filename, fullResMap);

    Healpix_Map<double> map(order, RING);
    map.Import(fullResMap, false);

    float vmin=0.01;
    float vmax=50.0;

    JetColorMap jet(vmin, vmax, true);
    float scale = 0.25;
    for (int i=0; i<npix; i++){
        std::vector<vec3> vd;
        HP.boundaries(i, 1, vd);
        // std::cout << "X["  << i << "] = " << vd[1].x << ", " << vd[1].y << ", " << vd[1].z << std::endl;
        // We now have four points, the four coordinates of the pixel edge.
        // we need to draw two triangles, ABC and ACD
        GLfloat rgb[3];
        jet(map[i], rgb);

        glm::vec4 v(rgb[0], rgb[1], rgb[2], 1.0f);
        push_triangle(vertices, vd[0], vd[1], vd[2], v, scale);
        push_triangle(vertices, vd[0], vd[2], vd[3], v, scale);
    }

    // Build the vertex array object.
    // subsequent calls to glVertex things now refer to the contex
    // in this object
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);

    glGenBuffers(1, &vertexBuffer);
    checkGLerror("GenBuffers");

    // This sets the current array buffer to being the one
    // labelled as vertexBuffer.
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    checkGLerror("BindBuffer");


    // glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*9, vertices1, GL_STATIC_DRAW);

    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertices.size(), vertices.data(), GL_STATIC_DRAW);
    // checkGLerror("BufferData");

    //Our vertex shader can take arbitrary input.
    //We have to tell opengl how to interpret that input as the input
    //that the shader is expecting
    // This call sets up the input described as "position" in vertex.shader
    // 0: input location we are configuring
    // 3: number of elements in the input
    // GL_FLOAT: type of element
    // GL_FALSE: something to do with normalization
    // 3 * sizeof(GLfloat): stride
    // (GLvoid*)0: offset
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0); 
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)(4*sizeof(GLfloat)));
    glEnableVertexAttribArray(1); 

    checkGLerror("Attrib");



    // //Disconnect from this object
    // glBindVertexArray(0);

    std::cout << "Copied " << sizeof(GLfloat)*vertices.size() <<  " bytes to GPU (?)" << std::endl;
}

void ViewManager::createProgram(){
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "Error with shader program: " << std::endl << infoLog << std::endl;
        throw "error";
    }
    else{
        std::cout << "Compiled shader program." << std::endl;
    }



    glUseProgram(shaderProgram);

}


void ViewManager::setupWindow(){


    r = 0.0;
    g = 0.0;
    b = 0.0;

    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);    


    // Create a GLFWwindow object that we can use for GLFW's functions
    window = glfwCreateWindow(WIDTH, HEIGHT, "Healpix Viewer", NULL, NULL);    
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        throw "Fail";
    }

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    std::cout << "width = " << width << "  height = " << height << std::endl;

    glfwGetFramebufferSize(window, &width, &height);
    std::cout << "width = " << width << "  height = " << height << std::endl;
    

    // Init GLFW
    glfwMakeContextCurrent(window);
    glEnable (GL_DEPTH_TEST);

    // Set the required callback functions
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, key_callback);


    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        throw "Fail2";
    }    

    //Apparently okay to ignore this.
    GLuint err = glGetError();
    if (err!=GL_NO_ERROR){
        const GLubyte* err_string = gluErrorString(err);
    }

    // Define the viewport dimensions
    glViewport(0, 0, width, height);


}

void ViewManager::handleInput(int key, int scancode, int action, int mode){
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    GLfloat cameraSpeed = 0.05f;
    GLfloat ang = glm::radians(5.0f);

    if(key == GLFW_KEY_W){
        glm::vec3 u = cameraUp;
        cameraUp = cosf(ang)*cameraUp - sinf(ang)*cameraFront;
        cameraFront = cosf(ang)*cameraFront + sinf(ang)*u;
    }
    if(key == GLFW_KEY_S){
        glm::vec3 u = cameraUp;
        cameraUp = cosf(-ang)*cameraUp - sinf(-ang)*cameraFront;
        cameraFront = cosf(-ang)*cameraFront + sinf(-ang)*u;
    }
    if(key == GLFW_KEY_A){
        glm::vec3 perp = glm::cross(cameraFront, cameraUp);
        cameraFront = cosf(-ang)*cameraFront + sinf(-ang)*perp;
    }
    // glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if(key == GLFW_KEY_D){
        glm::vec3 perp = glm::cross(cameraFront, cameraUp);
        cameraFront = cosf(ang)*cameraFront + sinf(ang)*perp;
    }

    if(key == GLFW_KEY_Q){
        glm::vec3 perp = glm::cross(cameraFront, cameraUp);
        cameraUp = cosf(ang)*cameraUp + sinf(ang)*perp;
    }
    if(key == GLFW_KEY_E){
        glm::vec3 perp = glm::cross(cameraFront, cameraUp);
        cameraUp = cosf(-ang)*cameraUp + sinf(-ang)*perp;
    }

    if(key == GLFW_KEY_R){
        viewAngle*=1.1;
    }
    if(key == GLFW_KEY_F){
        viewAngle/=1.1;
    }


}


void ViewManager::sendMatrix(const char * name, glm::mat4 &M){
    GLuint handle = glGetUniformLocation(shaderProgram, name);
    glUniformMatrix4fv(handle, 1, GL_FALSE, glm::value_ptr(M));
    checkGLerror(name);
}

void ViewManager::drawHealpix(){


    // GLfloat t = glfwGetTime();

    // Model Matrix - from local coordinates to world coordinates
    glm::mat4 model;
    model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f)); 

    // View matrix - rotate to the camera plane.  No change here right now
    glm::mat4 view;
    view = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    // Projection matrix - apply perspective
    glm::mat4 projection;
    projection = glm::perspective(glm::radians(viewAngle), ((GLfloat)WIDTH) / HEIGHT, 0.1f, 2.0f);

    // Save these matrices
    sendMatrix("model", model);
    sendMatrix("view", view);
    sendMatrix("projection", projection);


    //Choose the color scheme
    glm::vec4 blue(0.0f, 0.0f, 1.0f, 1.0f);
    glm::vec4 red(1.0f, 0.0f, 0.0f, 1.0f);

    GLuint colorMinLoc = glGetUniformLocation(shaderProgram, "color_min");
    glUniform4fv(colorMinLoc, 1, glm::value_ptr(blue));
    checkGLerror("Color Max");

    GLuint colorMaxLoc = glGetUniformLocation(shaderProgram, "color_max");
    glUniform4fv(colorMaxLoc, 1, glm::value_ptr(red));
    checkGLerror("Color Min");

    glDrawArrays(GL_TRIANGLES, 0, vertices.size()/3);
    checkGLerror("Draw");

}


void ViewManager::runLoop() {

    glUseProgram(shaderProgram);
    // glBindVertexArray(vertexArrayObject);
    // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    checkGLerror("Clear color");


    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        drawHealpix();
        glfwPollEvents();

        // // Swap the screen buffers
        glfwSwapBuffers(window);
    }


}

// The MAIN function, from here we start the application and run the game loop
int main()
{
    glfwInit();


    int nside = 128;
    ViewManager viewManager(nside);
    viewManager.setupWindow();
    std::cout << glGetString(GL_VERSION) << std::endl;
    std::string filename("map.fits");
    viewManager.setupHealpix(filename);
    // glShadeModel(GL_SMOOTH);
    // checkGLerror("shade model");
    viewManager.compileVertexShader("vertex.shader");
    viewManager.compileFragmentShader("fragment.shader");
    viewManager.createProgram();
    viewManager.runLoop();

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    ViewManager * viewManager = (ViewManager*) glfwGetWindowUserPointer(window);
    viewManager->handleInput(key, scancode, action, mode);
}