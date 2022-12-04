#pragma once

#include "types.hpp"
#include "object_pool.hpp"
#include "draw_command.hpp"
#include <framework/window.hpp>
#include "drawables/vertex.hpp"

#include <string>
#include <vector>
#include <list>

namespace engine
{

    const uint32_t MAX_VERTICES_PER_BUFFER = 10000;
    const uint32_t MAX_INDICES_PER_BUFFER = 10000;


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


    struct ObjectData {
        /*size_t num_vertices;
        size_t num_indices;

        size_t vert_offset;
        size_t index_offset;

        size_t vertex_bucket;
        size_t index_bucket;

        bool group = false;*/

        Ref<Mesh> mesh;
        Ref<Material> material;
        glm::mat4 modelMatrix;
        glm::vec3 position;
        glm::vec3 scale;
        glm::quat rotation;

        MeshPushConstant push_constant;
    };

    struct TextureData {
        size_t id;
        size_t size;
        size_t offset;
        size_t texture_bucket;
    };

    struct RenderObject {
        std::list<ObjectData>::iterator objectId;
        uint32_t materialId;
        ShaderData uniformData;
    };

    /**
     * @brief Renderer abstract class, do not instantiate directly
     *
     */
    class Renderer
    {
        public:

        using ObjectsData = std::list<ObjectData>;
        using ObjectDataId = std::list<ObjectData>::iterator;

        using TexturesData = std::list<TextureData>;
        using TexturesDataId = std::list<TextureData>::iterator;

        using IndexType = uint32_t;

    public:
        virtual void Init(const char*, framework::Window&) = 0;

        virtual ObjectDataId RegisterMesh(const std::vector<Vertex>&, std::vector<IndexType>& indices, bool) = 0;

        virtual TexturesDataId RegisterTexture(unsigned char*, TextureInfo) = 0;

        virtual Material CreateMaterial(Ref<Material>) = 0;

        virtual void Push(ObjectData) = 0;

        virtual void PushCameraData(const ShaderData& camData) = 0;
        
        virtual void Begin(Ref<RenderPass>) = 0;

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