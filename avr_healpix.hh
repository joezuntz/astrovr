#ifdef _H_AVR_HEALPIX
#else
#define _H_AVR_HEALPIX

#include "color_map.hh"
#include "avr_object.hh"
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

    // Map data
    Healpix_Map<double> fullResMap;
    Healpix_Map<double> lowResMap;
    Healpix_Base HP;


    // Map plotting information
    float radius;
    ColorMap * color_map;

    AVRHealpix(int nside, float r);
    ~AVRHealpix();
    void load(const char * filename);
    void push_healpix_triangle(vec3 &p1, vec3 &p2, vec3 &p3, glm::vec4 &col, GLfloat scale);
    virtual void draw(glm::mat4 projection);

};


#endif