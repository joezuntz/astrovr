#include "avr_viewer.hh"
#include "avr_healpix.hh"


int main(int argc, char * argv[])
{
    glfwInit();

    AVRViewer viewer(800, 600);
    viewer.setupWindow();


    // Set up the Healpix map object
    int nside = 128;
    float radius = 0.9f;
    AVRHealpix * hmap = new AVRHealpix(nside, radius);
    hmap->load("map.fits");
    hmap->createProgram("vertex.shader", "fragment.shader");

    //Add the healpix map to the scene
    viewer.objects.push_back(hmap);

   	viewer.runLoop();

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}
