#pragma once

#include "types.hpp"
#include "../object_pool.hpp"
#include "core/framework/window.hpp"
#include <core/common/common.hpp>

#include "vulkan/imgui/imgui_setup.hpp"
#include "draw_command.hpp"
#include <string>
#include <vector>
#include <list>

namespace engine
{

    const uint32_t MAX_VERTICES_PER_BUFFER = 1000000;
    const uint32_t MAX_INDICES_PER_BUFFER =  1000000;

    const uint32_t MAX_COMMAND_QUEUE_SIZE = 2000;

    struct ShaderModuleInfo
    {
        uint16_t stage_flags;
        std::string vertex_path;
        std::string fragment_path;
        std::string compute_path;
    };

    struct RenderPass
    {
        ShaderModuleInfo shaderModuleInfo;

    protected:
        RenderPass() = default;
    };


    struct ObjectData
    {
        Ref<Mesh> mesh;
        Ref<BindGroup> material;
        Ref<PushConstant> pushConstant;
        ObjectDataConstant objectConstant;
        uint32_t layout_index = 0;
        uint32_t uniformIndex = 0;
    };

    struct TextureData
    {
        size_t id{};
        size_t size{};
        size_t offset{};
        size_t texture_bucket{};
    };

    struct RenderObject
    {
        std::list<ObjectData>::iterator objectId;
        uint32_t materialId{};
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
        virtual void Init(const char *, framework::Window &) = 0;

        virtual ObjectDataId RegisterMesh(const std::vector<common::Vertex> &, std::vector<IndexType> &indices, bool) = 0;

        virtual TexturesDataId RegisterTexture(TextureCreationInfo) = 0;

        virtual Material CreateMaterial(Ref<Material>) = 0;

        virtual void Push(ObjectData) = 0;

        virtual void Submit() = 0;

        virtual void EndFrame() = 0;

        virtual void BeginFrame() = 0;

        virtual void Begin() = 0;

        virtual void End(glm::vec3 &v) = 0;

        virtual void Flush(Ref<RenderPass>, engine::DrawType) = 0;

        virtual void UpdateRenderPassUniforms(Ref<RenderPass> renderPassRef, BindingIndex index, const void *data) = 0;

        virtual void Cleanup() = 0;


    public:
        /**
         * @brief Construct a new Renderer object
         *
         * @param type the underlying api this renderer will be using
         */
        Renderer(renderer_type type) : m_pType{type} {};
        Renderer() = default;

        Renderer(Renderer &&) = delete;
        Renderer(const Renderer &) = delete;

        virtual ImGuiRenderer& getDebugRenderer() = 0;

        virtual ~Renderer() {}

        uint32_t numPushedCommands() {
            return currentCommandsInQueue;
        }

        uint32_t CurrentFrameInFlight() {
            return m_pCurrentFrame;
        }

        virtual void SetViewport(const Viewport&) = 0;

        virtual void SetScissor(const Scissor&) = 0;

        virtual void SetRenderPass(Ref<RenderPass>) = 0;

    protected:

        uint32_t m_pCurrentFrame = 0;
        renderer_type m_pType{};
        uint32_t currentCommandsInQueue = 0;
        std::vector<DrawCommand> m_pCurrentCommandQueue;
    };
}