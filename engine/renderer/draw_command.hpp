#pragma once

#include "resource_manager.hpp"

namespace engine
{
    class DrawCommand
    {
    public:
        DrawCommand() = default;

        uint32_t vertexOffset = 0;
        uint32_t indexOffset = 0;
        uint32_t numVertices = 0;
        uint32_t numIndices = 0;

        Ref<Buffer> vertexBuffer;
        Ref<Buffer> indexBuffer;
        Ref<UniformBindings> uniformBindings;
        Ref<engine::Material> material;
    };

}