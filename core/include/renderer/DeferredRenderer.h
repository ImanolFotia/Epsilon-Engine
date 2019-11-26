#pragma once

#include <Renderer.h>
#include <OpenGL/FrameBuffer.h>
#include <Shader.h>

class DeferredRenderer : public Renderer
{
public:
    DeferredRenderer(int width, int height)
    {
        /**Setting up the Geometry buffer*/
        m_gBuffer = (std::shared_ptr<FrameBuffer>) new FrameBuffer(width, height, true);

        m_gBuffer->addRenderTarget("gAlbedoSpec", GL_RGBA8, GL_RGBA, GL_NEAREST, GL_NEAREST, true);
        m_gBuffer->addRenderTarget("gPosition", GL_RGB16F, GL_RGB, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR, true);
        m_gBuffer->addRenderTarget("gExpensiveNormal", GL_RGBA16F, GL_RGBA, GL_NEAREST, GL_NEAREST, true);
        m_gBuffer->addRenderTarget("gDepth", GL_R32F, GL_RED, GL_NEAREST, GL_NEAREST_MIPMAP_NEAREST, true);
        m_gBuffer->addRenderTarget("gExtraComponents", GL_RGBA16F, GL_RGBA, GL_NEAREST, GL_NEAREST, true);
        m_gBuffer->addRenderTarget("gLightAccumulation", GL_RGB16F, GL_RGB, GL_NEAREST, GL_NEAREST, true);

        m_gBuffer->addDepthAttachment();

        m_gBuffer->FinishFrameBuffer();

        /** Setting up the composite buffer*/
        m_CompositeBuffer = (std::shared_ptr<FrameBuffer>) new FrameBuffer(width, height, true);

        m_CompositeBuffer->addRenderTarget("colorBuffer", GL_RGB16F, GL_RGB, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR, true);
        m_CompositeBuffer->addRenderTarget("brightColorBuffer", GL_RGB16F, GL_RGB, GL_LINEAR, GL_LINEAR, true);
        m_CompositeBuffer->FinishFrameBuffer();

    }

    ~DeferredRenderer() {}

    void beginRender()
    {
        glDisable(GL_BLEND);
        m_gBuffer->setViewport();
        m_gBuffer->bindFramebuffer();
        m_gBuffer->clearBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    }

    void endRender()
    {
        m_gBuffer->unbindFramebuffer();
    }

    void getTexture() {}

private:
    std::shared_ptr<FrameBuffer> m_gBuffer;
    std::shared_ptr<FrameBuffer> m_CompositeBuffer;
    std::shared_ptr<Shader> m_shader;
    std::shared_ptr<eCamera> m_camera;
protected:

};

