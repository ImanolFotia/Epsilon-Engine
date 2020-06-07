#pragma once

#include <PostProcess/Effect.h>

namespace Epsilon
{
    namespace PostProcess
    {
        template <typename T>
        class ScreenSpaceReflections : public Effect
        {
        public:
            ScreenSpaceReflections(int w, int h, std::shared_ptr<Shader> s) : Effect(w, h)
            {
                m_Width = w;
                m_Height = h;
                m_pShader = (std::shared_ptr<Shader>)new Shader("shaders/SSR.vglsl", "shaders/SSR.fglsl");
            }

            ~ScreenSpaceReflections() {}

            void Apply(GLuint gFinalImage, GLuint gPosition, GLuint gNormal, GLuint gExpensiveNormal, GLuint gExtraComponents,
                       glm::mat4 proj, glm::mat4 view)
            {
                m_pShader->Use();
                glViewport(0, 0, m_Width, m_Height);
                glActiveTexture(GL_TEXTURE0);
                glUniform1i(glGetUniformLocation(m_pShader->getProgramID(), "gFinalImage"), 0);
                glBindTexture(GL_TEXTURE_2D, gFinalImage);
                //glGenerateMipmap(GL_TEXTURE_2D);

                glActiveTexture(GL_TEXTURE1);
                glUniform1i(glGetUniformLocation(m_pShader->getProgramID(), "gPosition"), 1);
                glBindTexture(GL_TEXTURE_2D, gPosition);
                glGenerateMipmap(GL_TEXTURE_2D);

                glActiveTexture(GL_TEXTURE2);
                glUniform1i(glGetUniformLocation(m_pShader->getProgramID(), "gNormal"), 2);
                glBindTexture(GL_TEXTURE_2D, gExpensiveNormal);

                glActiveTexture(GL_TEXTURE3);
                glUniform1i(glGetUniformLocation(m_pShader->getProgramID(), "gExtraComponents"), 3);
                glBindTexture(GL_TEXTURE_2D, gExtraComponents);

                glUniformMatrix4fv(glGetUniformLocation(m_pShader->getProgramID(), "projection"), 1, GL_FALSE, &proj[0][0]);
                glm::mat4 invproj = glm::inverse(proj);
                glUniformMatrix4fv(glGetUniformLocation(m_pShader->getProgramID(), "invprojection"), 1, GL_FALSE, &invproj[0][0]);
                glm::mat4 invView = glm::inverse(view);
                glUniformMatrix4fv(glGetUniformLocation(m_pShader->getProgramID(), "invView"), 1, GL_FALSE, &invView[0][0]);
                glUniformMatrix4fv(glGetUniformLocation(m_pShader->getProgramID(), "view"), 1, GL_FALSE, &view[0][0]);
                glUniform2f(glGetUniformLocation(m_pShader->getProgramID(), "Resolution"), this->m_Width, this->m_Height);

                m_pFullScreenQuad->Render();

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, 0);
            }

            void PrepareSSRTexture()
            {
                m_pFrameBuffer = (std::shared_ptr<FrameBuffer<std::string>>)new FrameBuffer<std::string>(m_Width, m_Height, false);
                m_pFrameBuffer->addRenderTarget("SSRBuffer", GL_RED, GL_RGB, GL_LINEAR, GL_LINEAR, true);
                m_pFrameBuffer->FinishFrameBuffer();
            }

            virtual void Update() {}

            /**----------------------------------------*/
            /**           SSR Specific Code           */
            /**----------------------------------------*/

            std::shared_ptr<FrameBuffer<T>> m_pFrameBuffer;
        };
    } // namespace PostProcess
} // namespace Epsilon