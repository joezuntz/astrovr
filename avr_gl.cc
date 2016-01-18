#include "avr_gl.hh"
#include <iostream>

/*
GLFWwindow*  avr_setup_window(int window_width, int window_height){

    glfwInit();

    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);    


    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Healpix Viewer", NULL, NULL);    

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        throw "Fail";
    }


    // Init GLFW
    glfwMakeContextCurrent(window);
    glEnable (GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_TEXTURE_2D);

    // Set the required callback functions
    // glfwSetWindowUserPointer(window, this);
    // glfwSetKeyCallback(window, key_callback);


    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    //glewExperimental = GL_TRUE;
    //// Initialize GLEW to setup the OpenGL Function pointers
    //if (glewInit() != GLEW_OK)
    //{
    //    std::cerr << "Failed to initialize GLEW" << std::endl;
    //    throw "Fail2";
    //}    

    //Apparently okay to ignore this.
    GLuint err = glGetError();
    if (err!=GL_NO_ERROR){
        gluErrorString(err);
    }

    return window;
}

*/