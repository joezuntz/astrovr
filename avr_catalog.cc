#include "avr_gl.Hh"

//#define GLM_FORCE_RADIANS
//// GLEW
//#define GLEW_STATIC
//#include <GL/glew.h>
//
//// GLFW
//#include <GLFW/glfw3.h>
#include "avr_catalog.hh"

#include <iostream>
#include <fstream>
#include "fitsio.h"


AVRCatalog::AVRCatalog()
{

}



void AVRCatalog::push_point(float ra, float dec, float size, float d)
{
	float s, x, y, z;
	ra = glm::radians(ra);
	dec = glm::radians(dec);

	// all points are white for now
	s =     cosf(dec);
	x = d * cosf(ra) * s;
	y = d * sinf(ra) * s;
	z = d * sinf(dec);

	// Position
	vertices.push_back(x);
	vertices.push_back(y);
	vertices.push_back(z);

	// Size
	vertices.push_back(size);

	//Colour
	vertices.push_back(1.0f);
	vertices.push_back(1.0f);
	vertices.push_back(1.0f);
	vertices.push_back(1.0f);

}

void AVRCatalog::load(const char * filename)
{
    glBindVertexArray(vertexArrayObject);
    checkGLerror("Bind VAO");

    //Open the FITS file
    int status = 0;
    fitsfile * fits;
    fits_open_file(&fits, filename, READONLY, &status);
    checkFitsStatus(status, "opening file");

    // Go to the second HDU, which is the first place a table can be
	fits_movabs_hdu(fits, 2, NULL, &status);
    checkFitsStatus(status, "going to fits (table) extension");


  	long nrows;
    fits_get_num_rows(fits, &nrows, &status);
    checkFitsStatus(status, "counting rows");

    std::vector<float> ra(nrows);
    std::vector<float> dec(nrows);
    std::vector<float> size(nrows);

    int ra_column, dec_column, size_column;
	fits_get_colnum(fits, CASEINSEN, "ra", &ra_column, &status);
    checkFitsStatus(status, "finding ra column");
	fits_get_colnum(fits, CASEINSEN, "dec", &dec_column, &status);
    checkFitsStatus(status, "finding dec column");
	fits_get_colnum(fits, CASEINSEN, "size", &size_column, &status);
    checkFitsStatus(status, "finding size column");

 	fits_read_col(fits, TFLOAT,  ra_column, 1, 1, (LONGLONG) nrows, NULL, ra.data(), NULL, &status);
    checkFitsStatus(status, "reading ra column");

 	fits_read_col(fits, TFLOAT,  dec_column, 1, 1, (LONGLONG) nrows, NULL, dec.data(), NULL, &status);
    checkFitsStatus(status, "reading dec column");

 	fits_read_col(fits, TFLOAT,  size_column, 1, 1, (LONGLONG) nrows, NULL, size.data(), NULL, &status);
    checkFitsStatus(status, "reading size column");

    fits_close_file(fits, &status);
    checkFitsStatus(status, "closing fits file");

    // Convert ra, dec, r to x, y, z
    // into the vertices array
    for (int i=0; i<nrows; i++){
    	// ra, dec, size, distance
    	push_point(ra[i], dec[i], size[i], 0.5);
    }



    std::cout << "Have " << vertices.size()/8 << " points to plot in catalog" << std::endl;

    // Send our data to the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertices.size(), vertices.data(), GL_STATIC_DRAW);
    checkGLerror("BufferData");


    // Define the inputs of our shaders.
    // First is the point location
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0); 
    // Second input is the point color
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)(4*sizeof(GLfloat)));
    glEnableVertexAttribArray(1); 
}


void AVRCatalog::draw(glm::mat4 projection)
{


    useProgram();
    sendMatrix("projection", projection);    
    glDrawArrays(GL_POINTS, 0, (GLsizei)vertices.size());

}

