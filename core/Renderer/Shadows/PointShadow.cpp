#include "PointShadow.hpp"

namespace Epsilon
{
    namespace Renderer
    {
        PointShadow::PointShadow(glm::vec3 p, unsigned short r, unsigned short n, unsigned short f)
        {
            mPosition = p;
            mResolution = r;
            mNear = n;
            mFar = f;

        }

        void PointShadow::Setup()
        {
            oFrameBuffer = new OpenGL::FrameBuffer<int>(mResolution, mResolution, false);
            oFrameBuffer->addRenderTarget(0, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_LINEAR, GL_LINEAR, true, GL_TEXTURE_CUBE_MAP);
            oFrameBuffer->MakeInmutable(0); 
            oFrameBuffer->FinishFrameBuffer(true);

            mProjection = glm::perspective(glm::radians(90.0), 1.0, (double)mNear, (double)mFar);

            mViews.push_back(mProjection * glm::lookAt(mPosition, mPosition + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
            mViews.push_back(mProjection * glm::lookAt(mPosition, mPosition + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
            mViews.push_back(mProjection * glm::lookAt(mPosition, mPosition + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
            mViews.push_back(mProjection * glm::lookAt(mPosition, mPosition + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
            mViews.push_back(mProjection * glm::lookAt(mPosition, mPosition + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
            mViews.push_back(mProjection * glm::lookAt(mPosition, mPosition + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        }

        void PointShadow::Destroy() {
            if(oFrameBuffer) oFrameBuffer->Destroy();
        }

        void PointShadow::Begin(int index)
        {
            int mip_res = mResolution * std::pow(0.5, index);
            
            auto shader = ResourceManager::Get().getShader("PointShadow");
            shader->Use();
            glEnable(GL_DEPTH_TEST);

            oFrameBuffer->bindFramebuffer();
            oFrameBuffer->setViewport(mip_res, mip_res);
            oFrameBuffer->AttachRenderBuffer(0);
            oFrameBuffer->clearBuffer(GL_DEPTH_BUFFER_BIT);
 
            shader->PushUniform("Far", (float)mFar);
            shader->PushUniform("LightPosition", mPosition);

            glUniformMatrix4fv(glGetUniformLocation(shader->getProgramID(), "ViewMatrices"), 6, GL_FALSE, &mViews[0][0][0]);
        }

        void PointShadow::End()
        {
            oFrameBuffer->unbindFramebuffer();
            auto shader = ResourceManager::Get().getShader("PointShadow");
            shader->Free();
        }
    } // namespace Renderer
} // namespace Epsilon