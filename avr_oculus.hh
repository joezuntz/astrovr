#include "avr_gl.hh"
#include "FBO.h"
#include "avr_test.hh"

// #include "avr_object.hh"

class AVROculus
{
public:

	// The Head-Mounted Display
	ovrHmd hmd;
    ovrPosef eyePoses[2];

	GLFWwindow *window;
	GLuint frameBuffer;
	GLuint texture;
	GLuint depthBuffer;
    int width, height;
    FBO eyeFBOs[2];
    ovrGLTexture eyeTextures[2];
	ovrEyeRenderDesc eyeDescriptors[2];
	AVRTest * triangle;



	void reportError(const char * location);
	void setup();
	void runLoop();
	void configureTexture();
	void configureGLFW();
	void configureEyes();	
	void renderEye(ovrEyeType eye);
	void setupTriangle(); //Temporary test method

};