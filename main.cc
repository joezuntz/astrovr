#include "avr_viewer.hh"
#include "avr_healpix.hh"
#include "avr_catalog.hh"


int main(int argc, char * argv[])
{
    glfwInit();

    AVRViewer viewer(800, 600);
    viewer.setupWindow();


    // Set up the Healpix map object
    // int nside = 128;
    // float radius = 0.9f;
    // AVRHealpix * hmap = new AVRHealpix(nside, radius);
    // hmap->load("map.fits");
    // hmap->createProgram("shaders/healpix/vertex.shader", "shaders/healpix/fragment.shader");

    AVRCatalog * catalog = new AVRCatalog();
	catalog->load("cat.fits");
	catalog->createProgram("shaders/catalog/vertex.shader", "shaders/catalog/fragment.shader");

    //Add the healpix map to the scene
    viewer.objects.push_back(catalog);

   	viewer.runLoop();

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}
