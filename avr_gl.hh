#ifndef _H_AVR_GL
#define _H_AVR_GL

#define _USE_MATH_DEFINES
#include <cmath>

#if defined(_WIN32)
#undef HAVE_UNISTD_H

//#define GLFW_EXPOSE_NATIVE_WIN32
//#define GLFW_EXPOSE_NATIVE_WGL
#define OVR_OS_WIN32

//Disable some over-zealous warnings that trigger
//the Oculus SDK
#pragma warning(disable : 4201)
#pragma warning(disable : 4100)
#pragma warning(disable : 4244)

//Link with a bunch of libs


#pragma warning (disable:4100)
#include "../../OculusRoomTiny_Advanced/Common/Win32_GLAppUtil.h"
#include "Kernel/OVR_System.h"

// Include the Oculus SDK
#include "GL/CAPI_GLE.h"

//#include "OVR_CAPI_GL.h"
#include <GL/GLU.h>

//
//#define GL_GLEXT_PROTOTYPES
//#include <windows.h>


#elif defined(__APPLE__)
 #define GLFW_EXPOSE_NATIVE_COCOA
 #define GLFW_EXPOSE_NATIVE_NSGL
 #define OVR_OS_MAC
#elif defined(__linux__)
 #define GLFW_EXPOSE_NATIVE_X11
 #define GLFW_EXPOSE_NATIVE_GLX
 #define OVR_OS_LINUX
#endif

/*
#include <GL/CAPI_GLE.h>
#include <OVR_CAPI_GL.h>
*/

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>

//GLFWwindow * avr_setup_window(int window_width, int window_height);

#endif
