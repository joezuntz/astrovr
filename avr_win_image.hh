#ifdef _H_AVR_WIN_IMAGE
#else
#define _H_AVR_WIN_IMAGE

#ifdef _WIN32
void avr_load_image_windows(const char * filename, unsigned char ** data, unsigned int *width, unsigned int *height);
#endif

#endif