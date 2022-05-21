#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>

namespace engine
{
    struct vk_data_t
    {
        VkInstance instance;

        VkSurfaceKHR surface;
        VkDevice logicalDevice;
        VkPhysicalDevice physicalDevice;

        VkPipelineLayout pipelineLayout{};

        VkQueue presentQueue;
        VkQueue graphicsQueue;
        VkPipeline graphicsPipeline;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        VkPipelineMultisampleStateCreateInfo multisampling{};
        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        VkPipelineColorBlendStateCreateInfo colorBlending{};

        VkRenderPass renderPass{};

        VkRenderPassBeginInfo renderPassInfo{};
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

        VkPipelineViewportStateCreateInfo viewportState{};
        VkRect2D scissor{};
        VkViewport viewport{};
    };
}