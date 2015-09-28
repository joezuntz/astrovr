#ifndef _H_AVR_GL
#define _H_AVR_GL

#define GLEW_STATIC
#include <GL/glew.h>

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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <GLFW/glfw3.h>

// #include "avr_healpix.hh"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>



GLFWwindow * avr_setup_window(int window_width, int window_height);

#endif
