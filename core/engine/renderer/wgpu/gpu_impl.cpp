#undef VMA_DEBUG_LOG
#undef VMA_DEBUG_LOG_FORMAT
#include "resource_manager.hpp"
#include "helpers.hpp"

#include "vk_mem_alloc.h"

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

	wgpu::VulkanBuffer VulkanResourceManager::pCreateVertexBuffer()
	{
	}

	vk::VulkanBuffer VulkanResourceManager::pCreateIndexBuffer()
	{
	}

	vk::VulkanUniformBuffer VulkanResourceManager::pCreateUniformBuffer(UniformBindingInfo bindingInfo)
	{
	}

	vk::VulkanBuffer VulkanResourceManager::pCreateStagingTextureBuffer(unsigned char *pixels, TextureCreationInfo textureInfo)
	{
	}

	void VulkanResourceManager::pCreateBuffer(vk::VulkanBuffer &buffer, size_t size, VkBufferUsageFlags usage, VmaAllocationCreateFlags properties, VmaMemoryUsage mem_usage)
	{
	}

	void VulkanResourceManager::pCreateUniformBuffers()
	{
	}

	vk::VulkanTexture VulkanResourceManager::pCreateTextureBuffer(vk::VulkanTextureInfo texInfo)
	{
	}

	Ref<Buffer> VulkanResourceManager::pFetchVertexBuffer(uint32_t numVertices, size_t vertexSize)
	{
	}

	Ref<Buffer> VulkanResourceManager::pFetchIndexBuffer(uint32_t numIndices, uint32_t maxOffset)
	{
	}

	void VulkanResourceManager::pCreateDescriptorPool()
	{
	}

	void VulkanResourceManager::pCreateGlobalDescriptorPool()
	{
	}
	void VulkanResourceManager::pCreateGlobalDescriptorSets(VkDescriptorSetLayout layout, VkDescriptorPool &pool, VkDescriptorSet &descriptorSets, uint32_t count)
	{
	}

	void VulkanResourceManager::pCreateDescriptorSets(/*vk::VulkanMaterial &material*/ VkDescriptorSetLayout layout, VkDescriptorPool &pool, std::vector<VkDescriptorSet> &descriptorSets, uint32_t count)
	{
	}

	void VulkanResourceManager::pRecreateDescriptorSets()
	{
	}

	vk::VulkanBuffer VulkanResourceManager::pCreateStagingBuffer(const std::vector<common::Vertex> &vertices)
	{
	}

	vk::VulkanBuffer VulkanResourceManager::pCreateStagingBuffer(const std::vector<common::AnimatedVertex> &vertices)
	{
	}

	vk::VulkanBuffer VulkanResourceManager::pCreateStagingIndexBuffer(const std::vector<IndexType> &indices)
	{
	}

	void VulkanResourceManager::pUpdateMaterial(vk::VulkanMaterial &material)
	{
	}

	Ref<PushConstant> VulkanResourceManager::createPushConstant(const std::string &name, PushConstantData push_constant)
	{
	}

	void VulkanResourceManager::pRecreateFrameBuffers(VkExtent2D extent)
	{
	}

	void VulkanResourceManager::pGenerateMipMaps(TextureCreationInfo texInfo, vk::VulkanTexture &texture)
	{
	}

}