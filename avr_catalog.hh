#pragma once
#include "stdafx.h"


#ifdef _H_AVR_CATALOG
#else
#define _H_AVR_CATALOG

#include "avr_object.hh"
#include "avr_fits.hh"


class AVRCatalog : public AVRFits {
public:

    AVRCatalog();	
    void load(const char * filename);
    virtual void draw(glm::mat4 projection);
    void push_point(float ra, float dec, float size, float d);
	virtual const char * objectName() { return "catalog"; };

};


#endif