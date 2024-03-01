#pragma once

#if !defined(__ANDROID__)
#include <vulkan/vulkan.hpp>
#endif

#if defined(ANDROID) || defined(__ANDROID__)
#define VK_USE_PLATFORM_ANDROID_KHR
#undef VK_USE_PLATFORM_XLIB_KHR
#endif
#include <set>
#include "vk_mem_alloc.h"
#include <unordered_map>
#include <cstdint>

namespace vk
{
	static uint32_t MAX_FRAMES_IN_FLIGHT = 3;
	static uint32_t MIN_FRAMES_IN_FLIGHT = 2;
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
		VkBuffer buffer = VK_NULL_HANDLE;
		VkBufferCreateInfo bufferInfo;
		int32_t allocatedVertices = 0;
		size_t offset = 0;
		size_t size = 0;
		std::vector<SubBuffer> subBuffers;
		std::unordered_map<uint32_t, uint32_t> subBufferIndex;
		VkDescriptorBufferInfo descriptorInfo;
		VmaAllocation allocation;
		bool mapped = false;
		size_t dataSize = 0;
		std::string name = "";
	};

	struct VulkanTextureInfo
	{
		uint32_t width = 0;
		uint32_t height = 0;
		uint32_t num_channels = 0;
		uint32_t mipLevels = 1;
		uint32_t arrayLayers = 1;
		VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
		VkSampleCountFlagBits numSamples = VK_SAMPLE_COUNT_1_BIT;
		VkImageUsageFlags usage{};

		VkFilter filter{};
		VkSamplerAddressMode addressMode{};
		VkCompareOp compareOp{};
		bool compareEnable = false;
		bool isSampler = false;
	};

	enum VulkanTextureBindingType
	{
		RENDER_BUFFER_SAMPLER = 0,
		MATERIAL_SAMPLER
	};

	struct VulkanTexture
	{
		int32_t index = -1;
		VulkanTextureInfo info{};
		VulkanTextureBindingType bindingType = MATERIAL_SAMPLER;
		VkImageCreateInfo imageInfo{};
		VkImage image = VK_NULL_HANDLE;
		VkImageView imageView = VK_NULL_HANDLE;
		VkFormat format{};
		VkSampler sampler = VK_NULL_HANDLE;
		VkImageLayout imageLayout{};
		VmaAllocation allocation{};
		VkDeviceMemory deviceMemory{};

		VkFilter filter;
		VkSamplerAddressMode addressMode;
		VkCompareOp compareOp;
		bool isDepthAttachment = false;
		bool compareEnable = false;
		std::string name{};
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
		VulkanUniformBuffer()
		{
			buffers.resize(MAX_FRAMES_IN_FLIGHT);
		}
		std::vector<VulkanBuffer> buffers{};
		size_t size = 0;
	};

	struct VulkanAllocation
	{
		VkDeviceMemory deviceMemory;
		VkMemoryPropertyFlags properties;
		std::vector<VulkanBuffer> ownedBuffers;
		size_t allocatedBytes = 0;
	};

	struct VulkanGPUMappedBuffer
	{
		std::vector<VulkanBuffer> buffers;
		bool mapped = false;
		size_t size{};
	};

	struct VulkanShaderBinding
	{
		VulkanTexture texture;
		std::vector<VulkanBuffer> buffers;
		VkDescriptorType descriptorBinding;
		uint32_t bindingPoint = 0;
		bool isRenderPassAttachment = false;
		std::string renderpass = "";
		int32_t attachment_index = -1;
		std::string name = "";
	};

	struct VulkanMaterial
	{

		VulkanMaterial()
		{
			bufferInfo.resize(MAX_FRAMES_IN_FLIGHT);
		}

		VkDescriptorSetLayout descriptorSetLayout{};
		std::vector<VkDescriptorSet> descriptorSets;
		VkPipelineLayout* pipelineLayout = nullptr;
		// std::vector<VulkanTexture> renderBufferBindings;
		std::vector<VulkanShaderBinding> shaderBindings;
		std::vector<VkDescriptorBufferInfo> bufferInfo;
		size_t bufferOffset = 0;
		size_t bufferSize = 0;
		int slots = 0;
		std::string name = "";
	};

	struct VulkanComputePipeline {
		VkDescriptorSetLayout descriptorSetLayout;
		VkPipelineLayout pipelineLayout{};
		VkPipeline computePipeline;
		std::vector<VkDescriptorSet> descriptorSets;
		bool recreatePipeline = false;
	};

	struct VulkanMemoryBarrier {
		VkImageMemoryBarrier2KHR imageMemoryBarrier{};
		VkMemoryBarrier2KHR memoryBarrier{};
		VkDependencyInfoKHR dependencyInfo{};
	};


	struct VulkanMemoryBarrierInfo {
		engine::MemoryBarrierHint hint;
		VkImage image;
		VkBuffer buffer;
		VkImageSubresourceRange subresourceRange;
	};

	struct VulkanComputeShader {
		VulkanComputePipeline pipeline{};
		uint16_t groupCountX = 1;
		uint16_t groupCountY = 1;
		uint16_t groupCountZ = 1;
		std::vector<VulkanMemoryBarrier> memoryBarrier;
		std::vector<VkImageMemoryBarrier> imageMemoryBarrier;
		bool hasMemoryBarrier = false;
		bool hasImageBarrier = false;
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
		bool dirty = true;
		// VkClearColorValue clearColor = {0.1f, 0.1f, 0.1f, 1.0f};
		VkClearColorValue clearColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		VkClearDepthStencilValue depthStencilClearColor = { 1.0f, 0 };
		uint32_t numAttachments = 1;
		bool recreatePipeline = false;
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
		std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		//
		std::vector<VkImage> Images;
		//
		VkImage DepthImage;
		VulkanTexture DepthTexture;
		VulkanTextureInfo DepthTextureInfo;
		VulkanTextureBuffer DepthTextureBuffer;
		//
		std::vector<VkFormat> ImageFormats;
		VkExtent2D Extent;
		VkViewport Viewport;
		VkRect2D Scissor;
		//
		std::vector<VkImageView> ImageViews;
		std::vector<VulkanTexture> Textures;
		//
		std::vector<VkFramebuffer> Framebuffers;
		//
		bool hasDepthSampler = false;
		//
		VulkanTexture ResolveTexture;


		void setViewport(const VkViewport& v) {
			Viewport = v;
		}
		
		void setScissor(const VkRect2D& s) {
			Scissor = s;
		}
	};

	class VulkanRenderPass
	{
	public:

		VulkanRenderPass() = default;

		std::vector<VkCommandBuffer> commandBuffers;
		VkRenderPass renderPass;
		VkRenderPassBeginInfo renderPassInfo{};
		std::vector<VulkanRenderPipeline> renderPipelines;
		std::vector<VkGraphicsPipelineCreateInfo> renderPipelinesInfo;
		VulkanRenderPassData renderPassData;
		std::vector<VulkanVertexInfo> vertexInfo;
		RenderPassChain renderPassChain;
		// VkClearColorValue clearColor = {0.1f, 0.1f, 0.1f, 1.0f};
		VkClearColorValue clearColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		VkClearDepthStencilValue depthStencilClearColor = { 1.0f, 0 };
		std::vector<VkClearValue> clearValues = {};
		uint32_t id = 0;
		bool resizeWithSwapChain = false;
		bool dirtyPipeline = true;
		bool multisampled = false;
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

		VkSemaphore computeAvailableSemaphores;
		VkSemaphore computeFinishedSemaphores;
		VkFence computeInFlightFences;
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

		std::vector<VkQueue> presentQueue;
		std::vector<VkQueue> graphicsQueue;
		VkQueue transferQueue;
		std::vector<VkQueue> computeQueue;

		bool vsync = false;

#if !defined(__ANDROID__)
		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			VK_EXT_MEMORY_BUDGET_EXTENSION_NAME,
			VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
			VK_KHR_MAINTENANCE3_EXTENSION_NAME
		};
#else 
		const std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
#endif

		VkSwapchainKHR swapChain;
		// RenderPassChain swapChainData;
		VulkanRenderPass defaultRenderPass;
		bool bindless_supported = false;
		uint32_t swapChainWidth = 0;
		uint32_t swapChainHeight = 0;

		uint32_t max_memory_heaps = 0;

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
		CommandBuffers m_pComputeCommandBuffers;
	};
}