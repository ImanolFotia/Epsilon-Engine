#pragma once

#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>

namespace vk
{
    static const uint32_t MAX_FRAMES_IN_FLIGHT = 2;
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
        size_t size = 0;
        std::vector<SubBuffer> subBuffers;
        std::unordered_map<uint32_t, uint32_t> subBufferIndex;
        VkDescriptorBufferInfo descriptorInfo;
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

    struct VulkanUniformBuffer {
        VulkanBuffer buffers[MAX_FRAMES_IN_FLIGHT];
        size_t size;
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
        VkDescriptorSetLayout descriptorSetLayout{};
        std::vector<VkDescriptorSet> descriptorSets;
        VkPipelineLayout *pipelineLayout = nullptr;
        std::vector<VulkanTexture> textures;
        VkDescriptorBufferInfo bufferInfo[MAX_FRAMES_IN_FLIGHT];
        size_t bufferOffset = 0;
        size_t bufferSize = 0;
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
    struct RenderPassChain
    {
        const std::vector<const char *> deviceExtensions = {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        std::vector<VkImage> Images;

        VkImage DepthImage;
        VulkanTexture DepthTexture;
        VulkanTextureInfo DepthTextureInfo;
        VulkanTextureBuffer DepthTextureBuffer;

        VkFormat ImageFormat;
        VkExtent2D Extent;
        std::vector<VkImageView> ImageViews;

        std::vector<VkFramebuffer> Framebuffers;
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
        VulkanVertexInfo vertexInfo;
        RenderPassChain renderPassChain;
        VkClearColorValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
        VkClearDepthStencilValue depthStencilClearColor = {1.0f, 0};
        VkClearValue clearValues[2] = {};
        uint32_t id;

        VulkanUniformBuffer uniformBuffer;
    };

    struct VulkanSyncObject
    {
        VkSemaphore imageAvailableSemaphores;
        VkSemaphore renderFinishedSemaphores;
        VkFence inFlightFences;
    };



    struct VulkanData
    {

        using CommandPools = std::vector<VkCommandPool>;
        using CommandBuffers = std::vector<VkCommandBuffer>;
        
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
        //RenderPassChain swapChainData;
        VulkanRenderPass defaultRenderPass;
        /*
        std::vector<VkImage> swapChainImages;
        VulkanTexture swapChainDepthTexture;
        VulkanTextureInfo swapChainDepthTextureInfo;
        VulkanTextureBuffer swapChainDepthTextureBuffer;
        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;
        std::vector<VkImageView> swapChainImageViews;

        std::vector<VkFramebuffer> swapChainFramebuffers;
*/
        std::vector<VulkanSyncObject> syncObjects;


        CommandPools m_pCommandPools;
        CommandBuffers m_pCommandBuffers;
    };
}