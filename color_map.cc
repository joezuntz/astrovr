#include "color_map.hh"
#include "math.h"

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
    return base( gray - 0.5 );
}
static
float green( float gray ) {
    return base( gray );
}
static
float blue( float gray ) {
    return base( gray + 0.5 );
}

JetColorMap::JetColorMap(float x_min, float x_max, bool is_log) : 
ColorMap(x_min, x_max, is_log) {};


float JetColorMap::operator()(float x, float c[3]){
	float v;
	if (islog){
		if (x<=xmin) x = xmin*1.0001;
		if (x>xmax) x = xmax;
		v = (log(x)-log(xmin))/(log(xmax)-log(xmin));
	}
	else {
		v = (x-xmin)/xrange;
		if (v<0) v = 0.0;
		if (v>1) v = 1.0;
	}
	c[0] = red(v);
	c[1] = green(v);
	c[2] = blue(v);
}

