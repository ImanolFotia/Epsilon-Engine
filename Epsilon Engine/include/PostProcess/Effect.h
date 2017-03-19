#pragma once
#include <OpenGL/FrameBuffer.h>
#include <OpenGL/HelperFunctions/FullScreenQuad.h>
#include <Shader.h>

#include <memory>
namespace PostProcess {
    class Effect
    {
    public:
        Effect()
        {
            m_pFullScreenQuad = (std::shared_ptr<OpenGLHelpers::FullScreenQuad>) new OpenGLHelpers::FullScreenQuad();
        }

        virtual ~Effect() {}

        virtual void ApplyEffect(){}

        virtual void Update(){}

    protected:

        std::shared_ptr<FrameBuffer> m_pFrameBuffer;
        std::shared_ptr<OpenGLHelpers::FullScreenQuad> m_pFullScreenQuad;
        std::shared_ptr<Shader> m_pShader;
    };
}
