#include <cubemapRenderer.h>
#include <Texture.h>

namespace Epsilon {
CubemapRenderer::CubemapRenderer() {
    this->mFrameBuffer = (std::shared_ptr<OpenGL::FrameBuffer<int> >)new OpenGL::FrameBuffer<int>(1024, 1024, true);
    
    this->mFrameBuffer->addRenderTarget(0, GL_RGB16F, GL_RGB, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR, true);
    this->mFrameBuffer->addDepthAttachment();
    this->mFrameBuffer->FinishFrameBuffer();

    this->mMainShader = (std::shared_ptr<Shader>) new Shader("shaders/vertex.glsl", "shaders/fragment.frag");
    this->mPassThroughShader = (std::shared_ptr<Shader>) new Shader("shaders/PassThrough.vglsl", "shaders/PassThrough.fglsl");

    this->mRenderTargets.reserve(6);

    this->mFullScreenQuad = (std::shared_ptr<OpenGLHelpers::FullScreenQuad>) new OpenGLHelpers::FullScreenQuad();
}
/**
    Function receives the current faces' index
*/
void CubemapRenderer::Begin(int index, GLuint texture, GLuint FBO) {
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glViewport(0,0,1024,1024);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X+index, texture, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 1.0, 0.0, 1.0);
}

void CubemapRenderer::End(int index) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

   // this->CopyTexture(index);
}

void CubemapRenderer::CopyTexture(int index) {

    eTexture tex("6544-normal.jpg");
    std::cout << "Bind copy texture framebuffer" << std::endl;
    this->mCopyTextureFrameBuffer->bindFramebuffer();

    glViewport(0, 0, 1024, 1024);
    glClear(GL_COLOR_BUFFER_BIT);

    std::cout << "Using Shader" << std::endl;
    mPassThroughShader->Use();

    glActiveTexture(GL_TEXTURE0);
    mPassThroughShader->PushUniform("texture0", 0);
    glBindTexture(GL_TEXTURE_2D, tex.getTextureID());

    std::cout << "Rendering fullscreen quad" << std::endl;
    mFullScreenQuad->Render();

    mPassThroughShader->Free();

    this->mCopyTextureFrameBuffer->unbindFramebuffer();
    std::cout << "Copying texture" << std::endl;
    this->mRenderTargets.push_back(this->mCopyTextureFrameBuffer->getRenderTargetHandler(0));
    std::cout << "End copy texture function" << std::endl;
}

std::vector<GLuint> CubemapRenderer::getRenderTargets()
{
    return this->mRenderTargets;
}
}