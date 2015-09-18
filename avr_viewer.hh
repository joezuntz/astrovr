#ifdef _H_AVR_VIEWER
#else
#define _H_AVR_VIEWER

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "avr_object.hh"

class AVRViewer
{
public:
	AVRViewer(GLFWwindow* _window);
	void setupWindow();
	void handleInput(int key, int scancode, int action, int mode);
	glm::mat4 projectionMatrix();
	void runLoop();
	void updatePointing(float deltaTime);	
	/* Window information */
	int window_width;// = 800
	int window_height;// = 600;
	int buffer_width;// = 800
	int buffer_height;// = 600;
    GLfloat viewAngle;
    GLfloat aspectRatio;
    GLFWwindow* window;

	// Objects to render
	std::vector<AVRObject*> objects;

	// Camera position and orientation
    glm::vec3 cameraPos;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;

    glm::vec3 rotationalVelocity;


};


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);


#endif