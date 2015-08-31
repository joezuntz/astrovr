#include "avr_viewer.hh"
#include <chrono>

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    AVRViewer * viewer = (AVRViewer*) glfwGetWindowUserPointer(window);
    viewer->handleInput(key, scancode, action, mode);
}

AVRViewer::AVRViewer(int w, int h) : window_width(w), window_height(h) {
    cameraPos   = glm::vec3(0.0f, 0.0f,  0.0f);
    cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
    viewAngle = 90.0f;

}

void AVRViewer::setupWindow(){

    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);    


    // Create a GLFWwindow object that we can use for GLFW's functions
    window = glfwCreateWindow(window_width, window_height, "Healpix Viewer", NULL, NULL);    

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        throw "Fail";
    }

    //
    glfwGetFramebufferSize(window, &buffer_width, &buffer_height);
    std::cout << "width = " << buffer_width << "  height = " << buffer_height << std::endl;
    
    aspectRatio = buffer_width/buffer_height;


    // Init GLFW
    glfwMakeContextCurrent(window);
    glEnable (GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE_EXT);
    glEnable(GL_TEXTURE_2D);

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
    glViewport(0, 0, buffer_width, buffer_height);


}



void AVRViewer::handleInput(int key, int scancode, int action, int mode){
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

glm::mat4 AVRViewer::projectionMatrix(){

    // Model Matrix - from local coordinates to world coordinates
    glm::mat4 model;
    model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f)); 

    // View matrix - rotate to the camera plane.  No change here right now
    glm::mat4 view;
    view = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    // Projection matrix - apply perspective
    glm::mat4 projection;
    projection = glm::perspective(glm::radians(viewAngle), aspectRatio, 0.1f, 2.0f);

    projection = projection*view*model;
    return projection;


}

void AVRViewer::runLoop() {


    auto start = std::chrono::system_clock::now();
    int nframe = 0;


    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glfwWindowShouldClose(window);
    

    while (!glfwWindowShouldClose(window))
    {
        // Clear the buffer window
        checkGLerror("before clear");
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        checkGLerror("after clear");

        // Determine the current matrix that projects
        // from the 3D coordinates to the screen coordinates
        glm::mat4 projection = projectionMatrix();
        

        // Draw each of our objects
        for (AVRObject * object : objects) {
            object->draw(projection);
        }
        checkGLerror("after draw");

        //Check for user pressing a key
        glfwPollEvents();

        // Swap the screen buffers
        glfwSwapBuffers(window);
        checkGLerror("after swap");

        nframe++;
        if (nframe%100==0){

            auto now = std::chrono::system_clock::now();
            double seconds = std::chrono::duration_cast<
  std::chrono::duration<double> >(now - start).count();
            double fps = nframe/seconds;
            printf("FPS %f  = %d / %f\n", fps, nframe, seconds);
        }

    }
}