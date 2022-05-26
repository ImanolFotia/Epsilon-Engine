#pragma once

#include "renderer.hpp"

#include <vk/vk.hpp>

#include <vector>
#include <list>

namespace engine
{
    class VulkanRenderer : public Renderer
    {
        using RenderPipelines = std::vector<vk::VulkanRenderPipeline>;
        using CommandPools = std::vector<VkCommandPool>;
        using CommandBuffers = std::vector<VkCommandBuffer>;
        using VertexBuffers = std::vector<vk::VulkanBuffer>;

    public:
        VulkanRenderer();

        ~VulkanRenderer();

        void Init(const char *appName, framework::Window &) override;

        uint32_t Submit(const std::vector<Vertex> &, const MaterialInfo &) override;

        void Push(uint32_t) override;

        void Begin() override;

        void End() override;

        void Flush() override;

        void Cleanup() override;

    private:
        vk::VulkanBuffer *allocateBuffer();

    private:
        framework::Window *m_pWindow = nullptr;

        uint32_t m_pCurrentFrame = 0;
        uint32_t m_pImageIndex = 0;

        vk::VulkanData m_pVkData;

        RenderPipelines m_pRenderPipelines;
        CommandPools m_pCommandPools;
        CommandBuffers m_pCommandBuffers;
        VertexBuffers m_pVertexBuffers;
        vk::VulkanVertexInfo<6> m_pVertexInfo;

        uint32_t current_vertex_count = 0;
        std::list<uint32_t> m_pCurrentFrameObjects;
    };
}