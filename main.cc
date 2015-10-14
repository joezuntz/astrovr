#include "avr_viewer.hh"
#include "avr_healpix.hh"
#include "avr_image.hh"
#include "avr_catalog.hh"
#include "avr_test.hh"
#include "avr_sphere.hh"

int main(int argc, char * argv[])
{
    glfwInit();
    GLFWwindow * window = avr_setup_window(800, 600);

    AVRViewer viewer(window);


    // Set up the Healpix map object
    int nside = 128;
    float radius = 0.9f;
    // AVRHealpix * hmap = new AVRHealpix(nside, radius);
    // hmap->load("map.fits");
    // hmap->createProgram("shaders/healpix/vertex.shader", "shaders/healpix/fragment.shader");

    // AVRTest * test = new AVRTest(0.0f);
    // test->createProgram("shaders/test/vertex.shader", "shaders/test/fragment.shader");
    // AVRTest * test2 = new AVRTest(-1.0f);
    // test2->createProgram("shaders/test/vertex.shader", "shaders/test/fragment.shader");

    AVRSphere * sphere = new AVRSphere(0.9);
    sphere->load("Fermi_3yr_stretch.png");
    sphere->createProgram("shaders/sphere/vertex.shader", "shaders/sphere/fragment.shader");


 //    AVRCatalog * catalog = new AVRCatalog();
	// catalog->load("cat.fits");
	// catalog->createProgram("shaders/catalog/vertex.shader", "shaders/catalog/fragment.shader");

    // AVRImage * img = new AVRImage();
    // img->createProgram("shaders/image/vertex.shader", "shaders/image/fragment.shader");
    // img->setupImage("gal.fits");
    // checkGLerror("after load_fits");


    //Add the healpix map to the scene
    // viewer.objects.push_back(catalog);
    // viewer.objects.push_back(hmap);
    // viewer.objects.push_back(test2);
    viewer.objects.push_back(sphere);
   	viewer.runLoop();

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}
