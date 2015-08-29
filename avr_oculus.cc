
#include "avr_oculus.hh"
#include <iostream>
#include <fstream>
#include <OpenGL/gl3.h>

void AVROculus::reportError(const char * location)
{

    // For new API:
    // ovrErrorInfo errorInfo;
    // ovr_GetLastErrorInfo(&errorInfo);
    // const char * message = errorInfo.ErrorString;
    const char * message = ovrHmd_GetLastError(hmd);
    std::cerr << "Oculus error while " << location << " : " << message << std::endl;
    throw message;

}


void AVROculus::configureGLFW()
{
    glfwInit();
    window = glfwCreateWindow(hmd->Resolution.w, hmd->Resolution.h, 
        "GLFW Oculus Rift Test", glfwGetPrimaryMonitor(), NULL);
    std::cout << "Window status: " << (window==NULL) << std::endl;

}

void AVROculus::configureTexture()
{
    // Configure Stereo settings.
    ovrSizei leftSize = ovrHmd_GetFovTextureSize(hmd, 
        ovrEye_Left, hmd->DefaultEyeFov[0],1.0f);
    ovrSizei rightSize = ovrHmd_GetFovTextureSize(hmd, 
        ovrEye_Right, hmd->DefaultEyeFov[1],1.0f);

    std::cout << "Left eye: << " << leftSize.w << " x " << leftSize.h  << std::endl;
    std::cout << "Right eye: << " << rightSize.w << " x " << rightSize.h  << std::endl;

    ovrSizei textureSize;
    textureSize.w  = leftSize.w + rightSize.w;
    textureSize.h = std::max(leftSize.h, rightSize.h);
    const int eyeRenderMultisample = 1;

    std::cout << "Texture: << " << textureSize.w << " x " << textureSize.h  << std::endl;

    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // Give an empty image to OpenGL ( the last "0" )
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 768, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 768);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
    // Set "renderedTexture" as our colour attachement #0
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);
 
    // Set the list of draw buffers.
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

    std::cout << "STATUS  " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;

    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
      std::cout << "FB size  " << w << "   " << h  << std::endl;
  


    // int width, height;
    // glfwGetFramebufferSize(window, &width, &height);
    union ovrGLConfig config;
    config.OGL.Header.BackBufferSize.w = w;
    config.OGL.Header.BackBufferSize.h = h;
// #if defined(_WIN32)
//     config.OGL.Window = glfwGetWin32Window(window);
// #elif defined(__APPLE__)
// #elif defined(__linux__)
//     config.OGL.Disp = glfwGetX11Display();
// #endif

    // Now create OpenGL texture

//    pRendertargetTexture = pRender->CreateTexture(
// Texture_RGBA | Texture_RenderTarget | eyeRenderMultisample,
// renderTargetSize.w, renderTargetSize.h, NULL);
//    // The actual RT size may be different due to HW limits.
//    renderTargetSize.w = pRendertargetTexture->GetWidth();
//    renderTargetSize.h = pRendertargetTexture->GetHeight();
}

void AVROculus::setup()
{

    ovrBool status = ovr_Initialize(NULL);
    if (!status) reportError("initializing");

    // Looks like this is changing
    hmd = ovrHmd_Create(0);
    if (!hmd){
        std::cout << "Creating a fake headset!" << std::endl;
        hmd = ovrHmd_CreateDebug(ovrHmd_DK2);
    }
    if (!hmd) reportError("creating the headset");
    std::cout << "Detected Oculus!" << std::endl << "ProductName: " << hmd->ProductName << std::endl;
    std::cout << "Manufacturer: "  << hmd->Manufacturer << std::endl;

    // Now we can enable head tracking.
    unsigned int trackMode = ovrTrackingCap_Orientation
                           | ovrTrackingCap_MagYawCorrection
                           | ovrTrackingCap_Position;

    status = ovrHmd_ConfigureTracking(hmd, trackMode, 0);
    if (!status) reportError("getting track mode");

    configureGLFW();
    configureTexture();


//  ovrEyeRenderDesc eyeDescriptors[2];
//  // Configure OpenGL.
//  ovrGLConfig cfg;
//  cfg.OGL.Header.API         = ovrRenderAPI_OpenGL;
//  cfg.OGL.Header.RTSize      = Sizei(hmd->Resolution.w, hmd->Resolution.h);
//  cfg.OGL.Header.Multisample = backBufferMultisample;
//  cfg.OGL.DC                 = dc;

//  //Default FOVs
//  ovrFovPort eyeFOV[2];
//  eyeFOV[0].UpTan = 1.0;
//  eyeFOV[0].DownTan = 1.0;
//  eyeFOV[0].LeftTan = 1.0;
//  eyeFOV[0].RightTan = 1.0;
//  eyeFOV[1].UpTan = 1.0;
//  eyeFOV[1].DownTan = 1.0;
//  eyeFOV[1].LeftTan = 1.0;
//  eyeFOV[1].RightTan = 1.0;

//  status = ovrHmd_ConfigureRendering(hmd, &cfg.Config, distortionCaps,
//                                                  eyesFov, eyeDescriptors);

//  status = ovrHmd_ConfigureRendering(ovrHmd hmd,
// const ovrRenderAPIConfig* apiConfig,
// unsigned int distortionCaps,
//                                                const ovrFovPort eyeFovIn[2],
//                                                ovrEyeRenderDesc eyeRenderDescOut[2] );



}



void AVROculus::runLoop(){
    while (1){

        double now = ovr_GetTimeInSeconds();
        ovrTrackingState tracking = ovrHmd_GetTrackingState(hmd, now);
        if (tracking.StatusFlags & 
            (ovrStatus_OrientationTracked | ovrStatus_PositionTracked )){
            
            ovrQuatf dir = tracking.HeadPose.ThePose.Orientation;
            std::cout << now << "  " << dir.x << "  " << dir.y << "  " 
            << dir.z << "  " << dir.w << std::endl;
        }
        else{
            std::cerr << "Tracking fail: " << tracking.StatusFlags << "  " << 
            ovrStatus_OrientationTracked << "  " << ovrStatus_PositionTracked <<
            std::endl;
        }

    }
}


int main(int argc, char * argv[]){
    AVROculus oculus;
    oculus.setup();
    // oculus.runLoop();

}