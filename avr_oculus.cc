// #define GLEW_STATIC
// #include <GL/glew.h>

#include "avr_oculus.hh"
#include <iostream>
#include <fstream>



void avr_gl_errorcheck(const char * where)
{
    GLuint err = glGetError();
    if (err!=GL_NO_ERROR){
        const GLubyte* err_string = gluErrorString(err);
        std::cerr << "ERROR at " << where << "  " << err_string<< std::endl;
        exit(err);
    }
}


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

    window = avr_setup_window(width, height);

    std::cout << "Window status: " << (window==NULL) << std::endl;
    const char* version = (const char*)glGetString(GL_VERSION);
    if (version) std::cout << "GL Version: " << version << std::endl;
    else std::cerr << "Arg GL Version NULL!" << std::endl;

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

    allocateFBO(eyeFBOs[ovrEye_Left], leftSize.w, leftSize.h);
    allocateFBO(eyeFBOs[ovrEye_Right], rightSize.w, rightSize.h);

    // ovrSizei textureSize;
    // textureSize.w  = leftSize.w + rightSize.w;
    // textureSize.h = std::max(leftSize.h, rightSize.h);
    // const int eyeRenderMultisample = 1;

    // avr_gl_errorcheck("start of texture");

    // std::cout << "Texture: << " << textureSize.w << " x " << textureSize.h  << std::endl;

    // glGenFramebuffers(1, &frameBuffer);
    // glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);





    // glGenTextures(1, &texture);


    // // Give an empty image to OpenGL ( the last "0" )
    // glBindTexture(GL_TEXTURE_2D, texture);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 768, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


    // // Depth attachment.
    // glGenRenderbuffers(1, &depthBuffer);
    // glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 768);
    // glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
    // glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);
    // GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    // glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers


    // std::cout << "STATUS  " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << "  where good = " << GL_FRAMEBUFFER_COMPLETE << std::endl;

    // int w, h;
    // glfwGetFramebufferSize(window, &w, &h);
    //   std::cout << "FB size  " << w << "   " << h  << std::endl;
  


    // int width, height;
    // glfwGetFramebufferSize(window, &width, &height);
    // union ovrGLConfig config;
    // config.OGL.Header.BackBufferSize.w = w;
    // config.OGL.Header.BackBufferSize.h = h;
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

    width = hmd->Resolution.w;
    height = hmd->Resolution.h;



    // Now we can enable head tracking.
    unsigned int trackMode = ovrTrackingCap_Orientation
                           | ovrTrackingCap_MagYawCorrection
                           | ovrTrackingCap_Position;

    status = ovrHmd_ConfigureTracking(hmd, trackMode, 0);
    if (!status) reportError("getting track mode");



    configureGLFW();


    // union ovrGLConfig config;
    // config.OGL.Header.BackBufferSize.w = w;
    // config.OGL.Header.BackBufferSize.h = h;


    ovrRenderAPIConfig cfg = ovrRenderAPIConfig();
    cfg.Header.API = ovrRenderAPI_OpenGL;
    cfg.Header.BackBufferSize.w = width;
    cfg.Header.BackBufferSize.h = height;
    cfg.Header.Multisample = 1;

    std::cout << "cfg created!" <<std::endl;

 // ovrGLConfig cfg;
 // cfg.OGL.Header.API         = ovrRenderAPI_OpenGL;
 // cfg.OGL.Header.RTSize      = Sizei(hmd->Resolution.w, hmd->Resolution.h);
//  cfg.OGL.Header.Multisample = backBufferMultisample;
//  cfg.OGL.DC                 = dc;


    
    status = ovrHmd_ConfigureRendering(hmd, &cfg, 0, hmd->DefaultEyeFov, eyeDescriptors);


    std::cout << "rendering configured." <<std::endl;

    setupTriangle();
    avr_gl_errorcheck("triangle");
    configureTexture();
    avr_gl_errorcheck("texture");
    configureEyes(); //connect the eyes to the texture
    avr_gl_errorcheck("eyes");


//  status = ovrHmd_ConfigureRendering(ovrHmd hmd,
// const ovrRenderAPIConfig* apiConfig,
// unsigned int distortionCaps,
//                                                const ovrFovPort eyeFovIn[2],
//                                                ovrEyeRenderDesc eyeRenderDescOut[2] );

}

