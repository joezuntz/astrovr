#include <iostream>
#include <fstream>
#include <sstream>
#include "avr_object.hh"


void checkGLerror(const char * where){
   GLuint err = glGetError();
    if (err!=GL_NO_ERROR){
        const GLubyte* err_string = gluErrorString(err);
        std::cerr << where << ":" << err_string << std::endl;
        throw "GLError";
    }
}


AVRObject::AVRObject() : vertexArrayObject(0), vertexBuffer(0), visible(true) {
    // Generate a vertex array pointer, which
    // defines where on the GPU our data will be stored (I think)
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);
    checkGLerror("Generating vertex arrays");

    glGenBuffers(1, &vertexBuffer);
    checkGLerror("Generating vertex buffers");

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    checkGLerror("BindBuffer");

    std::cout << "Generated AVRObject: " << vertexArrayObject << "  "   << vertexBuffer  << std::endl;

}


AVRObject::~AVRObject(){

    // delete the vertex buffers here

}




void AVRObject::push_triangle(glm::vec3 &p1, glm::vec3 &p2, glm::vec3 &p3, glm::vec4 &col, GLfloat scale)
{
    // push each of three pixels in turn
    vertices.push_back((GLfloat) scale * p1.x);
    vertices.push_back((GLfloat) scale * p1.y);
    vertices.push_back((GLfloat) scale * p1.z);
    vertices.push_back(1.0f);
    vertices.push_back(col.x);
    vertices.push_back(col.y);
    vertices.push_back(col.z);
    vertices.push_back(col.w);

    vertices.push_back((GLfloat) scale * p2.x);
    vertices.push_back((GLfloat) scale * p2.y);
    vertices.push_back((GLfloat) scale * p2.z);
    vertices.push_back(1.0f);
    vertices.push_back(col.x);
    vertices.push_back(col.y);
    vertices.push_back(col.z);
    vertices.push_back(col.w);

    vertices.push_back((GLfloat) scale * p3.x);
    vertices.push_back((GLfloat) scale * p3.y);
    vertices.push_back((GLfloat) scale * p3.z);
    vertices.push_back(1.0f);
    vertices.push_back(col.x);
    vertices.push_back(col.y);
    vertices.push_back(col.z);
    vertices.push_back(col.w);

}



void AVRObject::compileVertexShader(const char * filename){
    vertexShader = compileShader(filename, GL_VERTEX_SHADER);
}


void AVRObject::compileFragmentShader(const char * filename){
    fragmentShader = compileShader(filename, GL_FRAGMENT_SHADER);
}

GLuint AVRObject::compileShader(const char * filename, GLuint shaderType){
    glBindVertexArray(vertexArrayObject);
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



void AVRObject::sendMatrix(const char * name, glm::mat4 &M){
    GLuint handle = glGetUniformLocation(shaderProgram, name);
    checkGLerror("getting shader handle");
    glUniformMatrix4fv(handle, 1, GL_FALSE, glm::value_ptr(M));
    checkGLerror(name);
}


void AVRObject::createProgram(const char * vertexShaderFilename, const char * fragmentShaderFilename){

    // Create the program
    glBindVertexArray(vertexArrayObject);
    shaderProgram = glCreateProgram();

    //Compile the shaders
    compileVertexShader(vertexShaderFilename);
    compileFragmentShader(fragmentShaderFilename);

    //Attach both the shaders and link the program
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);


    //Check everything worked
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
    useProgram();
}


void AVRObject::useProgram(){
    glBindVertexArray(vertexArrayObject);
    glUseProgram(shaderProgram);
}

void AVRObject::loadStandardShaderDirectory(const char * base_dir)
{
	char vertex_shader_file[1024];
	char fragment_shader_file[1024];
	const char * myName = objectName();
	snprintf(vertex_shader_file, 1024, "%s\\shaders\\%s\\vertex.shader", base_dir, myName);
	snprintf(fragment_shader_file, 1024, "%s\\shaders\\%s\\fragment.shader", base_dir, myName);
	createProgram(vertex_shader_file, fragment_shader_file);

}

void AVRObject::drawIfVisible(glm::mat4 projection)
{
	if (visible) {
		draw(projection);
	}
}
