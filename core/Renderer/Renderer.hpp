#pragma once

#include <Renderer/Drawable.hpp>
#include <Renderer/Decal.hpp>
#include <Renderer/Material.hpp>
#include <Renderer/MaterialPBR.hpp>
#include <Renderer/Model.h>
#include <Renderer/Patch.h>
#include <Renderer/Terrain.h>

#include <Renderer/Texture1D.hpp>
#include <Renderer/Texture2D.hpp>

#include "RenderQueue.hpp"

namespace Epsilon {
    class Renderer {
        public:
            Renderer() = default;

            void PushCommand(RenderCommand_ptr comm) {
                mQueue->Push(comm);
            }

            void Flush() {
                
            }

            void RenderOpaque() {}

            void RenderTransparent() {}

        private:

            RenderQueue_ptr mQueue;
    };
}