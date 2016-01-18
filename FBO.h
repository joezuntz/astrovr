// FBO.h
#ifndef _FBO_H_
#define _FBO_H_

#include "avr_gl.hh"

//#if defined(_WIN32)
//#include <windows.h>
//#endif
//
//#ifdef __APPLE__
//#include <OpenGL/gl.h>
//#else
//#include <GL/glu.h>
//#endif

struct FBO {
    GLuint id, tex, depth;
    GLuint w, h;
};

void   allocateFBO(FBO&, int w, int h);
void deallocateFBO(FBO&);
void   bindFBO(const FBO&, float fboScale=1.0f);
void unbindFBO();

#endif //_FBO_H_
