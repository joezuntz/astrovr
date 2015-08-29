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
	void loadFits(const char * filename, std::vector<float> &pixel_data);
    void setupImage(const char * filename);
    GLuint texture;
    

    virtual void draw(glm::mat4 projection);
};


#endif