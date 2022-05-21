#pragma once

#include "renderer.hpp"

#include <vk/vk.hpp>


#include <vector>

namespace engine
{
    class VulkanRenderer : public Renderer
    {
        using RenderPipelines = std::vector<vk::VulkanRenderPipeline>;
        using CommandPools = std::vector<VkCommandPool>;
        using CommandBuffers = std::vector<VkCommandBuffer>;

    public:
        VulkanRenderer();

        ~VulkanRenderer();

        void Init(const char *appName, framework::Window &) override;

        void Begin() override;

        void End() override;

        void Flush() override;

        void Cleanup() override;

    private:
        framework::Window *m_pWindow;

        uint32_t m_pCurrentFrame;
        uint32_t m_pImageIndex;

        vk::VulkanData m_pVkData;

        RenderPipelines m_pRenderPipelines;
        CommandPools m_pCommandPools;
        CommandBuffers m_pCommandBuffers;
    };
}