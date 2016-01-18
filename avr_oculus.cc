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



void avr_gl_errorcheck(const char * where)
{
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
	throw message;

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

void AVROculus::setup(HINSTANCE hinst)
{
	ovrResult result = ovr_Create(&hmd, &luid);
	if (!OVR_SUCCESS(result)) reportError("initializing");

	hmdDesc = ovr_GetHmdDesc(hmd);
	width = hmdDesc.Resolution.w/2;
	height = hmdDesc.Resolution.h/2;

	Platform.InitWindow(hinst, L"The Multi-Wavelength Universe");
	Platform.InitDevice(width, height, reinterpret_cast<LUID*>(&luid));
	glDisable(GL_CULL_FACE);


	configureTextures();
	configureEyes();
	wglSwapIntervalEXT(0);


}



/*
void AVROculus::setup()
{

	ovrResult result = ovr_Initialize(nullptr);
	if (!OVR_SUCCESS(result)) reportError("initializing");


	result = ovr_Create(&hmd, &luid);
	if (!OVR_SUCCESS(result)) reportError("initializing");

	hmdDesc = ovr_GetHmdDesc(hmd);


    if (!hmd) reportError("creating the headset");
    std::cout << "Detected Oculus!" << std::endl << "ProductName: " << hmdDesc.ProductName << std::endl;
    std::cout << "Manufacturer: "  << hmdDesc.Manufacturer << std::endl;

    width = hmdDesc.Resolution.w;
    height = hmdDesc.Resolution.h;



    // Now we can enable head tracking.
    unsigned int trackMode = ovrTrackingCap_Orientation
                           | ovrTrackingCap_MagYawCorrection
                           | ovrTrackingCap_Position;

    result = ovr_ConfigureTracking(hmd, trackMode, 0);
	if (!OVR_SUCCESS(result)) reportError("getting track mode");



    configureGLFW();

	eyeDescriptors[0] = ovr_GetRenderDesc(hmd, ovrEye_Left, hmdDesc.DefaultEyeFov[0]);
	eyeDescriptors[1] = ovr_GetRenderDesc(hmd, ovrEye_Right, hmdDesc.DefaultEyeFov[1]);


    std::cout << "rendering configured." <<std::endl;

    avr_gl_errorcheck("setting up objects");
    configureTextures();
    avr_gl_errorcheck("texture");
    configureEyes(); //connect the eyes to the texture
    avr_gl_errorcheck("eyes");



}
*/


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
	//glm::mat4 scl = glm::scale(glm::mat4(), glm::vec3(scale_factor, scale_factor, scale_factor));
	glm::mat4 model;
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 reflect = glm::mat4
		(
			-1.0, 0.0, 0.0, 0.0,
			0.0, 1.0, 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			0.0, 0.0, 0.0, 1.0
			);
	model = reflect*model;
	model *= scale_factor;
	glm::mat4 projection_output = OVRToGLMat4(projection*view)*model;
	return projection_output;

}


/*

glm::mat4 AVROculus::projectionMatrix(ovrEyeType eye){

    ovrPosef pose = eyePoses[eye];
    double timeSinceStart = ovr_GetTimeInSeconds() - startTime;
    float rotationRate = 10.0f; // degrees persecond
    float rotAngle = rotationRate * timeSinceStart;

    // Model.  We just center it at zero    

    glm::mat4 rot1 = glm::rotate(glm::mat4(), glm::radians(90.0f),   glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 rot2 = glm::rotate(glm::mat4(), glm::radians(-90.0f),    glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 rot3 = glm::rotate(glm::mat4(), glm::radians(rotAngle), glm::vec3(0.0f, 0.0f, 1.0f));
    // glm::mat4 tran = glm::translate(glm::mat4(), glm::vec3(2.0f, 0.0f, 0.0f));
    // glm::mat4 tran = glm::mat4();
    float scale_factor = 10.0f;
    glm::mat4 scl = glm::scale(glm::mat4(), glm::vec3(scale_factor,scale_factor,scale_factor));

    glm::mat4 model = scl*rot2*rot1;


    glm::mat4 view = glm::inverse(makeMatrixFromPose(pose));


    ovrMatrix4f p1 = ovrMatrix4f_Projection(eyeDescriptors[eye].Fov,0.01f, 1000.0f, true);
    glm::mat4 projection = OVRToGLMat4(p1);
    projection = projection*view*model;
    return projection;


}
*/

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


void AVROculus::runLoop(){



    startTime = ovr_GetTimeInSeconds();

	while (Platform.HandleMessages()){
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
		if (found_key >= 0) {
			for (int i = 0; i < n_sphere; i++) {
				AVRSphere * sphere = (AVRSphere*)objects[i];
				if (i == found_key) {
					sphere->alpha = 1.0f;
				}
				else {
					sphere->alpha = 0.0f;
				}

			}

		}

		//Track the motion of the head
		double           sensorSampleTime = ovr_GetTimeInSeconds();
		double           ftiming = ovr_GetPredictedDisplayTime(hmd, 0);
		ovrTrackingState tracking = ovr_GetTrackingState(hmd, ftiming, ovrTrue);

		//Calculate the positions of the eyes
		ovr_CalcEyePoses(tracking.HeadPose.ThePose, offset, eyePoses);
		ld.SensorSampleTime = sensorSampleTime;

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



