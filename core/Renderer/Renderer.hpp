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

#include <Driver/API/VertexArrayObject.hpp>

#include "RenderQueue.hpp"

namespace Epsilon::Renderer {


    class Renderer {
        
        using data_type = Epsilon::API::DrawCommandDataBase_t;
        using gl_data_type = Epsilon::API::OpenGL::MultiDrawCommandDataIndirect_t;
        using pointer_data_type = std::shared_ptr<data_type>;

        public:
            Renderer() = default;

            void pushCommand(const RenderCommand& comm) {
                m_RenderQueue.push(comm);
            }

            void flush() {
                
            }

            void clear() {
                
            }

        private:

            std::vector<API::VertexArrayObject> m_VertexBufferObjects;
            std::vector<pointer_data_type> m_DrawCommands;
            std::queue<RenderCommand> m_RenderQueue;
    };
}