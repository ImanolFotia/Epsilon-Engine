#pragma once

#include <PostProcess/Effect.h>
#include <PostProcess/Helpers.hpp>

namespace Epsilon
{
    namespace PostProcess
    {
        template <typename T>
        class BlurMips : public Effect
        {
            using FrameBuffer_ptr = std::shared_ptr<OpenGL::FrameBuffer<T>>;

        public:
            BlurMips(int w, int h, std::shared_ptr<Shader> shader) : m_Width(w), m_Height(h), m_pShader(shader)
            {
                m_PinPongFrameBuffer[0] = std::make_shared<FrameBuffer_ptr>(w, h, true);
                m_PinPongFrameBuffer[1] = std::make_shared<FrameBuffer_ptr>(w, h, true);

                m_PinPongFrameBuffer[0]->addRenderTarget(0, GL_RGB16F, GL_RGB, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR, true);
                m_PinPongFrameBuffer[1]->addRenderTarget(0, GL_RGB16F, GL_RGB, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR, true);

                m_PinPongFrameBuffer[0]->FinishFrameBuffer();
                m_PinPongFrameBuffer[1]->FinishFrameBuffer();
            }

            ~BlurMips() {}

            void Apply(GLuint texture)
            {
                m_pShader->Use();
                unsigned int maxMipLevels = 5;
                int amount = 5; //amount to blur
                for (int mip = 0; mip < maxMipLevels; mip++)
                {
                    unsigned int mipWidth = m_PinPongFrameBuffer[0]->WIDTH * std::pow(0.5, mip);
                    unsigned int mipHeight = m_PinPongFrameBuffer[0]->HEIGHT * std::pow(0.5, mip);

                    m_pShader->PushUniform("cheap", 0);
                    bool horizontal = true;
                    bool first_iteration = true;
                    for (int i = 0; i < amount; i++)
                    {
                        m_PinPongFrameBuffer[horizontal]->bindFramebuffer();
                        m_PinPongFrameBuffer[horizontal]->setViewport(mipWidth, mipHeight);
                        m_pShader->PushUniform("horizontal", horizontal);

                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_2D, first_iteration ? texture : m_PinPongFrameBuffer[!horizontal]->getRenderTargetHandler(0));
                        m_pShader->PushUniform("image", 0);

                        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_PinPongFrameBuffer[horizontal]->getRenderTargetHandler(0), mip);

                        m_PinPongFrameBuffer[horizontal]->clearBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                        m_pFullScreenQuad->Render();

                        mLastUsedFBO = horizontal;
                        horizontal = !horizontal;

                        first_iteration = false; //negate this no matter what
                    }
                }
                m_pShader->Free();
                m_PinPongFrameBuffer[0]->unbindFramebuffer();
                m_PinPongFrameBuffer[1]->unbindFramebuffer();
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glViewport(0, 0, m_Width, m_Height);
            }

            void Update() {}

            FrameBuffer_ptr getFramebuffer()
            {
                return m_PinPongFrameBuffer[mLastUsedFBO];
            }

            GLuint getResult()
            {
                return m_PinPongFrameBuffer[mLastUsedFBO]->getRenderTargetHandler(0);
            }

        private:
            FrameBuffer_ptr m_PinPongFrameBuffer[2];
            std::shared_ptr<Shader> m_pShader;
            int m_Width;
            int m_Height;

            int mLastUsedFBO;
        };
    } // namespace PostProcess
} // namespace Epsilon