#include "avr_healpix.hh"
#include "healpix_map_fitsio.h"




AVRHealpix::AVRHealpix(int ns, float r) : 
    lowResMap(ns, RING, SET_NSIDE), 
    radius(r),
    nside(ns),
    HP(ns, RING, SET_NSIDE)
{
    npix = HP.Npix();
    order = (int)log2(ns);
    nring = 4*ns-1;
    //Set color map elsewhere
    color_map = new JetColorMap(0.1f, 6.0f, true);

    // Space for the indices of the vertices
    glGenBuffers(1, &elementBuffer);

}

AVRHealpix::~AVRHealpix(){
	delete color_map;
}

void AVRHealpix::push_healpix_triangle(vec3 &p1, vec3 &p2, vec3 &p3, glm::vec4 &col, GLfloat scale)
{
    glm::vec3 v1(p1.x, p1.y, p1.z);
    glm::vec3 v2(p2.x, p2.y, p2.z);
    glm::vec3 v3(p3.x, p3.y, p3.z);
    push_triangle(v1, v2, v3, col, scale);
}

void AVRHealpix::computeCornerIndices(std::vector<vec3> &corners, std::vector<GLuint> &elements)
{

    //Top of the top ring - just the North Pole.
    //There are 4 pixels on the top row
    int startpix, ringpix, ringpix0;  
    double theta;
    bool shifted;

    HP.get_ring_info2(1, startpix, ringpix0, theta, shifted);

    for (int p=startpix; p<startpix+ringpix0; p++){
        corners.push_back(vec3(0.0, 0.0, 1.0));
    }    


    for (int r=2; r<=nring; r++){
        HP.get_ring_info2(r, startpix, ringpix, theta, shifted);
        for (int p=startpix; p<startpix+ringpix; p++){
            // Get the four corners of the pixel.
            // If this turns out to be slow we can speed it up
            // by pulling out the bits of the boundaries method
            // that just do the top pixel
            std::vector<vec3> boundaries;
            HP.boundaries(p, 1, boundaries);
            // Get the top corner and save it
            corners.push_back(boundaries[0]);
        }
    }

    //For the bottom row the southern corner is the south pole
    // for (int p=0; p<ringpix0; p++){
        corners.push_back(vec3(0.0, 0.0, -1.0));
    // }


    

    for (int i=0; i<npix; i++){

        // First we do the top triangle.
        // What is the index in the vertices array of the top 
        // three corners of the i'th pixel?
        // The top corner is at index i.
        elements.push_back(i);

        // For the others we need to find the neighbouring pixels.
        // The ordering that comes out of this function is:
        // SW, W, NW, N, NE, E, SE and S
        fix_arr<int,8> neighbors;
        HP.neighbors(i, neighbors);

        // Then the remaining two vertices are the Norths of the
        // SE and SW pixels
        elements.push_back(neighbors[0]);
        elements.push_back(neighbors[6]);

        // Now we want the vertices of the lower triangle.
        // To have the same chirality as the first triangle
        // this must be in the order SE SE S
        elements.push_back(neighbors[6]);
        elements.push_back(neighbors[0]);

        // For the southern row the lowest neighbour does
        // not exist - South pole instead
        elements.push_back(neighbors[7]>=0 ? neighbors[7] : npix-1);
        // elements.push_back(neighbors[7]);

    }

}

void AVRHealpix::load(const char * filename)
{
	// Read the map into RAM
    read_Healpix_map_from_fits(filename, fullResMap);

    lowResMap.Import(fullResMap, false);

    std::cout << "Using  " << npix << " vertices in map" << std::endl;



    // Generate two vectors:
    // the list of vec3s representing the northern corners of the pixels
    // the list of indices into the first array that give you the corners of
    // a given pixel.  This re-uses corners for all the four pixels that surround them
    std::vector<vec3> corners;
    std::vector<GLuint> elements;
    computeCornerIndices(corners, elements);

    // We now want to send the top corners in along 
    //with the corresponding colors.  The remaining
    // corners we will send in using the element array
    for (int i=0; i<npix; i++){
        // Top corner of pixel
        vec3 corner = corners[i];
        // Color of pixel
        GLfloat rgb[3];
        (*color_map)(lowResMap[i], rgb);

        vertices.push_back(corner.x*radius);
        vertices.push_back(corner.y*radius);
        vertices.push_back(corner.z*radius);
        vertices.push_back(rgb[0]);
        vertices.push_back(rgb[1]);
        vertices.push_back(rgb[2]);

    }


    // Set our VAO as the active one
    glBindVertexArray(vertexArrayObject);
    checkGLerror("Bind VAO");
    std::cout << "Have " << vertices.size() << " vertices to put up" << std::endl;
    // Send our data to the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertices.size(), vertices.data(), GL_STATIC_DRAW);
    checkGLerror("BufferData");

    // Now send the indices into that array which specify the pixels to use
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*elements.size(), elements.data(), GL_STATIC_DRAW);


    // Define the inputs of our shaders.
    // First is the pixel location
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0); 
    // Second input is the pixel color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(1); 

    checkGLerror("Attrib");


}

void AVRHealpix::draw(glm::mat4 projection)
{
    useProgram();
    //glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);
    sendMatrix("projection", projection);    
    glDrawElements(GL_TRIANGLES, npix*6, GL_UNSIGNED_INT, 0);

}