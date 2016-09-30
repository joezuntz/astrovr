// #define GLEW_STATIC
// #include <GL/glew.h>


#include "avr_oculus.hh"
#define OVR_OS_WIN32


#include "avr_gl.hh"

#include <iostream>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "avr_oculus_timeline.h"
#include "avr_soundplayer.h"



void avr_gl_errorcheck(const char * where)
{
#pragma message("Removed GL error check")
	return;
	GLuint err = glGetError();
	if (err != GL_NO_ERROR) {
		const GLubyte* err_string = gluErrorString(err);
		throw (char*)err_string;
		std::cerr << "ERROR at " << where << "  " << err_string << std::endl;
		exit(err);
	}
}


void AVROculus::reportError(const char * location)
{

	// For new API:
	ovrErrorInfo errorInfo;
	ovr_GetLastErrorInfo(&errorInfo);
	const char * message = errorInfo.ErrorString;
	std::cerr << "Oculus error while " << location << " : " << message << std::endl;
	throw location;

}


/*
void AVROculus::configureGLFW()
{

    window = avr_setup_window(width, height);

    std::cout << "Window status: " << (window==NULL) << std::endl;
    const char* version = (const char*)glGetString(GL_VERSION);
    if (version) std::cout << "GL Version: " << version << std::endl;
    else std::cerr << "Arg GL Version NULL!" << std::endl;

}
*/


static glm::vec3 getEulerAngles(const ovrQuatf & in1) {
	OVR::Quat<float> in(in1);
	glm::vec3 eulerAngles;

	in.GetEulerAngles<OVR::Axis_X, OVR::Axis_Y, OVR::Axis_Z, OVR::Rotate_CW, OVR::Handed_R>
		(&eulerAngles.x, &eulerAngles.y, &eulerAngles.z);

	return eulerAngles;
}

static glm::quat fromOvr(const ovrQuatf & in) {
	return glm::quat(getEulerAngles(in));
}


static glm::mat4 OVRToGLMat4(ovrMatrix4f m1)
{
	OVR::Matrix4f m(m1);
	glm::mat4 ret;
	m.Transpose();
	memcpy(&ret, &m, sizeof(OVR::Matrix4f));
	return ret;
}


glm::mat4 makeMatrixFromPose(const ovrPosef& eyePose)
{
    const ovrVector3f& p = eyePose.Position;
    const ovrQuatf& q = eyePose.Orientation;
    return glm::translate(glm::mat4(1.f), glm::vec3(p.x, p.y, p.z))
        * glm::mat4_cast(glm::quat(q.w, q.x, q.y, q.z));
}


void AVROculus::configureTextures()
{
	ld.Header.Type = ovrLayerType_EyeFov;
	ld.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft;   // Because OpenGL.

	for (int eye = 0; eye < 2; ++eye)
	{

		ovrSizei idealTextureSize = ovr_GetFovTextureSize(hmd, ovrEyeType(eye), hmdDesc.DefaultEyeFov[eye], 1);
		eyeRenderTexture[eye] = new TextureBuffer(hmd, true, true, idealTextureSize, 1, NULL, 1);
		eyeDepthBuffer[eye] = new DepthBuffer(eyeRenderTexture[eye]->GetSize(), 0);
		ld.ColorTexture[eye] = eyeRenderTexture[eye]->TextureSet;
		ld.Viewport[eye] = OVR::Recti(idealTextureSize);
		ld.Fov[eye] = hmdDesc.DefaultEyeFov[eye];
	}

	ovrResult result = ovr_CreateMirrorTextureGL(hmd, GL_SRGB8_ALPHA8, width, height, reinterpret_cast<ovrTexture**>(&mirrorTexture));
	if (!OVR_SUCCESS(result)) throw "Error creating mirror texture";
	// Configure the mirror read buffer
	glGenFramebuffers(1, &mirrorFBO);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, mirrorFBO);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mirrorTexture->OGL.TexId, 0);
	glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);



}

// Get the horizontal and vertical screen sizes in pixel
void GetDesktopResolution(int& horizontal, int& vertical)
{
	RECT desktop;
	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	horizontal = desktop.right;
	vertical = desktop.bottom;
}


void AVROculus::setup(HINSTANCE hinst, const wchar_t * windowTitle)
{
	ovrResult result = ovr_Create(&hmd, &luid);
	if (!OVR_SUCCESS(result)) reportError("initializing");

	hmdDesc = ovr_GetHmdDesc(hmd);

	GetDesktopResolution(width, height);

	Platform.InitWindow(hinst, windowTitle);
	Platform.InitDevice(width, height, reinterpret_cast<LUID*>(&luid));
	glDisable(GL_CULL_FACE);
	//glEnable(GL_DEPTH_TEST);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_ONE, GL_ONE);
	//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
	checkGLerror("Enable");


	configureTextures();
	configureEyes();
	wglSwapIntervalEXT(0);


}





