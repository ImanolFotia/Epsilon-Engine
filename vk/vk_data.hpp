#pragma once

#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>

namespace vk
{
    const size_t ALLOCATION_SIZE_MB = 0xFFFFFFF;

    struct VulkanVertexInfo
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
        VkVertexInputBindingDescription bindingDescription{};
    };

    struct SubBuffer
    {
        uint32_t num_vertices = 0;
        size_t offset = 0;
    };

    struct VulkanBuffer
    {
        VkBuffer buffer;
        VkBufferCreateInfo bufferInfo;
        uint32_t allocatedVertices = 0;
        size_t offset = 0;
        std::vector<SubBuffer> subBuffers;
        std::unordered_map<uint32_t, uint32_t> subBufferIndex;
        VmaAllocation allocation;
    };

    struct VulkanTextureInfo
    {
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t num_channels = 0;
        VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
        VkImageUsageFlags usage;
    };

    struct VulkanTexture
    {
        VulkanTextureInfo info;
        VkImageCreateInfo imageInfo;
        VkImage image;
        VkImageView imageView;
        VkFormat format;
        VkSampler sampler;
    };

    struct VulkanTextureBuffer
    {
        VkImageCreateInfo imageInfo;
        size_t allocatedSize = 0;
        VkDeviceMemory deviceMemory;
        size_t offset = 0;
    };

    struct VulkanAllocation
    {
        VkDeviceMemory deviceMemory;
        VkMemoryPropertyFlags properties;
        std::vector<VulkanBuffer> ownedBuffers;
        size_t allocatedBytes;
    };

    struct VulkanMaterial
    {
        std::vector<VkDescriptorSet> descriptorSets;
        VkPipelineLayout *pipelineLayout = nullptr;
        std::vector<VulkanTexture> textures;
    };

    struct VulkanRenderPipeline
    {
        VkDescriptorSetLayout descriptorSetLayout{};
        VkPipelineLayout pipelineLayout{};
        VkPipeline graphicsPipeline;
        VkPipelineViewportStateCreateInfo viewportState{};
        VkRect2D scissor{};
        VkViewport viewport{};
        VkPipelineRasterizationStateCreateInfo rasterizer{};
        VkPipelineMultisampleStateCreateInfo multisampling{};
        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        VkPipelineColorBlendStateCreateInfo colorBlending{};

        VkClearColorValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
        VkClearDepthStencilValue depthStencilClearColor = {1.0f, 0};
    };

    struct VulkanRenderPassData {
        std::vector<VkAttachmentDescription> colorAttachments{};
        std::vector<VkAttachmentReference> colorAttachmentRefs{};

        VkSubpassDescription subpass{};
        VkSubpassDependency dependency{};

        VkAttachmentDescription depthAttachment{};
        VkAttachmentReference depthAttachmentRef{};

        bool hasDepthAttachment = false;
    };

    struct VulkanRenderPassAttachment
    {
        VkFormat format;
        VkSampleCountFlags sampleCount;
        VkAttachmentLoadOp loadOp;
        VkAttachmentStoreOp storeOp;
        VkAttachmentLoadOp stencilLoadOp;
        VkAttachmentStoreOp stencilStoreOp;
        VkImageLayout initialLayout;
        VkImageLayout finalLayout;
    };

    struct VulkanRenderPass
    {
        VulkanRenderPass() {
            clearValues[0].color = clearColor;
            clearValues[1].depthStencil = depthStencilClearColor;
        }
        VkRenderPass renderPass;
        VkRenderPassBeginInfo renderPassInfo{};
        std::vector<VulkanRenderPipeline> renderPipelines;
        VulkanRenderPassData renderPassData;
        VkClearColorValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
        VkClearDepthStencilValue depthStencilClearColor = {1.0f, 0};
        VkClearValue clearValues[2] = {};
    };

    struct VulkanSyncObject
    {
        VkSemaphore imageAvailableSemaphores;
        VkSemaphore renderFinishedSemaphores;
        VkFence inFlightFences;
    };

    struct VulkanSwapChain
    {

        const std::vector<const char *> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        VkSwapchainKHR swapChain;
        std::vector<VkImage> swapChainImages;
        VkImage swapChainDepthImage;
        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;
        std::vector<VkImageView> swapChainImageViews;

        std::vector<VkFramebuffer> swapChainFramebuffers;
    };

    struct VulkanData
    {
        VkInstance instance;
        VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
        VkSurfaceKHR surface;
        VkDevice logicalDevice;
        VkPhysicalDevice physicalDevice;

        VkQueue presentQueue;
        VkQueue graphicsQueue;

        const std::vector<const char *> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        VkSwapchainKHR swapChain;
        std::vector<VkImage> swapChainImages;
        VulkanTexture swapChainDepthTexture;
        VulkanTextureInfo swapChainDepthTextureInfo;
        VulkanTextureBuffer swapChainDepthTextureBuffer;
        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;
        std::vector<VkImageView> swapChainImageViews;

        std::vector<VkFramebuffer> swapChainFramebuffers;

        std::vector<VulkanSyncObject> syncObjects;
    };
}