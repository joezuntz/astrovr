#pragma once
#include "stdafx.h"


#ifdef _H_AVR_HEALPIX
#else
#define _H_AVR_HEALPIX
#include "avr_object.hh"
#include "color_map.hh"
#include "healpix_base.h"
#include "healpix_map.h"


class AVRHealpix : public AVRObject
{
public:
    // Healpix map parameters
    int order;
    int nside;
    int npix;
    int nring;

	int frameCount; // approx frame count - just for updating rotation
	double latitude, longitude;

    // Map data
    Healpix_Map<double> fullResMap;
    Healpix_Map<double> lowResMap;
    Healpix_Base HP;

	glm::mat4 skyRotation;

    // Map plotting information
    float radius;
	float alpha;
    ColorMap * color_map;
    GLuint elementBuffer;

	AVRHealpix(int nside, float r);
	AVRHealpix(int nside, float r, float vmin, float vmax, bool isLog);
	AVRHealpix(int nside, float r, ColorMap * color_map);
	~AVRHealpix();
    void load(const char * filename);
    void push_healpix_triangle(vec3 &p1, vec3 &p2, vec3 &p3, glm::vec4 &col, GLfloat scale);
    virtual void draw(glm::mat4 projection);
    void computeCornerIndices(std::vector<vec3> &corners, std::vector<GLuint> &elements);
	virtual const char * objectName() { return "healpix"; };
	virtual void setAlpha(float a);
	void computeSkyRotation();
	double getLocalHourAngle(double LongtitudeAsHourAngle, double ra);
};


#endif