void AVROculus::configureEyes(){

	eyeDescriptors[0] = ovr_GetRenderDesc(hmd, ovrEye_Left, hmdDesc.DefaultEyeFov[0]);
	eyeDescriptors[1] = ovr_GetRenderDesc(hmd, ovrEye_Right, hmdDesc.DefaultEyeFov[1]);
	offset[0] = eyeDescriptors[0].HmdToEyeViewOffset;
	offset[1] = eyeDescriptors[1].HmdToEyeViewOffset;





}

glm::mat4 AVROculus::projectionMatrix(ovrEyeType eye) {
	static float Yaw(3.141592f);

	Matrix4f finalRollPitchYaw = Matrix4f(eyePoses[eye].Orientation);
	Vector3f finalUp = finalRollPitchYaw.Transform(Vector3f(0, 1, 0));
	Vector3f finalForward = finalRollPitchYaw.Transform(Vector3f(0, 0, -1));
	Vector3f shiftedEyePos = eyePoses[eye].Position;
	Matrix4f view = Matrix4f::LookAtRH(shiftedEyePos, shiftedEyePos + finalForward, finalUp);
	Matrix4f projection = ovrMatrix4f_Projection(hmdDesc.DefaultEyeFov[eye], 0.2f, 100.0f, ovrProjection_RightHanded);
	float scale_factor = 1.0f;

	glm::mat4 model = scale_factor*additionalRotation*additionalTranslation*fixedTransform;
	glm::mat4 projection_output = OVRToGLMat4(projection*view)*model;
	return projection_output;

}



void AVROculus::renderEye(ovrEyeType eye){

	//Oculus creates a set of textures and not just one so it can do a rolling render process (I think)
	//This moves us to the next texture in the stack
	eyeRenderTexture[eye]->TextureSet->CurrentIndex = (eyeRenderTexture[eye]->TextureSet->CurrentIndex + 1) % eyeRenderTexture[eye]->TextureSet->TextureCount;

	// Enable rendering to this eye
	eyeRenderTexture[eye]->SetAndClearRenderSurface(eyeDepthBuffer[eye]);

	//Tell the layer the new eye orientation.
	//Not sure why this is necessary. Why does it care?
	ld.RenderPose[eye] = eyePoses[eye];

	glm::mat4 projection = projectionMatrix(eye);

    // Draw the scene here.

	//Clear the scene
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//And draw the object(s).
	for (AVRObject * object : objects) {
		object->drawIfVisible(projection);
	}


	//Clear the choice of render surface - clean up
	eyeRenderTexture[eye]->UnsetRenderSurface();


}

