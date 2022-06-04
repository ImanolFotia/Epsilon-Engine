#pragma once

#include "engine/types.hpp"

#include "framework/window.hpp"

#include <string>
#include <vector>
#include <list>

namespace engine
{

    const uint32_t MAX_VERTICES_PER_BUFFER = 10000;
    const uint32_t MAX_INDICES_PER_BUFFER = 10000;


    enum shader_module_stage {
        FRAGMENT = 0x1,
        VERTEX = 0x10,
        COMPUTE = 0x100
    };

    struct ShaderModuleInfo {
        uint16_t stage_flags;
        std::string vertex_path;
        std::string fragment_path;
        std::string compute_path;
    };

    struct RenderPass {
        ShaderModuleInfo shaderModuleInfo;

        protected:
            RenderPass() = default;
    };

    struct DrawCommandIndirect {

    };


    struct MeshPushConstant {
        alignas(16) glm::mat4 model;
    };

    struct ObjectData {
        uint16_t num_vertices;
        uint16_t num_indices;

        uint16_t vert_offset;
        uint16_t index_offset;

        uint16_t vertex_bucket;
        uint16_t index_bucket;

        bool group = false;

        MeshPushConstant push_constant;
    };

    struct RenderPassInfo {
        uint32_t numDescriptors;
        size_t size;
        std::vector<std::pair<VkFormat, size_t>> vertexLayout;
    };

    struct MaterialInfo {};

    /**
     * @brief Renderer abstract class, do not instantiate directly
     *
     */
    class Renderer
    {
        public:

        using ObjectsData = std::list<ObjectData>;
        using ObjectDataId = std::list<ObjectData>::iterator;

        using IndexType = uint32_t;

    public:
        virtual void Init(const char*, framework::Window&) = 0;

        virtual ObjectDataId Submit(const std::vector<Vertex>&, std::vector<IndexType>& indices, const MaterialInfo&, bool) = 0;

        virtual uint32_t addRenderpass(RenderPassInfo) = 0;

        virtual void Push(ObjectDataId) = 0;

        virtual void PushCameraData(const ShaderData& camData) = 0;
        
        virtual void Begin() = 0;

        virtual void End() = 0;

        virtual void Flush() = 0;

        virtual void Cleanup() = 0;

    public:
        /**
         * @brief Construct a new Renderer object
         *
         * @param type the underlying api this renderer will be using
         */
        Renderer(renderer_type type) : m_pType{type} {};
        Renderer() = default;

        Renderer(Renderer&&) = delete;
        Renderer(const Renderer&) = delete;

        virtual ~Renderer() {}

    protected:
        renderer_type m_pType;
    };
}