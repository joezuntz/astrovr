#ifdef _H_AVR_VIEWER
#else
#define _H_AVR_VIEWER

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#define GLM_FORCE_RADIANS
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>
#include "avr_object.hh"

class AVRViewer
{
public:
	AVRViewer(int w, int h);
	void setupWindow();
	void handleInput(int key, int scancode, int action, int mode);
	glm::mat4 projectionMatrix();
	void runLoop();
	/* Window information */
    GLFWwindow* window;
	int window_width;// = 800
	int window_height;// = 600;
	int buffer_width;// = 800
	int buffer_height;// = 600;
    GLfloat viewAngle;
    GLfloat aspectRatio;

	// Objects to render
	std::vector<AVRObject*> objects;

	// Camera position and orientation
    glm::vec3 cameraPos;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;


};


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);


#endif