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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
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

void AVRImage::enlargeTexturePower2(std::vector<GLfloat> &pixels, 
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
            std::cout << i << "  " << j << pixels[i*original_height+j] << std::endl;
            embed_image[(i*desired_height+j)*3+0] = pixels[i*original_height+j]/vmax;
            embed_image[(i*desired_height+j)*3+1] = pixels[i*original_height+j]/vmax;
            embed_image[(i*desired_height+j)*3+2] = pixels[i*original_height+j]/vmax;
        }
    }

}

void AVRImage::setupImage(const char * filename)
{

    /*

    std::vector<float> pixel_data;
    loadFits(filename, pixel_data);

    int nlarge = 128;

    std::vector<float> embed_image(nlarge*nlarge);
    for (int i=0; i<width; i++){
	    for (int j=0; j<height; j++){
	    	// std::cout << i << "  " << j << std::endl;
	    	embed_image[i*nlarge+j] = pixel_data[i*height+j];
	    }
    }
    pixel_data.clear();







    // Now need to convert pixel data into a texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 128,  128, 0, GL_RED, GL_FLOAT, 
        embed_image.data());
    checkGLerror("Filling texture");

    GLuint samplerHandle = glGetUniformLocation(shaderProgram, "tex");
    checkGLerror("sampler location");

    glUniform1i(samplerHandle, GL_TEXTURE0);
    checkGLerror("sending sampler");
    std::cout << "handle for uniform: " << samplerHandle << std::endl;
	//glGenerateMipmap(GL_TEXTURE_2D);
    //checkGLerror("mipmapping");
    */

    std::vector<GLfloat> fitsPixels;
    std::vector<GLfloat> texturePixels;
    long dim[2];
    loadFits(filename, fitsPixels, dim);
    enlargeTexturePower2(fitsPixels, texturePixels, dim[0], dim[1], 128, 128);




    // srand(1234);
    // Black/white checkerboard
    // std::vector<GLfloat> texturePixels;
    // for (int i=0; i<128; i++){
    //     for (int j=0; j<128; j++){
    //         texturePixels.push_back(j/128.0);
    //         texturePixels.push_back(0.0f);
    //         texturePixels.push_back(0.0f);
    //     }
    // }
    std::cout << "ntp = " << texturePixels.size() << std::endl;
    // float pixels[] = {
    //     0.0f, 0.0f, 0.0f,   1.0f, 1.0f, 1.0f,
    //     1.0f, 1.0f, 1.0f,   0.0f, 0.0f, 0.0f
    // };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 128, 128, 0, GL_RGB, GL_FLOAT, texturePixels.data());
    checkGLerror("TexImage2");



    // set up a rectangle to draw the 

    GLfloat v[42] = {
//  Position          Color               Texcoords
     -0.25f, 0.25f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f, // Top-left
      0.25f, 0.25f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f, // Top-right
     -0.25f, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f,  // Bottom-left

      0.25f,  0.25f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f, // Top-right
      0.25f,  0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // Bottom-right
     -0.25f,  0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f,  // Bottom-left

    // -10.0f, -10.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f,  // Bottom-left
    //  10.0f, -10.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // Bottom-right
    //  10.0f,  10.0f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f // Top-right
	};

	for (int i=0; i<42; i++) vertices.push_back(v[i]);

    // glBindVertexArray(vertexArrayObject);
    // checkGLerror("Bind VAO");
    // std::cout << "Have " << vertices.size() << " vertices to put up" << std::endl;
    // Send our data to the buffer
    // glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertices.size(), vertices.data(), GL_STATIC_DRAW);
    checkGLerror("BufferData");


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
