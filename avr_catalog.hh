#ifdef _H_AVR_CATALOG
#else
#define _H_AVR_CATALOG

#include "avr_object.hh"

class AVRCatalog : public AVRObject
{
public:

    AVRCatalog();
    ~AVRCatalog();
    void load(const char * filename);
    virtual void draw(glm::mat4 projection);
    void checkFitsStatus(int status, const char * location);
    void push_point(float ra, float dec, float size, float d);

};


#endif