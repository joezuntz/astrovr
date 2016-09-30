#pragma once
#include "stdafx.h"

#ifdef _H_AVR_IMAGE
#else
#define _H_AVR_IMAGE

#include "avr_object.hh"
#include "avr_fits.hh"


class AVRImage : public AVRFits
{
public:

    AVRImage();

    int width;
    int height;
	void loadFits(const char * filename, std::vector<float> &pixel_data, long dim[2]);
    void setupImage(const char * filename);
    GLuint texture;
    
	void enlargeTextureRBGPower2(std::vector<GLfloat> &pixels, 
    std::vector<GLfloat> &embed_image,
    int original_width, int original_height,
    int desired_width, int desired_height
    );
	virtual const char * objectName() { return "image"; };

    virtual void draw(glm::mat4 projection);
};


#endif