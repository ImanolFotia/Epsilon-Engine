#pragma once

#include <pch.hpp>
#include <PostProcess/Effect.h>
#include <Helpers.hpp>

namespace Epsilon
{
    namespace PostProcess
    {

        template <typename T>
        class SSAO : public Effect
        {
        public:
            SSAO(int width, int height, std::shared_ptr<Shader> s) : Effect(width, height)
            {
                m_Width = width;
                m_Height = height;
                m_pShader = (std::shared_ptr<Shader>)new Shader("shaders/SSAO.vglsl", "shaders/SSAO.fglsl");
                blurShader = (std::shared_ptr<Shader>)new Shader("shaders/blurSSAO.vglsl", "shaders/blurSSAO.fglsl");
            }

            virtual void Update() {}

            /**----------------------------------------*/
            /**           SSAO Specific Code           */
            /**----------------------------------------*/

            GLuint Apply(GLuint gDepth, GLuint gNormal, glm::mat4 projection, glm::mat4 view)
            {
                //glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
                m_pFrameBuffer->bindFramebuffer();
                m_pFrameBuffer->clearBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                m_pFrameBuffer->setViewport();

                m_pShader->Use();

                glActiveTexture(GL_TEXTURE0);
                glUniform1i(glGetUniformLocation(m_pShader->getProgramID(), "gDepth"), 0);
                glBindTexture(GL_TEXTURE_2D, gDepth);
                glGenerateMipmap(GL_TEXTURE_2D);

                glActiveTexture(GL_TEXTURE1);
                glUniform1i(glGetUniformLocation(m_pShader->getProgramID(), "gNormal"), 1);
                glBindTexture(GL_TEXTURE_2D, gNormal);

                glActiveTexture(GL_TEXTURE2);
                glUniform1i(glGetUniformLocation(m_pShader->getProgramID(), "texNoise"), 2);
                glBindTexture(GL_TEXTURE_2D, this->noiseTexture);

                glUniform3fv(glGetUniformLocation(m_pShader->getProgramID(), "samples"), 32, &ssaoKernel[0][0]);

                glUniformMatrix4fv(glGetUniformLocation(m_pShader->getProgramID(), "projection"), 1, GL_FALSE, &projection[0][0]);
                glUniformMatrix4fv(glGetUniformLocation(m_pShader->getProgramID(), "invprojection"), 1, GL_FALSE, &glm::inverse(view)[0][0]);
                glUniformMatrix4fv(glGetUniformLocation(m_pShader->getProgramID(), "view"), 1, GL_FALSE, &view[0][0]);

                m_pFullScreenQuad->Render();
                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                // 3. Blur SSAO texture to remove noise
                m_pFrameBufferBlur->bindFramebuffer();
                m_pFrameBufferBlur->clearBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                m_pFrameBufferBlur->setViewport();

                blurShader->Use();

                glActiveTexture(GL_TEXTURE0);
                m_pFrameBuffer->getRenderTargetHandler("SSAOBuffer");

                m_pFullScreenQuad->Render();

                m_pFrameBufferBlur->unbindFramebuffer();
                glBindTexture(GL_TEXTURE_2D, 0);

                return 0;
            }

            void PrepareSSAOTexture()
            {
                m_pFrameBuffer = (std::shared_ptr<FrameBuffer<std::string>>)new FrameBuffer<std::string>(m_Width, m_Height, false);
                m_pFrameBuffer->addRenderTarget("SSAOBuffer", GL_RED, GL_RGB, GL_LINEAR, GL_LINEAR, true);
                m_pFrameBuffer->FinishFrameBuffer();
            }

            void PrepareSSAOBlurTexture()
            {
                m_pFrameBufferBlur = (std::shared_ptr<FrameBuffer<std::string>>)new FrameBuffer<std::string>(m_Width, m_Height, false);
                m_pFrameBufferBlur->addRenderTarget("ssaoBlur", GL_RED, GL_RGB, GL_LINEAR, GL_LINEAR, true);
                m_pFrameBufferBlur->FinishFrameBuffer();
            }

            void PrepareRendomSamplingTexture()
            {
                // Sample kernel
                std::uniform_real_distribution<GLfloat> randomFloatsClamped(0.2, 1.0); // generates random floats between 0.0 and 1.0
                std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);        // generates random floats between 0.0 and 1.0
                std::default_random_engine generator;
                for (GLuint i = 0; i < 32; ++i)
                {
                    glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloatsClamped(generator));
                    sample = glm::normalize(sample);
                    sample *= randomFloats(generator);
                    GLfloat scale = GLfloat(i) / 32.0;

                    // Scale samples s.t. they're more aligned to center of kernel
                    scale = Helpers::lerp(0.1f, 1.0f, scale * scale);
                    sample *= scale;
                    ssaoKernel.push_back(sample);
                }

                // Noise texture
                for (GLuint i = 0; i < 64; i++)
                {
                    glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
                    ssaoNoise.push_back(noise);
                }

                glGenTextures(1, &noiseTexture);
                glBindTexture(GL_TEXTURE_2D, noiseTexture);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            }

        private:
            GLuint noiseTexture;
            std::vector<glm::vec3> ssaoKernel;
            std::shared_ptr<Shader> blurShader;
            std::shared_ptr<FrameBuffer<std::string>> m_pFrameBufferBlur;
            std::vector<glm::vec3> ssaoNoise;
            std::shared_ptr<FrameBuffer<T>> m_pFrameBuffer;

        protected:
        };
    } // namespace PostProcess
} // namespace Epsilon