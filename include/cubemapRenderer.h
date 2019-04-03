#pragma once

#include <GL/glew.h>
#include <vector>
#include <memory>
#include <OpenGL/FrameBuffer.h>
#include <OpenGL/HelperFunctions/FullScreenQuad.h>
#include <OpenGL/FrameBuffer.h>
#include <Shader.h>
#include <glm/glm.hpp>
#include <camera.h>

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
    std::shared_ptr<FrameBuffer<int> > mFrameBuffer;
    std::shared_ptr<FrameBuffer<int> > mCopyTextureFrameBuffer;
    std::shared_ptr<OpenGLHelpers::FullScreenQuad> mFullScreenQuad;
    std::shared_ptr<Shader> mPassThroughShader;
    std::vector<GLuint> mRenderTargets;
    std::shared_ptr<Camera> mRendererCamera;

};

