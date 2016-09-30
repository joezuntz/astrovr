#pragma once
#include "stdafx.h"

#include "avr_gl.hh"
#include "avr_sphere.hh"



// Current tours
#define PLANCK_MODE -1
#define MULTIWAVELENGTH_FREE 0
#define MULTIWAVELENGTH_LONG_TOUR 1


struct TextureBuffer;
struct DepthBuffer;

class AVROculusTimeline;
class AVRSoundPlayer;

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
	int currentObjectIndex;
	int targetObjectIndex;
	float fadeStartTime;
	float fadeTime;


	void renderMirror();
	void reportError(const char * location);
	void setup(HINSTANCE hinst, const wchar_t * windowTitle);
	void manualInput();
	void updateAlpha();
	void runLoop(int tour);
	void configureTextures();
	//void configureGLFW();
	void configureEyes();	
	void renderEye(ovrEyeType eye);
	//void setupHealpixMap(); //Temporary test method
	glm::mat4 projectionMatrix(ovrEyeType eye);
	glm::mat4 fixedTransform;

	glm::vec3 previousPosition;
	glm::vec3 targetPosition;
	float translationStartTime;
	float translationEndTime;
	int areWeTranslating;
	glm::mat4 additionalTranslation;

	glm::vec3 previousRotation;
	glm::vec3 targetRotation;
	float rotationStartTime;
	float rotationEndTime;
	int areWeRotating;
	glm::mat4 additionalRotation;


	void translateTo(double x, double y, double z);
	void transitionTo(int i);
	void rotateTo(double lat, double lon);
	void finishTour();
	void AVROculus::startTour(int tour_id);

	AVROculusTimeline * tour_timeline;
	int running_tour;
	AVRSoundPlayer * player;

};

