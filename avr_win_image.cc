#ifdef _WIN32
#include <windows.h>
#include <objidl.h>
#pragma warning(disable : 4458)
#pragma comment (lib, "gdiplus.lib")

#include <algorithm>
namespace Gdiplus
{
	using std::min;
	using std::max;
}

#include <gdiplus.h>


void avr_load_image_windows(const char * filename, unsigned char ** data, unsigned int *width, unsigned int *height)
{
	WCHAR wfilename[1024];
	mbstowcs(wfilename, filename, 1024);
	Gdiplus::Bitmap image(wfilename);

	if (image.GetLastStatus() != Gdiplus::Ok) {
		throw "Failed to load image file properly";
	}


	unsigned int h, w;
	w = image.GetWidth();
	h = image.GetHeight();

	unsigned int total_size = w*h;
	unsigned int total_bytes = total_size * 3;
	unsigned char * image_data = new unsigned char[total_bytes];
	Gdiplus::Rect rect(0, 0, w, h);
	Gdiplus::BitmapData* bitmapData = new Gdiplus::BitmapData;

	image.LockBits(&rect, Gdiplus::ImageLockModeRead,
		PixelFormat24bppRGB,bitmapData);

	unsigned char * pixels = (unsigned char*)bitmapData->Scan0;
	UINT p = 0;
	for (UINT row = 0; row < h; ++row)
	{
		for (UINT col = 0; col < w; ++col)
		{
			image_data[p++] = pixels[row * bitmapData->Stride + 3*col + 2];
			image_data[p++] = pixels[row * bitmapData->Stride + 3*col + 1];
			image_data[p++] = pixels[row * bitmapData->Stride + 3*col + 0];
		}
	}


	*height = h;
	*width = w;
	*data = image_data;


}

#endif