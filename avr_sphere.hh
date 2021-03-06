#ifdef _H_AVR_SPHERE
#else
#define _H_AVR_SPHERE
#include "avr_object.hh"


class AVRSphere : public AVRObject
{
public:
    GLdouble radius;

    AVRSphere(double r);
    ~AVRSphere();
    void setupSphere();    
    void load(const char * filename);  // load an image file as the texture
    virtual void draw(glm::mat4 projection);

    GLuint texture;
    int width, height; // of the texture image
    std::vector<GLushort> indices;
    GLuint elementBuffer;

};


#endif