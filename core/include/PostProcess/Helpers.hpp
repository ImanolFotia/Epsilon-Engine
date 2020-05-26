#pragma once

#include <GL/glew.h>

#include <memory>

#include <OpenGL/FrameBuffer.h>
#include <OpenGL/HelperFunctions/FullScreenQuad.h>
#include <Shader.h>

namespace PostProcess
{
    namespace Helpers
    {
        
        static std::shared_ptr<FrameBuffer<int>> CopyTextureFBO;
        static std::shared_ptr<Shader> CopyShader;
        static std::shared_ptr<OpenGLHelpers::FullScreenQuad> FSQ;
        
        static GLuint CopyTexture(int width, int height, GLuint origin)
        {
            CopyTextureFBO->bindFramebuffer();
            CopyTextureFBO->setViewport(width, height);
            CopyTextureFBO->clearBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            CopyShader->Use();

            glActiveTexture(GL_TEXTURE0);
            glUniform1i(glGetUniformLocation(CopyShader->getProgramID(), "texture0"), 0);
            glBindTexture(GL_TEXTURE_2D, origin);
            glGenerateMipmap(GL_TEXTURE_2D);

            CopyTextureFBO->clearBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            FSQ->Render();

            CopyShader->Free();

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);

            CopyTextureFBO->unbindFramebuffer();
            glViewport(0, 0, width, height);

            return CopyTextureFBO->getRenderTargetHandler(0);
        }

        static void Init (int width, int height){
            CopyTextureFBO = std::make_shared<FrameBuffer<int>>(width, height, false);
            CopyTextureFBO->addRenderTarget(0, GL_RGB16F, GL_RGB, GL_LINEAR, GL_LINEAR, false);
            CopyTextureFBO->FinishFrameBuffer();

            FSQ = std::make_shared<OpenGLHelpers::FullScreenQuad>();
            
            CopyShader = std::make_shared<Shader>("shaders/PassThrough.vglsl", "shaders/PassThrough.fglsl");
        }

    } // namespace Helpers
} // namespace PostProcess