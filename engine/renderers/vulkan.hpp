#pragma once

#include "renderer.hpp"
#include "vk_data.hpp"

#include <vk/vk.hpp>

#include <vector>


namespace engine
{
    class VulkanRenderer : public Renderer
    {
        
    public:
        VulkanRenderer();

        ~VulkanRenderer();

        void Init(const char* appName, framework::Window&) override;

        void Begin() override;

        void End() override;

        void Flush() override;

        void Cleanup() override;

    private:
        framework::Window* m_pWindow;
        uint32_t m_pCurrentFrame;
        uint32_t m_pImageIndex;
        vk_data_t m_pVkData;

        VkCommandPool m_pCommandPool;
        std::vector<VkCommandBuffer> m_pCommandBuffers;
    };
}