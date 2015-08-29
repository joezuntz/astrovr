#define GLM_FORCE_RADIANS
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>
#include "avr_catalog.hh"

#include "avr_image.hh"
#include <iostream>
#include <fstream>
#include "fitsio.h"

AVRImage::AVRImage(){

	// glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &texture);
    checkGLerror("generating texture");

    // glActiveTexture(GL_TEXTURE0);
    // checkGLerror("active texture");
	
    glBindTexture(GL_TEXTURE_2D, texture);
    checkGLerror("binding texture");

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// float color[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	// glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    checkGLerror("setting min/mag");

}

void AVRImage::loadFits(const char * filename, std::vector<float> &pixel_data, long dim[2])
{

    // Open the FITS file
    int status = 0;
    fitsfile * fits;
    fits_open_file(&fits, filename, READONLY, &status);
    checkFitsStatus(status, "opening file");


    // Get the image size information - ndim and dims
    int ndim;
    fits_get_img_dim(fits, &ndim, &status);
    checkFitsStatus(status, "getting image dimension");

    if (ndim!=2){
        std::cerr << "Bad image dimension (should be 2) : " << ndim << std::endl;
        throw "image dim";
    }
    fits_get_img_size(fits, ndim, dim, &status);
    checkFitsStatus(status, "getting image size");

    std::cerr << "Loaded FITS image of size: " << dim[0] << " x " << dim[1] << std::endl;

    int npix = dim[0] * dim[1];

    height = dim[0];
    width = dim[1];


    // Load the image data
    
    pixel_data.resize(npix);

    long first_pixel[2] = {1,1};
    fits_read_pix(fits, TFLOAT, first_pixel, (LONGLONG) npix,
          NULL, pixel_data.data(), NULL, &status);
    checkFitsStatus(status, "loading image");
    fits_close_file(fits, &status);

}

void AVRImage::enlargeTextureRBGPower2(std::vector<GLfloat> &pixels, 
    std::vector<GLfloat> &embed_image,
    int original_width, int original_height,
    int desired_width, int desired_height
    )
{
    embed_image.resize(3*desired_width*desired_height);


    float vmax = -1e30;
    for (int i=0; i<pixels.size(); i++) if (pixels[i]>vmax) vmax=pixels[i];

    for (int i=0; i<original_width; i++){
        for (int j=0; j<original_height; j++){
            embed_image[(i*desired_height+j)*3+0] = pixels[i*original_height+j]/vmax;
            embed_image[(i*desired_height+j)*3+1] = pixels[i*original_height+j]/vmax;
            embed_image[(i*desired_height+j)*3+2] = pixels[i*original_height+j]/vmax;
        }
    }

}

void AVRImage::setupImage(const char * filename)
{


    // Load the pixels and size of the specified FITS image.
    std::vector<GLfloat> fitsPixels;
    long dim[2];
    loadFits(filename, fitsPixels, dim);


    // OpenGL textures must have side lengths which are powers of two.
    // Embed the image onto a larger image of that size, and make it RGB.
    // TODOL Investigate doing this using GL_LUMINANCE
    std::vector<GLfloat> texturePixels;
    enlargeTextureRBGPower2(fitsPixels, texturePixels, dim[0], dim[1], 128, 128);
    fitsPixels.clear();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 128, 128, 0, GL_RGB, GL_FLOAT, texturePixels.data());
    checkGLerror("TexImage2");



    // The texture we define above is drawn onto a rectangle which
    // we now create by drawing the two triangles that make it up

    GLfloat v[42] = {
//  Position          Color               Texcoords
     -0.25f, 0.25f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f, // Top-left
      0.25f, 0.25f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f, // Top-right
     -0.25f, 0.0f,    1.0f, 1.0f, 1.0f,   0.0f, 1.0f,  // Bottom-left

      0.25f,  0.25f,  1.0f, 1.0f, 1.0f,   1.0f, 0.0f, // Top-right
      0.25f,  0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // Bottom-right
     -0.25f,  0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f,  // Bottom-left
	};

	for (int i=0; i<42; i++) vertices.push_back(v[i]);

    // Send ther data to the GPU
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertices.size(), vertices.data(), GL_STATIC_DRAW);
    checkGLerror("BufferData");


    //Now define the packed data format we are sending in.
	GLsizei stride = 7*sizeof(GLfloat);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(2*sizeof(float)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(5*sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
    checkGLerror("attributes");


    // Two triangles, with corners
}


void AVRImage::draw(glm::mat4 projection)
{
    checkGLerror("before draw");

    useProgram();
    checkGLerror("after useProgram");
    // glm::mat4 projection2;
    sendMatrix("projection", projection);
    checkGLerror("before drawarrays");
    glDrawArrays(GL_TRIANGLES, 0, vertices.size()/7);
    checkGLerror("after drawarrays");

}
