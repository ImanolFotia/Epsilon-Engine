#pragma once
#include <PostProcess/Effect.h>

namespace Epsilon
{
    namespace PostProcess
    {

        template <typename T>
        class MotionBlur : public Effect
        {

        public:
            ~MotionBlur() {}

            MotionBlur(int w, int h, std::shared_ptr<Shader> s) : Effect(w, h, s)
            {
                m_pFrameBuffer = (std::shared_ptr<OpenGL::FrameBuffer<std::string>>)new OpenGL::FrameBuffer<std::string>(w, h, false);
                m_pFrameBuffer->addRenderTarget("MotionBlurBuffer", GL_RGB16F, GL_RGB, GL_LINEAR, GL_LINEAR, false);
                m_pFrameBuffer->FinishFrameBuffer();
            }

            GLuint Apply(GLuint tex1, GLuint tex2, double frametime)
            {
                m_pFrameBuffer->bindFramebuffer();
                m_pFrameBuffer->clearBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                m_pShader->Use();
                m_pFrameBuffer->setViewport();

                glActiveTexture(GL_TEXTURE0);
                glUniform1i(glGetUniformLocation(m_pShader->getProgramID(), "gFinalImage"), 0);
                glBindTexture(GL_TEXTURE_2D, tex1);

                glActiveTexture(GL_TEXTURE1);
                glUniform1i(glGetUniformLocation(m_pShader->getProgramID(), "gExtraComponents"), 1);
                glBindTexture(GL_TEXTURE_2D, tex2);

                int currentFPS = (1.0 / frametime);
                int targetFPS = (1.0 / 0.016f);
                int velocityScale = currentFPS / targetFPS;
                glUniform1f(glGetUniformLocation(m_pShader->getProgramID(), "uVelocityScale"), velocityScale * m_Intesity);

                m_pFullScreenQuad->Render();

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, 0);

                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, 1);

                m_pFrameBuffer->unbindFramebuffer();
                m_pShader->Free();

                return 0;
            }

            virtual void Update() {}

        private:
            bool m_Activated;
            std::shared_ptr<OpenGL::FrameBuffer<T>> m_pFrameBuffer;
            float m_Intesity;
        };
    } // namespace PostProcess
} // namespace Epsilon
