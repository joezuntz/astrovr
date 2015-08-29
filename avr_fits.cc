#include <iostream>
#include <fstream>
#include "avr_fits.hh"
#include "fitsio.h"

void AVRFits::checkFitsStatus(int status, const char * location)
{
	if (status){
		char message[32];
		std::cout << "FITS error status: " << status << std::endl;
		std::cout << "at operation: " << location << std::endl;
		fits_get_errstatus(status, message);
		std::cout << "Message: " << message << std::endl;
		throw message;
	}

}