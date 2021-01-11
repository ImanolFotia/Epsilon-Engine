#pragma once

#include <Shader.h>
#include <Types.h>
#include <Core.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ResourceManager.h>
#include <Driver/API/OpenGL/FrameBuffer.h>

namespace Epsilon
{
    namespace Renderer
    {
        class ShadowBase
        {
        public:
            ShadowBase() = default;

            virtual ~ShadowBase() {}

            virtual void Begin(int) = 0;

            virtual void End() = 0;

            unsigned short Far() { return mFar; }

        protected:
            GLuint mShadowTexture = 0;
            GLuint mShadowFBO = 0;
            GLuint mShadowRBO = 0;
            glm::vec3 mPosition;
            glm::mat4 mProjection;

            std::vector<glm::mat4> mViews;

            unsigned short mResolution = 512;
            unsigned short mNear, mFar;
        };
    } // namespace Renderer
} // namespace Epsilon