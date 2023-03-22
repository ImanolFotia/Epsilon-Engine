#pragma once

#if (BUILD_ANDROID == 0)
#include <vulkan/vulkan.hpp>
#endif

#if defined(ANDROID) || defined(__ANDROID__)
#define VK_USE_PLATFORM_ANDROID_KHR
#undef VK_USE_PLATFORM_XLIB_KHR
#endif
#include <set>
#include "vk_mem_alloc.h"
#include <unordered_map>

namespace vk
{
    static const uint32_t MAX_FRAMES_IN_FLIGHT = 3;
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
        uint32_t id = 0;
        VkBuffer buffer;
        VkBufferCreateInfo bufferInfo;
        uint32_t allocatedVertices = 0;
        size_t offset = 0;
        size_t size = 0;
        std::vector<SubBuffer> subBuffers;
        std::unordered_map<uint32_t, uint32_t> subBufferIndex;
        VkDescriptorBufferInfo descriptorInfo;
        VmaAllocation allocation;
        std::string name = "";
    };

    struct VulkanTextureInfo
    {
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t num_channels = 0;
        uint32_t mipLevels = 1;
        VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
        VkImageUsageFlags usage;

        VkFilter filter;
        VkSamplerAddressMode addressMode;
        VkCompareOp compareOp;
        bool compareEnable = false;
    };

    enum VulkanTextureBindingType
    {
        RENDER_BUFFER_SAMPLER = 0,
        MATERIAL_SAMPLER
    };

    struct VulkanTexture
    {
        uint32_t index = 0;
        VulkanTextureInfo info;
        VulkanTextureBindingType bindingType = MATERIAL_SAMPLER;
        VkImageCreateInfo imageInfo;
        VkImage image = VK_NULL_HANDLE;
        VkImageView imageView = VK_NULL_HANDLE;
        VkFormat format;
        VkSampler sampler = VK_NULL_HANDLE;
        VmaAllocation allocation;

        VkFilter filter;
        VkSamplerAddressMode addressMode;
        VkCompareOp compareOp;
        bool compareEnable = false;
    };

    struct VulkanTextureBuffer
    {
        VkImageCreateInfo imageInfo;
        size_t allocatedSize = 0;
        VkDeviceMemory deviceMemory;
        size_t offset = 0;
    };

    struct VulkanUniformBuffer
    {
        VulkanBuffer buffers[MAX_FRAMES_IN_FLIGHT];
        size_t size = 0;
    };

    struct VulkanAllocation
    {
        VkDeviceMemory deviceMemory;
        VkMemoryPropertyFlags properties;
        std::vector<VulkanBuffer> ownedBuffers;
        size_t allocatedBytes = 0;
    };

    struct VulkanShaderBinding
    {
        VulkanTexture texture;
        VkDescriptorType descriptorBinding;
        uint32_t bindingPoint = 0;
        bool isRenderPassAttachment = false;
        std::string name = "";
    };

    struct VulkanMaterial
    {
        VkDescriptorSetLayout descriptorSetLayout{};
        std::vector<VkDescriptorSet> descriptorSets;
        VkPipelineLayout *pipelineLayout = nullptr;
        // std::vector<VulkanTexture> renderBufferBindings;
        std::vector<VulkanShaderBinding> shaderBindings;
        VkDescriptorBufferInfo bufferInfo[MAX_FRAMES_IN_FLIGHT];
        size_t bufferOffset = 0;
        size_t bufferSize = 0;
        int slots = 0;
        std::string name = "";
    };

    struct VulkanRenderPipeline
    {
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
        std::vector<VkPipelineLayout> pipelineLayout{};
        VkPipeline graphicsPipeline;
        VkPipelineViewportStateCreateInfo viewportState{};
        VkRect2D scissor{};
        VkViewport viewport{};
        VkPipelineRasterizationStateCreateInfo rasterizer{};
        VkPipelineMultisampleStateCreateInfo multisampling{};
        std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
        VkPipelineColorBlendStateCreateInfo colorBlending{};
        VkFrontFace winding;
        VkCullModeFlags cullMode;

        // VkClearColorValue clearColor = {0.1f, 0.1f, 0.1f, 1.0f};
        VkClearColorValue clearColor = {1.0f, 1.0f, 1.0f, 1.0f};
        VkClearDepthStencilValue depthStencilClearColor = {1.0f, 0};
        uint32_t numAttachments = 1;
    };

    struct VulkanRenderPassData
    {
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

        std::vector<VkFormat> ImageFormats;
        VkExtent2D Extent;
        std::vector<VkImageView> ImageViews;
        std::vector<VulkanTexture> Textures;

        std::vector<VkFramebuffer> Framebuffers;
    };

    struct VulkanGPUMappedBuffer
    {
        VulkanBuffer buffer;
    };

    class VulkanRenderPass
    {
    public:
        VulkanRenderPass()
        {
            // clearValues.resize(2);
            // clearValues[0].color = clearColor;
            // clearValues[1].depthStencil = depthStencilClearColor;
        }

        VkRenderPass renderPass;
        VkRenderPassBeginInfo renderPassInfo{};
        std::vector<VulkanRenderPipeline> renderPipelines;
        VulkanRenderPassData renderPassData;
        std::vector<VulkanVertexInfo> vertexInfo;
        RenderPassChain renderPassChain;
        // VkClearColorValue clearColor = {0.1f, 0.1f, 0.1f, 1.0f};
        VkClearColorValue clearColor = {1.0f, 1.0f, 1.0f, 1.0f};
        VkClearDepthStencilValue depthStencilClearColor = {1.0f, 0};
        std::vector<VkClearValue> clearValues = {};
        uint32_t id = 0;

        std::list<VulkanUniformBuffer> uniformBuffer;
        uint32_t numAttachments = 1;
        std::string name = "";
    };

    struct VulkanTechnique
    {
        std::vector<VulkanRenderPass> passes;
        std::vector<VulkanBuffer> buffers;
        std::vector<VulkanBuffer> textures;
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
        std::set<uint32_t> uniqueQueueFamilies;

        VkQueue presentQueue;
        VkQueue graphicsQueue;

        const std::vector<const char *> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        VkSwapchainKHR swapChain;
        // RenderPassChain swapChainData;
        VulkanRenderPass defaultRenderPass;
        bool bindless_supported = false;
        uint32_t swapChainWidth = 0;
        uint32_t swapChainHeight = 0;

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