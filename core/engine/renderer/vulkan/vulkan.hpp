#pragma once
// #undef VMA_DEBUG_LOG
// #undef VMA_DEBUG_LOG_FORMAT
#include "core/engine/renderer/renderer.hpp"
#include "core/engine/renderer/frame.hpp"
#include "core/engine/renderer/types.hpp"
#include "core/engine/renderer/drawables/vertex.hpp"
#include "../draw_command.hpp"

#include "apis/vk/vk.hpp"
#include "imgui/imgui_setup.hpp"

#include <core/framework/singleton.hpp>

#include <vector>
#include <list>
/*
#define VMA_DEBUG_LOG_FORMAT(format, ...)

#define VMA_DEBUG_LOG_FORMAT(format, ...) \
    do                                    \
    {                                     \
        printf((format), __VA_ARGS__);    \
        printf("\n");                     \
    } while (false)
*/

#include "vk_mem_alloc.h"

namespace engine
{
    class VulkanResourceManager;
    class VulkanRenderer : public Renderer
    {
        using RenderPasses = std::unordered_map<uint32_t, vk::VulkanRenderPass>;
        using VertexBuffers = std::vector<vk::VulkanBuffer>;
        using IndexBuffers = std::vector<vk::VulkanBuffer>;
        using TextureBuffers = std::vector<vk::VulkanTextureBuffer>;
        using Textures = std::vector<vk::VulkanTexture>;
        using Materials = std::vector<vk::VulkanMaterial>;
        using MemoryAllocations = std::unordered_map<VkMemoryPropertyFlags, vk::VulkanAllocation>;
        using IndexType = uint32_t;

        using VertexContainer = std::vector<Vertex>;
        using IndexContainer = std::vector<IndexType>;

        using UniformBuffers = std::vector<vk::VulkanBuffer>;

        const uint32_t DefaultRenderPass = 0;

        struct VulkanRenderPass : public RenderPass
        {
            vk::VulkanRenderPass renderPass;
        };

    public:
        VulkanRenderer();

        ~VulkanRenderer();

        void Init(const char *appName, framework::Window &) override;

        ObjectDataId RegisterMesh(const std::vector<Vertex> &, std::vector<IndexType> &indices, bool) override;

        TexturesDataId RegisterTexture(TextureCreationInfo) override;

        Material CreateMaterial(Ref<Material>) override;

        void Push(ObjectData) override;

        void Submit() override;

        void EndFrame() override;

        void Begin() override;

        void BeginFrame() override;

        void End(glm::vec3 &v) override;

        void Flush(engine::Ref<engine::RenderPass>, engine::DrawType) override;

        void Cleanup() override;

        void UpdateRenderPassUniforms(Ref<RenderPass>, BindingIndex, const void *);

        void pUpdateUniformBuffer(const vk::VulkanBuffer &, const void *);

        void pUpdateUniforms(const vk::VulkanBuffer &);

        void pUpdateMaterial(vk::VulkanMaterial &);

        void pRecreateSwapChain();

        int32_t pPrepareSyncObjects();

        void setResourceManagerRef(engine::VulkanResourceManager *ref)
        {
            m_pResourceManagerRef = ref;
        }

        void *perPassData = nullptr;

    private:
        void FlushNonIndexed(vk::VulkanRenderPass *renderPass);
        void FlushIndexed(vk::VulkanRenderPass *renderPass);
        void FlushIndirect(vk::VulkanRenderPass *renderPass);
        void FlushIndexedIndirect(vk::VulkanRenderPass *renderPass);

    private:
        framework::Window *m_pWindow = nullptr;

        uint32_t m_pCurrentFrame = 0;
        int32_t m_pImageIndex = 0;

        vk::VulkanData m_pVkData;

        Frame m_pFrame;

        vk::VulkanVertexInfo m_pVertexInfo;
        bool imguiInit = false;
        uint32_t renderpass_id = 0;
        uint32_t attachedRenderPass = 0;

        std::list<DrawCommand> m_pCurrentCommandQueue;

        VulkanResourceManager *m_pResourceManagerRef = nullptr;
        bool m_pRenderPassActive = false;

        ImGuiRenderer m_pImguiRenderer{}; //!! TODO: find a better place for this
    };
}
