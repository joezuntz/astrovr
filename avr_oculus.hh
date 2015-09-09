#if defined(_WIN32)
 #define GLFW_EXPOSE_NATIVE_WIN32
 #define GLFW_EXPOSE_NATIVE_WGL
 #define OVR_OS_WIN32
#elif defined(__APPLE__)
 #define GLFW_EXPOSE_NATIVE_COCOA
 #define GLFW_EXPOSE_NATIVE_NSGL
 #define OVR_OS_MAC
#elif defined(__linux__)
 #define GLFW_EXPOSE_NATIVE_X11
 #define GLFW_EXPOSE_NATIVE_GLX
 #define OVR_OS_LINUX
#endif
// #include <GLFW/glfw3native.h>
#include <OVR_CAPI_GL.h>
#include <GLFW/glfw3.h>
#include "FBO.h"

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
	GLuint triangleVAO;
	GLuint triangleBuffer;

	void reportError(const char * location);
	void setup();
	void runLoop();
	void configureTexture();
	void configureGLFW();
	void configureEyes();	
	void renderEye(ovrEyeType eye);
	void setupTriangle(); //Temporary test method

};