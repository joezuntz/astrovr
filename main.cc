#include "avr_viewer.hh"
#include "avr_healpix.hh"
#include "avr_image.hh"
#include "avr_catalog.hh"
#include "avr_test.hh"
#include "avr_sphere.hh"

#define N_IMAGE 8
AVRSphere *spheres[N_IMAGE];

const char * image_filenames[N_IMAGE] = {
    "../oculus/tiles/DSS-tiles.png",
    "../oculus/tiles/WISE-tiles.png",
    "../oculus/tiles/H-alpha-tiles.png",
    "../oculus/tiles/IRAS-tiles.png",
    "../oculus/tiles/fermi_3yr-tiles.png",
    "../oculus/tiles/Radio_vsmoothed-tiles.png",
    "../oculus/tiles/Planck-tiles.png",
    "../oculus/tiles/RASS-tiles.png",
};

class SkyViewer : public AVRViewer
{
public:
    SkyViewer(GLFWwindow * window);
    virtual void handleInput(int key, int scancode, int action, int mode);
    int quiet;
    int active_image;
};

SkyViewer::SkyViewer(GLFWwindow * window) : AVRViewer(window), quiet(0), active_image(0) {}

void SkyViewer::handleInput(int key, int scancode, int action, int mode)
{


    AVRViewer::handleInput(key, scancode, action, mode);


    // Stop multiple presses
    if (quiet>0) {
        quiet--;
        return;
    }
    if (key==GLFW_KEY_ENTER){
        active_image++;
    }
    else if (key==GLFW_KEY_RIGHT_SHIFT){
        active_image--;
    }
    else{
        return;
    }
    quiet=1;
    while(active_image<0) active_image+=N_IMAGE;
    active_image = (active_image%N_IMAGE);

    printf("Active image now = %d\n", active_image);

    for (int i=0; i<N_IMAGE; i++){
        spheres[i]->enabled = (i==active_image);
    }
}


int main(int argc, char * argv[])
{
    glfwInit();
    GLFWwindow * window = avr_setup_window(800, 600);

    SkyViewer viewer(window);


    // Set up the Healpix map object
    int nside = 128;
    float radius = 0.9f;

    // hmap = new AVRHealpix(nside, radius);
    // hmap->load("map.fits");
    // hmap->createProgram("shaders/healpix/vertex.shader", "shaders/healpix/fragment.shader");
    // hmap->enabled=false;

    // AVRTest * test = new AVRTest(0.0f);
    // test->createProgram("shaders/test/vertex.shader", "shaders/test/fragment.shader");
    // AVRTest * test2 = new AVRTest(-1.0f);
    // test2->createProgram("shaders/test/vertex.shader", "shaders/test/fragment.shader");



    for (int i=0; i<N_IMAGE; i++){
        spheres[i]= new AVRSphere(0.9);
        const char * filename = image_filenames[i];
        printf("Loading %s\n", filename);
        spheres[i]->load(filename);
        spheres[i]->createProgram("shaders/sphere/vertex.shader", "shaders/sphere/fragment.shader");
        spheres[i]->enabled = (i==0);
        printf("Sphere %d: %d\n",i,spheres[i]->enabled);
        viewer.objects.push_back(spheres[i]);
    }

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
    // viewer.objects.push_back(hmap);
   	viewer.runLoop();

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}
