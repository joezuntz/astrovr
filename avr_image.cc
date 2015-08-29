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

void AVRImage::loadFits(const char * filename, std::vector<float> &pixel_data)
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
    long dim[2];
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

    srand(1234);
    // Black/white checkerboard
    std::vector<GLfloat> texturePixels;
    for (int i=0; i<16; i++){
        for (int j=0; j<16; j++){
            texturePixels.push_back(j/16.0);
            texturePixels.push_back(0.0f);
            texturePixels.push_back(0.0f);
        }
    }
    std::cout << "ntp = " << texturePixels.size() << std::endl;
    // float pixels[] = {
    //     0.0f, 0.0f, 0.0f,   1.0f, 1.0f, 1.0f,
    //     1.0f, 1.0f, 1.0f,   0.0f, 0.0f, 0.0f
    // };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 16, 16, 0, GL_RGB, GL_FLOAT, texturePixels.data());
    checkGLerror("TexImage2");



    // set up a rectangle to draw the 

    GLfloat v[21] = {
//  Position          Color               Texcoords
    -1.0f,  -0.5f,   1.0f, 0.0f, 1.0f,   0.0f, 0.0f, // Top-left
     1.0f,  0.4f,   1.0f, 0.0f, 1.0f,   1.0f, 0.0f, // Top-right
    -1.0f, -0.4f,   1.0f, 0.0f, 1.0f,   0.0f, 1.0f,  // Bottom-left

    // -10.0f, -10.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f,  // Bottom-left
    //  10.0f, -10.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // Bottom-right
    //  10.0f,  10.0f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f // Top-right
	};

	for (int i=0; i<21; i++) vertices.push_back(v[i]);

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
    glm::mat4 projection2;
    std::cout << glm::to_string(projection2) << std::endl<< std::endl;
    sendMatrix("projection", projection2);
    checkGLerror("before drawarrays");
    glDrawArrays(GL_TRIANGLES, 0, vertices.size()/7);
    checkGLerror("after drawarrays");

}
