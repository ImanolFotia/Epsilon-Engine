#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>

namespace engine
{
    struct VulkanRenderPipeline
    {
        VkPipelineLayout pipelineLayout{};
        VkPipeline graphicsPipeline;
        VkRenderPass renderPass;
        VkRenderPassBeginInfo renderPassInfo{};
        VkPipelineViewportStateCreateInfo viewportState{};
        VkRect2D scissor{};
        VkViewport viewport{};

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        VkPipelineMultisampleStateCreateInfo multisampling{};
        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        VkPipelineColorBlendStateCreateInfo colorBlending{};
    };

    struct VulkanData
    {
        VkInstance instance;

        VkSurfaceKHR surface;
        VkDevice logicalDevice;
        VkPhysicalDevice physicalDevice;

        VkQueue presentQueue;
        VkQueue graphicsQueue;

        const std::vector<const char *> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        VkSwapchainKHR swapChain;
        std::vector<VkImage> swapChainImages;
        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;
        std::vector<VkImageView> swapChainImageViews;

        std::vector<VkFramebuffer> swapChainFramebuffers;

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
    };
}