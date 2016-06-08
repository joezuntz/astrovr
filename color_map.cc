#include "color_map.hh"
#include "math.h"
#include <stdio.h>
#pragma warning(disable : 4244)

ColorMap::ColorMap(float x_min, float x_max, bool is_log) :
 xmin(x_min), xmax(x_max), islog(is_log), xrange(xmax-xmin)
 {

}

static
float interpolate( float val, float y0, float x0, float y1, float x1 ) {
    return (val-x0)*(y1-y0)/(x1-x0) + y0;
}

static
float base( float val ) {
    if ( val <= -0.75 ) return 0;
    else if ( val <= -0.25 ) return interpolate( val, 0.0, -0.75, 1.0, -0.25 );
    else if ( val <= 0.25 ) return 1.0;
    else if ( val <= 0.75 ) return interpolate( val, 1.0, 0.25, 0.0, 0.75 );
    else return 0.0;
}

static
float red( float gray ) {
    return (float) base( gray - 0.5 );
}
static
float green( float gray ) {
    return (float)base( gray );
}
static
float blue( float gray ) {
    return (float)base( gray + 0.5 );
}

JetColorMap::JetColorMap(float x_min, float x_max, bool is_log) : 
ColorMap(x_min, x_max, is_log) {};

static float lin_norm(float x, float xmin, float xmax) {
	return (x - xmin) / (xmax - xmin);
}
static float log_norm(float x, float xmin, float xmax) {
	return (log(x) - log(xmin)) / (log(xmax) - log(xmin));
}


float JetColorMap::operator()(float x, float c[3]){
	float v;
	if (islog){
		if (x<=xmin) x = xmin*1.0001;
		if (x>xmax) x = xmax;
		v = log_norm(x, xmin, xmax);
	}
	else {
		v = lin_norm(x, xmin, xmax);
		if (v<0) v = 0.0;
		if (v>1) v = 1.0;
	}
	c[0] = red(v);
	c[1] = green(v);
	c[2] = blue(v);
	return 1.0;
}

PlanckColorMap::PlanckColorMap(const char * filename) :
ColorMap(0.0f, 1.0f, false)
{
	//Open the file and read each of the lines.
	//Make three interpolators in r, g,b
	FILE * f = fopen(filename, "r");
	for (int i = 0; i < 256; i++) {
		float x, r, g, b;
		fscanf(f, "%f  %f  %f  %f\n", &x, &r, &g, &b);
		R[i] = r/256.0;
		G[i] = g / 256.0;
		B[i] = b / 256.0;
	}
	fclose(f);
}



float PlanckColorMap::operator()(float x, float c[3]) {
	//Interpolate in r,g,b
	if (!isfinite(x)) x = 0.5f;
	int i = (int)(x * 256.0);
	if (i < 0) i = 0;
	if (i > 255) i = 255;
	c[0] = R[i];
	c[1] = G[i];
	c[2] = B[i];
	return 1.0;
}



PlanckBrokenColorMap::PlanckBrokenColorMap(const char * filename, float scale) :
	PlanckColorMap(filename),
	x1(-1e3f), x2(-1.0f), x3(1.0f), x4(1e7f),
	y1(0.0f), y2(0.28f), y3(0.32f), y4(1.0f),
	scaling(scale)
{
}

float PlanckBrokenColorMap::operator()(float x, float c[3]) {
	x *= scaling;

	if (x < x1) {
		x = x1;
	}
	else if (x>x4) {
		x = x4;
	}

	if ((x >= x1) && (x < x2)) {
		x = 1-log_norm(-x, -x2, -x1);
		x = y1 + x*(y2 - y1);
	}
	else if ((x >= x2) && (x < x3)) {
		x = lin_norm(x, x2, x3);
		x = y2 + x*(y3 - y2);
	}
	else if ((x >= x3) && (x <= x4)) {
		x = log_norm(x, x3, x4);
		x = y3 + x*(y4 - y3);
	}
	else {
		// nan. return mid point. or something.
		x = 0.3f;
	}
	return PlanckColorMap::operator()(x, c);
}

