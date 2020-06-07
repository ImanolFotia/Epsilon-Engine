#pragma once
#include <Driver/API/OpenGL/FrameBuffer.h>
#include <Driver/API/OpenGL/HelperFunctions/FullScreenQuad.h>
#include <Shader.h>
#include <vector>
#include <memory>
#include <glm/glm.hpp>

namespace Epsilon
{
    namespace PostProcess
    {
        class Effect
        {

        public:
            Effect(int, int, std::shared_ptr<Shader>) {}

            virtual ~Effect() {}

            virtual void Apply(GLuint texture) = 0;

            virtual void Update() = 0;

        protected:
            std::shared_ptr<OpenGLHelpers::FullScreenQuad> m_pFullScreenQuad;
            std::shared_ptr<Shader> m_pShader;
            int m_Width;
            int m_Height;
        };
    } // namespace PostProcess
} // namespace Epsilon
