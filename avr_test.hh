#pragma once
#include "stdafx.h"

#ifdef _H_AVR_TEST
#else
#define _H_AVR_TEST

#include "avr_object.hh"


class AVRTest : public AVRObject
{
public:

    AVRTest(float offset);
    virtual void draw(glm::mat4 projection);
	virtual const char * objectName() { return "test"; };

};


#endif