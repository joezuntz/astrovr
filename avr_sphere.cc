#include "avr_sphere.hh"
#include <png.h>
#include <iostream>
 
#define TEXTURE_LOAD_ERROR 0

AVRSphere::AVRSphere(double r) : radius(r) {

	glGenTextures(1, &texture);
    checkGLerror("generating texture");

	
    glBindTexture(GL_TEXTURE_2D, texture);
    checkGLerror("binding texture");

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    checkGLerror("setting min/mag");
    glGenBuffers(1, &elementBuffer);

}

void AVRSphere::setupSphere(){

	int rings = 20;
	int sectors = 20;

	// x,y,z points for sphere vertices
	float R = 1./(float)(rings-1);
	float S = 1./(float)(sectors-1);

	for(int r = 0; r < rings; r++){
		for(int s = 0; s < sectors; s++) {
			float y = sin( -M_PI_2 + M_PI * r * R );
			float x = cos(2*M_PI * s * S) * sin( M_PI * r * R );
			float z = sin(2*M_PI * s * S) * sin( M_PI * r * R );

			float u = s*S;
			float v = r*R;
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);
			vertices.push_back(u);
			vertices.push_back(v);


			indices.push_back(r * sectors + s);
			indices.push_back((r+1) * sectors + s);
			indices.push_back(r * sectors + (s+1));

			indices.push_back((r+1) * sectors + s);
			indices.push_back(r * sectors + (s+1));
			indices.push_back((r+1) * sectors + (s+1));

		}
	}
	for (int i=0; i<vertices.size()/5; i++) std::cout << "XXX   "
	 << vertices[5*i+0] << "   " 
	 << vertices[5*i+1] << "   " 
	 << vertices[5*i+2] << std::endl;


    glBindVertexArray(vertexArrayObject);
    checkGLerror("Bind VAO");

    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertices.size(), vertices.data(), GL_STATIC_DRAW);
    checkGLerror("BufferData");



    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*indices.size(), indices.data(), GL_STATIC_DRAW);

	GLsizei stride = 5*sizeof(GLfloat);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(3*sizeof(GLfloat)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);



}




AVRSphere::~AVRSphere()
{
	// delete the texture buffer here
}

void AVRSphere::load(const char * filename)
{

    setupSphere();

	//header for testing if it is a png
	png_byte header[8];

	//open file as binary
	FILE *fp = fopen(filename, "rb");
	if (!fp) {
		throw("failed to open file");
	}

	//read the header
	fread(header, 1, 8, fp);

	//test if png
	int is_png = !png_sig_cmp(header, 0, 8);
	if (!is_png) {
		fclose(fp);
		throw("file not a PNG file");
	}

	//create png struct
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
		NULL, NULL);
	if (!png_ptr) {
		fclose(fp);
		throw("could not create png");
	}

	//create png info struct
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
		fclose(fp);
		throw("could not create info");
	}

	//create png info struct
	png_infop end_info = png_create_info_struct(png_ptr);
	if (!end_info) {
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
		fclose(fp);
		throw("could not create end info");
	}

	//png error stuff, not sure libpng man suggests this.
	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		fclose(fp);
		throw("could not read png");
	}

	//init png reading
	png_init_io(png_ptr, fp);

	//let libpng know you already read the first 8 bytes
	png_set_sig_bytes(png_ptr, 8);

	// read all the info up to the image data
	png_read_info(png_ptr, info_ptr);

	//variables to pass to get info
	int bit_depth, color_type;
	png_uint_32 twidth, theight;

	// get info about png
	png_get_IHDR(png_ptr, info_ptr, &twidth, &theight, &bit_depth, &color_type,
		NULL, NULL, NULL);

	//update width and height based on png info
	width = twidth;
	height = theight;

	// Update the png info struct.
	png_read_update_info(png_ptr, info_ptr);

	// Row size in bytes.
	int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

	// Allocate the image_data as a big block, to be given to opengl
	png_byte *image_data = new png_byte[rowbytes * height];
	if (!image_data) {
		//clean up memory and close stuff
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		fclose(fp);
		throw("could not destroy png");
	}

	//row_pointers is for pointing to image_data for reading the png with libpng
	png_bytep *row_pointers = new png_bytep[height];
	if (!row_pointers) {
		//clean up memory and close stuff
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		delete[] image_data;
		fclose(fp);
		throw("could not read all data");
	}
	// set the individual row_pointers to point at the correct offsets of image_data
	for (int i = 0; i < height; ++i)
		row_pointers[height - 1 - i] = image_data + i * rowbytes;

	//read the png into image_data through row_pointers
	png_read_image(png_ptr, row_pointers);

	//Now generate the OpenGL texture object
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA, width, height, 0,
		GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*) image_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//clean up memory and close stuff
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
	delete[] image_data;
	delete[] row_pointers;
	fclose(fp);
}


void AVRSphere::draw(glm::mat4 projection)
{
    useProgram();
    checkGLerror("after useProgram");
	
	GLuint textureHandle = glGetUniformLocation(shaderProgram, "sphereTexture");
	glUniform1i(textureHandle, 0);

    sendMatrix("projection", projection);
    checkGLerror("after projection");

    checkGLerror("after sphere");
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, 0);

}