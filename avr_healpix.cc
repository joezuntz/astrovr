
#define GLM_FORCE_RADIANS
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>
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
    color_map = new JetColorMap(0.01, 50.0, true);
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

void AVRHealpix::load(const char * filename)
{
	// Read the map into RAM
    read_Healpix_map_from_fits(filename, fullResMap);

    lowResMap.Import(fullResMap, false);

    std::cout << "Using  " << npix << " vertices in map" << std::endl;

    //Locate the pixel vertices
    for (int i=0; i<npix; i++){
        std::vector<vec3> vd;
        HP.boundaries(i, 1, vd);
        // We now have four points, the four coordinates of the pixel edge.
        // we need to draw two triangles, ABC and ACD
        GLfloat rgb[3];
        (*color_map)(lowResMap[i], rgb);


        glm::vec4 v(rgb[0], rgb[1], rgb[2], 1.0f);
        push_healpix_triangle(vd[0], vd[1], vd[2], v, radius);
        push_healpix_triangle(vd[0], vd[2], vd[3], v, radius);
    }

    // Set our VAO as the active one
    glBindVertexArray(vertexArrayObject);
    checkGLerror("Bind VAO");
    std::cout << "Have " << vertices.size() << " vertices to put up" << std::endl;
    // Send our data to the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertices.size(), vertices.data(), GL_STATIC_DRAW);
    checkGLerror("BufferData");


    // Define the inputs of our shaders.
    // First is the pixel location
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0); 
    // Second input is the pixel color
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)(4*sizeof(GLfloat)));
    glEnableVertexAttribArray(1); 

    checkGLerror("Attrib");


}

void AVRHealpix::draw(glm::mat4 projection)
{
    useProgram();
    sendMatrix("projection", projection);    
    glDrawArrays(GL_TRIANGLES, 0, vertices.size()/3);
}