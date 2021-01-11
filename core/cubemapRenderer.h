#pragma once

#include <pch.hpp>
#include <Core.hpp>
#include <Driver/API/OpenGL/FrameBuffer.h>
#include <Driver/API/OpenGL/HelperFunctions/FullScreenQuad.h>
#include <Driver/API/OpenGL/FrameBuffer.h>
#include <Shader.h>
#include <glm/glm.hpp>
#include <camera.h>

namespace Epsilon {
class CubemapRenderer
{
public:
    CubemapRenderer();

    virtual ~CubemapRenderer(){}

    void Begin(int, GLuint, GLuint);

    void End(int);

    std::vector<GLuint> getRenderTargets();

public:
    std::shared_ptr<Shader> mMainShader;

private:

    void CopyTexture(int);

    void setCamera();

    float mNEAR, mFAR;
    int mWIDTH, mHEIGHT;
    glm::vec3 mPosition;
    std::shared_ptr<OpenGL::FrameBuffer<int> > mFrameBuffer;
    std::shared_ptr<OpenGL::FrameBuffer<int> > mCopyTextureFrameBuffer;
    std::shared_ptr<OpenGLHelpers::FullScreenQuad> mFullScreenQuad;
    std::shared_ptr<Shader> mPassThroughShader;
    std::vector<GLuint> mRenderTargets;
    std::shared_ptr<Camera> mRendererCamera;

};
}
