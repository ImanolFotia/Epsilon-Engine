#pragma once
#include <pch.hpp>

#include "ShadowBase.hpp"

namespace Epsilon
{
    namespace Renderer
    {
        class PointShadow : public ShadowBase
        {
        public:
            PointShadow(glm::vec3 p, unsigned short r = 1024, unsigned short n = 0.1, unsigned short f = 100.0);

            void Setup();

            void Destroy();

            void Begin(int index) override;

            void End() override;

            void updateMatrices(glm::vec3);

            GLuint getTexture() { return oFrameBuffer->getRenderTargetHandler(0); } //mShadowTexture; }

        private:
            OpenGL::FrameBuffer<int>* oFrameBuffer;
        protected:
        };
    } // namespace Renderer
} // namespace Epsilon