void AVROculus::renderMirror() {

	// Blit mirror texture to back buffer
	glBindFramebuffer(GL_READ_FRAMEBUFFER, mirrorFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	GLint w = mirrorTexture->OGL.Header.TextureSize.w;
	GLint h = mirrorTexture->OGL.Header.TextureSize.h;
	glBlitFramebuffer(0, h, w, 0,
		0, 0, w, h,
		GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	SwapBuffers(Platform.hDC);

}

void AVROculus::updateAlpha() {
	size_t n_sphere = objects.size();
	double           sensorSampleTime = ovr_GetTimeInSeconds();

	//Blank out all the objects to start with.
	for (int i = 0; i < n_sphere; i++) {
		AVRObject * obj = objects[i];
		obj->setAlpha(0.0f);
	}

	float timeSinceFadeStart = sensorSampleTime - fadeStartTime;
	AVRObject * currentObject = objects[currentObjectIndex];
	AVRObject * targetObject = objects[targetObjectIndex];

	if (currentObjectIndex == targetObjectIndex) {
		//fade complete, one object
		currentObject->setAlpha(1.0f);
	}
	else {
		float phase = timeSinceFadeStart / fadeTime;

		if (phase>1.0f) {
			//fade just finished.
			targetObject->setAlpha(1.0f);
			currentObjectIndex = targetObjectIndex;
		}
		else {
			//Fading part way between
			currentObject->setAlpha(1.0f - phase);
			targetObject->setAlpha(phase);
		}
	}


	if (areWeTranslating) {
		float f = (sensorSampleTime - translationStartTime) / (translationEndTime - translationStartTime);
		if (f > 1.0f) {
			previousPosition = targetPosition;
			areWeTranslating = 0;
		}
		else {
			float g = sinf(f * M_PI_2);
			glm::vec3 currentPosition = previousPosition + g * (targetPosition - previousPosition);
			additionalTranslation = glm::translate(glm::mat4(1.f), currentPosition);

		}

	}


	if (areWeRotating) {
		float f = (sensorSampleTime - rotationStartTime) / (rotationEndTime - rotationStartTime);
		if (f < 0) f = 0;
		if (f > 1.0f) {
			previousRotation = targetRotation;
			areWeRotating = 0;
		}
		else {
			float g = sinf(f * M_PI_2);
			glm::vec3 z0 = glm::vec3(-1.0f, 0.0f, 0.0f);

			glm::vec3 rotationAxis1 = glm::cross(previousRotation, targetRotation);
			rotationAxis1 /= glm::length(rotationAxis1);
			float angl1 = g * acos(glm::dot(previousRotation, targetRotation));
			glm::vec3 currentRotation = glm::rotate(previousRotation, angl1, rotationAxis1);


			float finalRotationAngle = acos(glm::dot(currentRotation, z0));
			glm::vec3 rotationAxis = glm::cross(z0, currentRotation);
			if (z0 == currentRotation) {
				additionalRotation = glm::mat4(1.f);

			}
			else {
				additionalRotation = glm::rotate(glm::mat4(1.f), finalRotationAngle, rotationAxis);

			}

		}

	}


	//glm::vec3 position_shift = glm::vec3(0.0f, 0.0f, 0.0f);
	//glm::mat4 translate = glm::translate(glm::mat4(1.f), position_shift);
	//model = model*translate;
}

void AVROculus::translateTo(double x, double y, double z)
{
	double currentTime = ovr_GetTimeInSeconds();

	targetPosition = glm::vec3(-x,-y,-z);


	translationStartTime = currentTime;
	translationEndTime = currentTime + 0.5f;
	areWeTranslating = 1;


}
void AVROculus::transitionTo(int i) {
	targetObjectIndex = i-1;
	fadeStartTime = ovr_GetTimeInSeconds();

}
void AVROculus::rotateTo(double lat, double lon) {

	double currentTime = ovr_GetTimeInSeconds();
	double x = -cos(lon) * cos(lat);
	double y = -sin(lat);
	double z = -cos(lat)*sin(lon);

	targetRotation = glm::vec3(x, y, z);
	rotationStartTime = currentTime;
	rotationEndTime = currentTime + 0.5f;
	areWeRotating= 1;


}


void AVROculus::manualInput() {

	double sensorSampleTime = ovr_GetTimeInSeconds();

	int found_key = -1;
	size_t n_sphere = objects.size();
	if (n_sphere > 9) throw "Too many spheres";
	for (int i = 0; i < n_sphere; i++) {
		const char * keys = "1234567890";
		char key = keys[i];
		if (Platform.Key[key]) {
			found_key = i;
			break;
		}
	}
	//ignore flips during transition
	if (found_key >= 0 && (currentObjectIndex == targetObjectIndex)) {
		for (int i = 0; i < n_sphere; i++) {
			if (i == found_key) {
				targetObjectIndex = i;
				fadeStartTime = sensorSampleTime;
			}
		}
	}

	if (found_key == -1) {
		if (Platform.Key['P']) {
			startTour(MULTIWAVELENGTH_LONG_TOUR);
		}
	}

}

void AVROculus::startTour(int tour_id) {
	if (running_tour) return; //skip if already running
	if (tour_id==MULTIWAVELENGTH_LONG_TOUR) {
		tour_timeline = createLongTourTimeline(this);
		player = new AVRSoundPlayer(L"C:\\Users\\Joe Zuntz\\Music\\oculucfulltourwithmusicmono.wav");
		player->play();
	}
	// CAN ADD OTHER TOURS I HERE WITH DIFFERENT TIMELINES AND SOUND FILES

	else {//nothing to do
		delete tour_timeline;
		delete player;
		tour_timeline = nullptr;
		player = nullptr;
	}
	running_tour = tour_id;

}

void AVROculus::finishTour() {
	running_tour = MULTIWAVELENGTH_FREE; // == 0
	delete tour_timeline;
	tour_timeline = nullptr;
}


void AVROculus::runLoop(int tour){
	running_tour = 0;
	player = nullptr;

	// The sound that gets played is hard-coded in the avr_soundplayer.cpp file.

	startTour(tour); // may be nothing, empty tour ("FREE")

	startTime = ovr_GetTimeInSeconds();
	currentObjectIndex = 2;
	targetObjectIndex = 2;
	fadeStartTime = 0.0f;
	fadeTime = 0.75f;
	previousPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	previousRotation = glm::vec3(-1.0f, 0.0f, 0.0f);
	additionalTranslation = glm::mat4(1.f);
	additionalRotation = glm::mat4(1.f);

	while (Platform.HandleMessages()){
		double currentTime = ovr_GetTimeInSeconds();
		if (running_tour>0) {
			tour_timeline->check_for_event(currentTime - startTime);
		}
		else {
			manualInput();
		}

		updateAlpha();

		//Track the motion of the head
		double           ftiming = ovr_GetPredictedDisplayTime(hmd, 0);
		ovrTrackingState tracking = ovr_GetTrackingState(hmd, ftiming, ovrTrue);

		//Calculate the positions of the eyes
		ovr_CalcEyePoses(tracking.HeadPose.ThePose, offset, eyePoses);
		ld.SensorSampleTime = currentTime;

		renderEye(ovrEye_Right);
		renderEye(ovrEye_Left);

		ovrLayerHeader* layers = &ld.Header;
		ovrResult result = ovr_SubmitFrame(hmd, 0, NULL, &layers, 1);
		if (!OVR_SUCCESS(result)) {
			throw "submission error";
		}

		renderMirror();
    }


}



