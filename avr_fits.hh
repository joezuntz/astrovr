#ifdef _H_AVR_FITS
#else
#define _H_AVR_FITS

#include "avr_object.hh"

class AVRFits : public AVRObject
{
protected:

    void checkFitsStatus(int status, const char * location);
};


#endif