#pragma once
#include <OpenGL/FrameBuffer.h>
#include <OpenGL/HelperFunctions/FullScreenQuad.h>
#include <Shader.h>
#include <vector>
#include <memory>
#include <glm/glm.hpp>

namespace PostProcess {
    class Effect{

    public:

        Effect(int, int){}

        virtual ~Effect(){}

        virtual GLuint Apply() = 0;

        virtual void Update() = 0;

    protected:

        std::shared_ptr<FrameBuffer<std::string> > m_pFrameBuffer;
        std::shared_ptr<OpenGLHelpers::FullScreenQuad> m_pFullScreenQuad;
        std::shared_ptr<Shader> m_pShader;
        int m_Width;
        int m_Height;
    };
}
