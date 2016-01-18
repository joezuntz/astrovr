#include "avr_gl.hh"
#include "avr_sphere.hh"

struct TextureBuffer;
struct DepthBuffer;

class AVROculus
{
public:

	// The Head-Mounted Display and related information
	ovrSession hmd;
	ovrHmdDesc hmdDesc;
    ovrPosef eyePoses[2];
	ovrEyeRenderDesc eyeDescriptors[2];
	ovrVector3f offset[2];



	// The surfaces to which we render scenes
	ovrGraphicsLuid luid;
	TextureBuffer * eyeRenderTexture[2]; //contains the texture set pointers and other stuff
	DepthBuffer   * eyeDepthBuffer[2];

	// Layers - collections of textures - for each eye
	ovrLayerEyeFov ld;

	// The mirror texture used for the screen window
	ovrGLTexture  * mirrorTexture;
	GLuint          mirrorFBO = 0;

	//The window?
    int width, height;

	//Things in the scene
	std::vector<AVRObject*> objects;

    double startTime;


	void renderMirror();
	void reportError(const char * location);
	void setup(HINSTANCE hinst);
	void runLoop();
	void configureTextures();
	//void configureGLFW();
	void configureEyes();	
	void renderEye(ovrEyeType eye);
	//void setupHealpixMap(); //Temporary test method
	glm::mat4 projectionMatrix(ovrEyeType eye);
};