#pragma once
// #undef VMA_DEBUG_LOG
// #undef VMA_DEBUG_LOG_FORMAT
#include "core/engine/renderer/renderer.hpp"
#include "core/engine/renderer/frame.hpp"
#include "core/engine/renderer/types.hpp"
#include <core/common/common.hpp>

#include "apis/wgpu/wgpu.hpp"

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

class ImGuiRenderer;

namespace engine
{
    class WGPUResourceManager;
    class WGPURenderer : public Renderer
    {
        using RenderPasses = std::unordered_map<uint32_t, wgpu::WGPURenderPass>;
        using VertexBuffers = std::vector<wgpu::WGPUBuffer>;
        using IndexBuffers = std::vector<wgpu::WGPUBuffer>;
        using TextureBuffers = std::vector<vk::WGPUTextureBuffer>;
        using Textures = std::vector<wgpu::WGPUTexture>;
        using Materials = std::vector<wgpu::WGPUMaterial>;
        using MemoryAllocations = std::unordered_map<VkMemoryPropertyFlags, wgpu::WGPUAllocation>;
        using IndexType = uint32_t;

        using VertexContainer = std::vector<common::Vertex>;
        using IndexContainer = std::vector<IndexType>;

        using UniformBuffers = std::vector<wgpu::WGPUBuffer>;

        const uint32_t DefaultRenderPass = 0;

        struct WGPURenderPass : public RenderPass
        {
            wgpu::WGPURenderPass renderPass;
        };

    public:
        WGPURenderer();

        ~WGPURenderer();

        void Init(const char *appName, framework::Window &) override;

        ObjectDataId RegisterMesh(const std::vector<common::Vertex> &, std::vector<IndexType> &indices, bool) override;

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

        void pUpdateUniformBuffer(const wgpu::WGPUBuffer &, const void *);

        void pUpdateUniforms(const wgpu::WGPUBuffer &);

        void pUpdateMaterial(wgpu::WGPUMaterial &);

        void setResourceManagerRef(engine::WGPUResourceManager *ref)
        {
            m_pResourceManagerRef = ref;
        }

        void *perPassData = nullptr;

        std::shared_ptr<ImGuiRenderer> getDebugRenderer() override;

        void SetViewport(const Viewport &) override;

        void SetScissor(const Scissor &) override;

        void SetVSync(bool) override;

        void SetRenderPass(Ref<RenderPass>) override;

        void InitDebugRenderer() override;

    private:
        void pRecreateSwapChain();
        int32_t pPrepareSyncObjects();

        void FlushNonIndexed(wgpu::WGPURenderPass *renderPass);
        void FlushIndexed(wgpu::WGPURenderPass *renderPass);
        void FlushIndirect(wgpu::WGPURenderPass *renderPass);
        void FlushIndexedIndirect(wgpu::WGPURenderPass *renderPass);

    private:
        framework::Window *m_pWindow = nullptr;

        int32_t m_pImageIndex = 0;

        wgpu::WGPUData m_pWGPUData;

        Frame m_pFrame;

        wgpu::WGPUVertexInfo m_pVertexInfo;
        bool imguiInit = false;
        uint32_t renderpass_id = 0;
        uint32_t attachedRenderPass = 0;
        uint32_t m_pNumDrawCalls = 0;
        engine::Ref<engine::RenderPass> m_pActiveRenderPass;

        WGPUResourceManager *m_pResourceManagerRef = nullptr;
        bool m_pRenderPassActive = false;
        bool m_pShouldRecreateSwapchain = false;

        std::shared_ptr<ImGuiRenderer> m_pImguiRenderer; //!! TODO: find a better place for this

        std::vector<VkCommandBuffer> m_pSecondaryCommandBuffers;
    };
}
