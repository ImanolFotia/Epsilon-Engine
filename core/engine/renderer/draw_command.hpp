#pragma once

#include "resource_manager.hpp"

namespace engine
{


    class DrawCommand
    {
    public:
        DrawCommand() = default;
        MeshResource meshResource;

        uint32_t layoutIndex = 0;

        Ref<engine::BindGroup> material;
        void *objectData = nullptr;
        size_t object_data_size = 0;
        uint32_t uniformIndex = 0;
        ObjectDataConstant pushConstantData;
    };

    struct IndirectCommand
    {
        uint32_t vertexCount{};
        uint32_t instanceCount{};
        uint32_t firstVertex{};
        uint32_t firstInstance{};
    };

    struct IndexedIndirectCommand
    {
        uint32_t indexCount{};
        uint32_t instanceCount{};
        uint32_t firstIndex{};
        int32_t vertexOffset{};
        uint32_t firstInstance{};
    };

    struct IndirectBatch
    {
        MeshResource meshResource{};
        Ref<BindGroup> material;
        uint32_t uniformIndex = 0;
        uint32_t layoutIndex = 0;
        uint32_t count = 0;
        uint32_t first = 0;
    };
}