void AVROculus::configureEyes(){
    ovrGLTexture& leftEyeTexture = eyeTextures[ovrEye_Left];
    ovrGLTextureData& leftEyeData = leftEyeTexture.OGL;
    ovrTextureHeader& leftEyeHeader = leftEyeData.Header;

    leftEyeHeader.API = ovrRenderAPI_OpenGL;
    leftEyeHeader.TextureSize.w = width;
    leftEyeHeader.TextureSize.h = height;
    leftEyeHeader.RenderViewport.Pos.x = 0;
    leftEyeHeader.RenderViewport.Pos.y = 0;
    leftEyeHeader.RenderViewport.Size.w = width / 2;
    leftEyeHeader.RenderViewport.Size.h = height;
    leftEyeData.TexId = eyeFBOs[ovrEye_Left].tex;

    // Right eye the same, except for the x-position in the texture.
    ovrGLTexture& rightEyeTexture = eyeTextures[ovrEye_Right];
    rightEyeTexture = leftEyeTexture;
    rightEyeTexture.OGL.Header.RenderViewport.Pos.x = (width + 1) / 2;
    rightEyeTexture.OGL.TexId = eyeFBOs[ovrEye_Right].tex;

}

void AVROculus::setupTriangle()
{
    triangle = new AVRTest(0.0f);
    triangle->createProgram("shaders/test/vertex.shader", "shaders/test/fragment.shader");
}


glm::mat4 projectionMatrix(){

    // Model Matrix - from local coordinates to world coordinates
    glm::mat4 model;
    model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f)); 

    // View matrix - rotate to the camera plane.  No change here right now
    glm::mat4 view;
    // view = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);


    // Projection matrix - apply perspective
    glm::mat4 projection;
    projection = glm::perspective(glm::radians(30.0f), 0.5f, 0.1f, 2.0f);

    projection = projection*view*model;
    return projection;


}


void AVROculus::renderEye(ovrEyeType eye){
    bindFBO(eyeFBOs[eye]);

    if (eye==ovrEye_Right){
        glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
        std::cout << "Right eye" << std::endl;
    }
    else{
         glClearColor(0.0f, 0.0f, 1.0f, 1.0f);      
        std::cout << "Left eye" << std::endl;
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //construct the matrix for this eye
    //push the matrix for the given eye here to the shader.
    glm::mat4 dummy;
    triangle->draw(dummy);
    unbindFBO();

}


void AVROculus::runLoop(){

    ovrVector3f offset[2] = {eyeDescriptors[0].HmdToEyeViewOffset, eyeDescriptors[1].HmdToEyeViewOffset};


    ovrTexture eyeTextureGeneric[2];

    eyeTextureGeneric[ovrEye_Left] = eyeTextures[ovrEye_Left].Texture;
    std::cout << eyeTextures[ovrEye_Left].OGL.TexId << std::endl;

    eyeTextureGeneric[ovrEye_Right] = eyeTextures[ovrEye_Right].Texture;
    std::cout << eyeTextures[ovrEye_Right].OGL.TexId << std::endl;

    double start = ovr_GetTimeInSeconds();

    //Get rid of the Health and Safety warning asap.
    ovrHmd_DismissHSWDisplay(hmd);
    while (1){

    ovrHmd_BeginFrame(hmd, 0);


    double now = ovr_GetTimeInSeconds();
    ovrTrackingState tracking;
    ovrHmd_GetEyePoses(hmd, 0, offset, eyePoses, &tracking);


    if (tracking.StatusFlags & 
        (ovrStatus_OrientationTracked | ovrStatus_PositionTracked )){

    }

    renderEye(ovrEye_Left);
    renderEye(ovrEye_Right);

    ovrHmd_EndFrame(hmd, eyePoses, eyeTextureGeneric);


    if (now-start>20) break;
    }
}


int main(int argc, char * argv[]){

    AVROculus oculus;
    int nside = 128;
    float radius = 0.9;
    // AVRHealpix * hmap = new AVRHealpix(nside, radius);
    // hmap->load("map.fits");
    // hmap->createProgram("shaders/healpix/vertex.shader", "shaders/healpix/fragment.shader");
    // oculus.object = hmap;
    // // oculus.width = 200;
    // oculus.height = 200;
    // oculus.configureGLFW();
    oculus.setup();
    oculus.runLoop();

}