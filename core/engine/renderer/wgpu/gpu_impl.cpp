#include "resource_manager.hpp"
#include "helpers.hpp"

#ifdef WIN32
#if !defined(__PRETTY_FUNCTION__) && !defined(__GNUC__)
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

#endif

/**
 * @brief Implementation of the Vulkan renderer private methods
 *
 */
namespace engine
{

	wgpu::WGPUBuffer WGPUResourceManager::pCreateVertexBuffer()
	{
	}

	wgpu::WGPUBuffer WGPUResourceManager::pCreateIndexBuffer()
	{
	}

	wgpu::WGPUUniformBuffer WGPUResourceManager::pCreateUniformBuffer(UniformBindingInfo bindingInfo)
	{
	}

	wgpu::WGPUBuffer WGPUResourceManager::pCreateStagingTextureBuffer(unsigned char *pixels, TextureCreationInfo textureInfo)
	{
	}

	void WGPUResourceManager::pCreateBuffer(wgpu::WGPUBuffer &buffer, size_t size, VkBufferUsageFlags usage, VmaAllocationCreateFlags properties, VmaMemoryUsage mem_usage)
	{
	}

	void WGPUResourceManager::pCreateUniformBuffers()
	{
	}

	wgpu::WGPUTexture WGPUResourceManager::pCreateTextureBuffer(wgpu::WGPUTextureInfo texInfo)
	{
	}

	Ref<Buffer> WGPUResourceManager::pFetchVertexBuffer(uint32_t numVertices, size_t vertexSize)
	{
	}

	Ref<Buffer> WGPUResourceManager::pFetchIndexBuffer(uint32_t numIndices, uint32_t maxOffset)
	{
	}

	void WGPUResourceManager::pCreateDescriptorPool()
	{
	}

	void WGPUResourceManager::pCreateGlobalDescriptorPool()
	{
	}
	void WGPUResourceManager::pCreateGlobalDescriptorSets(VkDescriptorSetLayout layout, VkDescriptorPool &pool, VkDescriptorSet &descriptorSets, uint32_t count)
	{
	}

	void WGPUResourceManager::pCreateDescriptorSets(/*vk::VulkanMaterial &material*/ VkDescriptorSetLayout layout, VkDescriptorPool &pool, std::vector<VkDescriptorSet> &descriptorSets, uint32_t count)
	{
	}

	void WGPUResourceManager::pRecreateDescriptorSets()
	{
	}

	wgpu::WGPUBuffer WGPUResourceManager::pCreateStagingBuffer(const std::vector<common::Vertex> &vertices)
	{
	}

	wgpu::WGPUBuffer WGPUResourceManager::pCreateStagingBuffer(const std::vector<common::AnimatedVertex> &vertices)
	{
	}

	wgpu::WGPUBuffer WGPUResourceManager::pCreateStagingIndexBuffer(const std::vector<IndexType> &indices)
	{
	}

	void WGPUResourceManager::pUpdateMaterial(wgpu::WGPUMaterial &material)
	{
	}

	Ref<PushConstant> WGPUResourceManager::createPushConstant(const std::string &name, PushConstantData push_constant)
	{
	}

	void WGPUResourceManager::pRecreateFrameBuffers(VkExtent2D extent)
	{
	}

	void WGPUResourceManager::pGenerateMipMaps(TextureCreationInfo texInfo, wgpu::WGPUTexture &texture)
	{
	}

}