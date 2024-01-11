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

	vk::VulkanBuffer VulkanResourceManager::pCreateVertexBuffer()
	{
		vk::VulkanBuffer buffer;
		pCreateBuffer(buffer, sizeof(common::Vertex) * MAX_VERTICES_PER_BUFFER, VERTEX_BUFFER_USAGE, VERTEX_BUFFER_PROP, VERTEX_BUFFER_MEM_USAGE);
		if (0)
			IO::Info("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "allocating ", sizeof(common::Vertex) * MAX_VERTICES_PER_BUFFER, " bytes in local vertex buffer");
		return buffer;
	}

	vk::VulkanBuffer VulkanResourceManager::pCreateIndexBuffer()
	{
		vk::VulkanBuffer buffer;
		pCreateBuffer(buffer, sizeof(IndexType) * MAX_INDICES_PER_BUFFER, INDEX_BUFFER_USAGE, INDEX_BUFFER_PROP, INDEX_BUFFER_MEM_USAGE);
		if (0)
			IO::Info("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "allocating ", sizeof(IndexType) * MAX_INDICES_PER_BUFFER, " bytes in local index buffer");
		return buffer;
	}

	vk::VulkanUniformBuffer VulkanResourceManager::pCreateUniformBuffer(UniformBindingInfo bindingInfo)
	{
		// uniformBufferPool
		vk::VulkanUniformBuffer buffer;
		buffer.size = bindingInfo.size;
		for (auto &i : buffer.buffers)
		{
			i.size = buffer.size;
			pCreateBuffer(i, bindingInfo.size, UNIFORM_BUFFER_USAGE, UNIFORM_BUFFER_PROP,
						  UNIFORM_BUFFER_MEM_USAGE);
			IO::Info("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "allocating ", size,
					 " bytes in local uniform buffer");
		}
		return buffer;
	}

	vk::VulkanBuffer VulkanResourceManager::pCreateStagingTextureBuffer(unsigned char *pixels, TextureCreationInfo textureInfo)
	{
		vk::VulkanBuffer stagingBuffer;
		size_t imageSize = 0;
		if (textureInfo.offsets.size() > 0)
		{
			imageSize = textureInfo.size;
		}
		else
		{
			imageSize = textureInfo.width * textureInfo.height * /*textureInfo.numChannels*/ 4;
			if (textureInfo.format == COLOR_RGBA_16F || textureInfo.format == COLOR_RGBA_32F || textureInfo.format == COLOR_RGB_32F || textureInfo.format == COLOR_RGB_16F)
				imageSize *= sizeof(float);
			if (textureInfo.format == COLOR_RG_16F || textureInfo.format == COLOR_RG_32F)
			{
				imageSize = textureInfo.width * textureInfo.height * 2 * sizeof(float);
			}
		}
		pCreateBuffer(stagingBuffer, imageSize, TEXTURE_BUFFER_USAGE, TEXTURE_BUFFER_PROP, TEXTURE_BUFFER_MEM_USAGE);

		void *data;
		vmaMapMemory(m_pAllocator, stagingBuffer.allocation, &data);
		memcpy(data, pixels, static_cast<size_t>(imageSize));
		vmaUnmapMemory(m_pAllocator, stagingBuffer.allocation);

		return stagingBuffer;
	}

	void VulkanResourceManager::pCreateBuffer(vk::VulkanBuffer &buffer, size_t size, VkBufferUsageFlags usage, VmaAllocationCreateFlags properties, VmaMemoryUsage mem_usage)
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = mem_usage;
		allocInfo.flags = properties;

		VmaAllocation allocation;

		if (usage == VERTEX_BUFFER_USAGE)
			ResourcesMemory.m_pVertexBufferAllocationSize += size;
		if (usage == INDEX_BUFFER_USAGE)
			ResourcesMemory.m_pIndexBufferAllocationSize += size;
		if (usage == UNIFORM_BUFFER_USAGE)
			ResourcesMemory.m_pUniformBufferAllocationSize += size;
		if (usage == STORAGE_BUFFER_USAGE)
			ResourcesMemory.m_pStorageBufferAllocationSize += size;

		vmaCreateBuffer(m_pAllocator, &bufferInfo, &allocInfo, &buffer.buffer, &buffer.allocation, nullptr);
	}

	void VulkanResourceManager::pCreateUniformBuffers()
	{
		// VkDeviceSize bufferSize = sizeof(ShaderData);

		// m_pUniformBuffers.resize(vk::MAX_FRAMES_IN_FLIGHT);

		// for (size_t i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++)
		//{
		//     pCreateUniformBuffer(bufferSize);
		// }
	}

	vk::VulkanTexture VulkanResourceManager::pCreateTextureBuffer(vk::VulkanTextureInfo texInfo)
	{
		vk::VulkanTexture texture;

		texture.info = texInfo;

		texture.imageInfo = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
		texture.imageInfo.usage = texInfo.usage; // VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

		texture.imageInfo.extent.width = static_cast<uint32_t>(texInfo.width);
		texture.imageInfo.extent.height = static_cast<uint32_t>(texInfo.height);
		texture.imageInfo.extent.depth = 1;
		texture.compareEnable = texInfo.compareEnable;
		texture.imageInfo.mipLevels = texInfo.mipLevels;
		texture.imageInfo.arrayLayers = texInfo.arrayLayers;
		texture.imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		texture.imageInfo.imageType = VK_IMAGE_TYPE_2D;

		texture.imageInfo.format = texInfo.format;
		texture.format = texInfo.format;
		texture.imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		texture.imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		texture.imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		texture.imageInfo.flags = 0; // Optional
		texture.imageInfo.usage = texInfo.usage;
		if (!(texture.imageInfo.usage & VK_IMAGE_USAGE_TRANSFER_DST_BIT))
		{
			texture.imageInfo.usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		}

		texture.addressMode = texInfo.addressMode;
		texture.compareOp = texInfo.compareOp;
		texture.filter = texInfo.filter;

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = VMA_MEMORY_USAGE_AUTO;

		int num_channels = 4;
		int size = 0;
		if (texInfo.format == VK_FORMAT_R8G8B8A8_UNORM || texInfo.format == VK_FORMAT_R8G8B8A8_SRGB)
		{
			num_channels = 4;
			size = 1;
		}
		if (texInfo.format == VK_FORMAT_D32_SFLOAT || texInfo.format == VK_FORMAT_D32_SFLOAT_S8_UINT || texInfo.format == VK_FORMAT_R32_SFLOAT)
		{
			num_channels = 1;
			size = 4;
		}

		vmaCreateImage(m_pAllocator, &texture.imageInfo, &allocInfo, &texture.image, &texture.allocation, nullptr);
		// std::cout << texInfo.width * texInfo.height * num_channels * size << std::endl;
		return texture;
	}

	Ref<Buffer> VulkanResourceManager::pFetchVertexBuffer(uint32_t numVertices, size_t vertexSize)
	{
		
		for (auto &bufferRef : vertexBufferReferences)
		{
			auto buffer = vertexBufferPool.get(bufferRef);

			if (buffer->dataSize != vertexSize)
				continue;
			// if (MAX_VERTICES_PER_BUFFER * vertexSize >= buffer->allocatedVertices * vertexSize + numVertices * vertexSize)
			if (numVertices <= MAX_VERTICES_PER_BUFFER - buffer->allocatedVertices)
			{
				return bufferRef;
			}
		}

		if (numVertices > MAX_VERTICES_PER_BUFFER)
		{

			vk::VulkanBuffer buffer;
			pCreateBuffer(buffer, sizeof(common::Vertex) * numVertices, VERTEX_BUFFER_USAGE, VERTEX_BUFFER_PROP, VERTEX_BUFFER_MEM_USAGE);
			buffer.id = vertexBufferCount;
			buffer.allocatedVertices = 0;
			buffer.dataSize = vertexSize;
			vertexBufferCount++;
			auto ref = vertexBufferPool.insert(("VertexBuffer_" + std::to_string(vertexBufferCount)), buffer);
			vertexBufferReferences.push_back(ref);

			return ref;
		}

		auto vertexBuffer = pCreateVertexBuffer();
		vertexBuffer.id = vertexBufferCount;
		vertexBuffer.allocatedVertices = 0;
		vertexBuffer.dataSize = vertexSize;
		vertexBufferCount++;
		auto ref = vertexBufferPool.insert(("VertexBuffer_" + std::to_string(vertexBufferCount)), vertexBuffer);
		vertexBufferReferences.push_back(ref);

		return ref;
	}

	Ref<Buffer> VulkanResourceManager::pFetchIndexBuffer(uint32_t numIndices, uint32_t maxOffset)
	{
		for (auto &bufferRef : indexBufferReferences)
		{
			auto buffer = indexBufferPool.get(bufferRef);

			// int maxAllocatingSize = sizeof(IndexType) * (buffer->allocatedVertices + numIndices);
			// if (MAX_INDICES_PER_BUFFER * sizeof(IndexType) >= maxAllocatingSize)
			if (numIndices <= MAX_INDICES_PER_BUFFER - buffer->allocatedVertices)
			{
				return bufferRef;
			}
		}

		if (numIndices > MAX_INDICES_PER_BUFFER)
		{
			vk::VulkanBuffer buffer;
			pCreateBuffer(buffer, sizeof(IndexType) * numIndices, INDEX_BUFFER_USAGE, INDEX_BUFFER_PROP, INDEX_BUFFER_MEM_USAGE);
			buffer.id = indexBufferCount;
			buffer.allocatedVertices = 0;
			indexBufferCount++;
			auto ref = indexBufferPool.insert(("IndexBuffer_" + std::to_string(indexBufferCount)), buffer);
			indexBufferReferences.push_back(ref);

			return ref;
		}
		auto indexBuffer = pCreateIndexBuffer();
		indexBuffer.id = indexBufferCount;
		indexBuffer.allocatedVertices = 0;
		indexBufferCount++;
		auto ref = indexBufferPool.insert(("IndexBuffer_" + std::to_string(indexBufferCount)), indexBuffer);
		indexBufferReferences.push_back(ref);

		return ref;
	}

	void VulkanResourceManager::pCreateDescriptorPool()
	{
		// Create default descriptor pool
		vkDestroyDescriptorPool(m_pVkDataPtr->logicalDevice, m_pDescriptorPool, nullptr);
		std::array<VkDescriptorPoolSize, 11> poolSizes =
			{{{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
			  {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
			  {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
			  {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
			  {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
			  {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
			  {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
			  {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
			  {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
			  {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
			  {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}}};

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

		poolInfo.maxSets = poolSizes.size() * vk::MAX_FRAMES_IN_FLIGHT;
		if (vkCreateDescriptorPool(m_pVkDataPtr->logicalDevice, &poolInfo, nullptr, &m_pDescriptorPool) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}

	void VulkanResourceManager::pCreateGlobalDescriptorPool()
	{

		// Create Global bindless descriptor pool
		// vkDestroyDescriptorPool(m_pVkDataPtr->logicalDevice, m_pGlobalDescriptorPool, nullptr);
		std::array<VkDescriptorPoolSize, 1> globalPoolSizes{};
		globalPoolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		globalPoolSizes[0].descriptorCount = MAX_BINDLESS_RESOURCES;

		VkDescriptorPoolCreateInfo globalPoolInfo{};
		globalPoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT | VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		globalPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		globalPoolInfo.poolSizeCount = static_cast<uint32_t>(globalPoolSizes.size());
		globalPoolInfo.pPoolSizes = globalPoolSizes.data();

		globalPoolInfo.maxSets = static_cast<uint32_t>(MAX_BINDLESS_RESOURCES * globalPoolSizes.size());
		if (vkCreateDescriptorPool(m_pVkDataPtr->logicalDevice, &globalPoolInfo, nullptr, &m_pGlobalDescriptorPool) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create global descriptor pool!");
		}
	}
	void VulkanResourceManager::pCreateGlobalDescriptorSets(VkDescriptorSetLayout layout, VkDescriptorPool &pool, VkDescriptorSet &descriptorSets, uint32_t count)
	{
		std::vector<VkDescriptorSetLayout> layouts(count, layout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = pool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = layouts.data();

		VkDescriptorSetVariableDescriptorCountAllocateInfo count_info{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO};
		uint32_t max_binding = MAX_BINDLESS_RESOURCES - 1;
		count_info.descriptorSetCount = 1;
		count_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO;
		// This number is the max allocatable count
		count_info.pDescriptorCounts = &max_binding;
		allocInfo.pNext = &count_info;

		if (vkAllocateDescriptorSets(m_pVkDataPtr->logicalDevice, &allocInfo, &descriptorSets) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate descriptor sets!");
		}
	}

	void VulkanResourceManager::pCreateDescriptorSets(/*vk::VulkanMaterial &material*/ VkDescriptorSetLayout layout, VkDescriptorPool &pool, std::vector<VkDescriptorSet> &descriptorSets, uint32_t count)
	{
		std::vector<VkDescriptorSetLayout> layouts(count, layout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_pDescriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(count);
		allocInfo.pSetLayouts = layouts.data();

		descriptorSets.resize(count);
		if (vkAllocateDescriptorSets(m_pVkDataPtr->logicalDevice, &allocInfo, descriptorSets.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate descriptor sets!");
		}
	}

	void VulkanResourceManager::pRecreateDescriptorSets()
	{
		for (auto &material : materialPool)
		{
			pCreateDescriptorSets(material.descriptorSetLayout, m_pDescriptorPool, material.descriptorSets, vk::MAX_FRAMES_IN_FLIGHT);
			pUpdateMaterial(material);
		}
	}

	vk::VulkanBuffer VulkanResourceManager::pCreateStagingBuffer(const std::vector<common::Vertex> &vertices)
	{
		vk::VulkanBuffer stagingBuffer;
		pCreateBuffer(stagingBuffer, vertices.size() * sizeof(common::Vertex), STAGING_BUFFER_USAGE, STAGING_BUFFER_PROP, STAGING_BUFFER_MEM_USAGE);

		void *data;
		vmaMapMemory(m_pAllocator, stagingBuffer.allocation, &data);
		memcpy(data, vertices.data(), vertices.size() * sizeof(common::Vertex));
		vmaUnmapMemory(m_pAllocator, stagingBuffer.allocation);
		if (0)
			IO::Info("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "allocating ", vertices.size() * sizeof(common::Vertex), " bytes in hosted staging buffer");
		return stagingBuffer;
	}

	vk::VulkanBuffer VulkanResourceManager::pCreateStagingBuffer(const std::vector<common::AnimatedVertex> &vertices)
	{
		vk::VulkanBuffer stagingBuffer;
		pCreateBuffer(stagingBuffer, vertices.size() * sizeof(common::AnimatedVertex), STAGING_BUFFER_USAGE, STAGING_BUFFER_PROP, STAGING_BUFFER_MEM_USAGE);

		void *data;
		vmaMapMemory(m_pAllocator, stagingBuffer.allocation, &data);
		memcpy(data, vertices.data(), vertices.size() * sizeof(common::AnimatedVertex));
		vmaUnmapMemory(m_pAllocator, stagingBuffer.allocation);
		if (0)
			IO::Info("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "allocating ", vertices.size() * sizeof(common::AnimatedVertex), " bytes in hosted staging buffer");
		return stagingBuffer;
	}

	vk::VulkanBuffer VulkanResourceManager::pCreateStagingIndexBuffer(const std::vector<IndexType> &indices)
	{
		vk::VulkanBuffer stagingBuffer;
		pCreateBuffer(stagingBuffer, indices.size() * sizeof(IndexType), STAGING_BUFFER_USAGE, STAGING_BUFFER_PROP, STAGING_BUFFER_MEM_USAGE);

		// void* data;
		void *data;
		vmaMapMemory(m_pAllocator, stagingBuffer.allocation, &data);
		memcpy(data, indices.data(), indices.size() * sizeof(IndexType));
		vmaUnmapMemory(m_pAllocator, stagingBuffer.allocation);
		if (0)
			IO::Info("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "allocating ", indices.size() * sizeof(IndexType), " bytes in hosted staging buffer");
		return stagingBuffer;
	}

	void VulkanResourceManager::pUpdateMaterial(vk::VulkanMaterial &material)
	{
		for (int i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++)
		{

			uint32_t numSlots = material.slots + 1;
			std::vector<VkWriteDescriptorSet> descriptorWrites{};
			std::list<VkDescriptorImageInfo> imageInfos;
			std::list<VkDescriptorBufferInfo> bufferInfos;
			// numSlots += material.shaderBindings.size();
			descriptorWrites.resize(numSlots);

			int j = 0;
			for (int index = 1; index < numSlots; index++)
			{
				auto &binding = material.shaderBindings[j];

				if (binding.descriptorBinding == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER && binding.isRenderPassAttachment)
				{
					auto &imageInfo = imageInfos.emplace_back();

					imageInfo.imageLayout = binding.texture.isDepthAttachment ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					imageInfo.imageLayout = binding.texture.format == VK_FORMAT_D32_SFLOAT ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL : imageInfo.imageLayout;

					binding.texture.imageLayout = imageInfo.imageLayout;
					if (!binding.renderpass.empty())
					{
						auto pass = renderPassPool.get(std::hash<std::string>{}(binding.renderpass));

						if (binding.attachment_index == pass->renderPassChain.Textures.size())
						{
							if (pass->id == std::numeric_limits<uint32_t>::max())
							{
								binding.texture = m_pVkDataPtr->defaultRenderPass.renderPassChain.DepthTexture;
							}
							else
							{
								binding.texture = pass->renderPassChain.DepthTexture;
							}
						}
						else
						{
							binding.texture = pass->renderPassChain.Textures.at(binding.attachment_index);
						}
					}

					imageInfo.imageView = binding.texture.imageView;
					imageInfo.sampler = binding.texture.sampler;

					descriptorWrites[index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWrites[index].dstSet = material.descriptorSets[i];
					descriptorWrites[index].dstBinding = binding.bindingPoint;
					descriptorWrites[index].dstArrayElement = 0;
					descriptorWrites[index].descriptorType = binding.descriptorBinding;
					descriptorWrites[index].descriptorCount = 1;
					descriptorWrites[index].pImageInfo = &imageInfo;
				}
				else if (binding.descriptorBinding == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
				{
					int l_index = i;
					if (binding.buffers.size() - 1 < i)
						l_index = binding.buffers.size() - 1;
					VkDescriptorBufferInfo &bufferInfo = bufferInfos.emplace_back();
					bufferInfo.buffer = binding.buffers[l_index].buffer; // m_pUniformBuffers[i].buffer;
					bufferInfo.offset = binding.buffers[l_index].offset;
					bufferInfo.range = binding.buffers[l_index].size;
					descriptorWrites[index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWrites[index].dstSet = material.descriptorSets[i];
					descriptorWrites[index].dstBinding = binding.bindingPoint;
					descriptorWrites[index].dstArrayElement = 0;
					descriptorWrites[index].descriptorType = binding.descriptorBinding;
					descriptorWrites[index].descriptorCount = 1;
					descriptorWrites[index].pBufferInfo = &bufferInfo;
					descriptorWrites[index].pImageInfo = nullptr;		// Optional
					descriptorWrites[index].pTexelBufferView = nullptr; // Optional
				}
				else if (binding.descriptorBinding == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
				{
					int l_index = i;
					if (binding.buffers.size() - 1 < i)
						l_index = binding.buffers.size() - 1;
					VkDescriptorBufferInfo &bufferInfo = bufferInfos.emplace_back();
					bufferInfo.buffer = binding.buffers[l_index].buffer; // m_pUniformBuffers[i].buffer;
					bufferInfo.offset = binding.buffers[l_index].offset;
					bufferInfo.range = binding.buffers[l_index].size;
					descriptorWrites[index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWrites[index].dstSet = material.descriptorSets[i];
					descriptorWrites[index].dstBinding = binding.bindingPoint;
					descriptorWrites[index].dstArrayElement = 0;
					descriptorWrites[index].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
					descriptorWrites[index].descriptorCount = 1;
					descriptorWrites[index].pBufferInfo = &bufferInfo;
					descriptorWrites[index].pImageInfo = nullptr;		// Optional
					descriptorWrites[index].pTexelBufferView = nullptr; // Optional
				}
				else if (binding.descriptorBinding == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE)
				{

					auto& imageInfo = imageInfos.emplace_back();

					imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;//binding.texture.isDepthAttachment ? VK_IMAGE_LAYOUT_GENERAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					//imageInfo.imageLayout = binding.texture.format == VK_FORMAT_D32_SFLOAT ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL : imageInfo.imageLayout;

					binding.texture.imageLayout = imageInfo.imageLayout;
					if (!binding.renderpass.empty())
					{
						auto pass = renderPassPool.get(std::hash<std::string>{}(binding.renderpass));

						if (binding.attachment_index == pass->renderPassChain.Textures.size())
						{
							if (pass->id == std::numeric_limits<uint32_t>::max())
							{
								binding.texture = m_pVkDataPtr->defaultRenderPass.renderPassChain.DepthTexture;
							}
							else
							{
								binding.texture = pass->renderPassChain.DepthTexture;
							}
						}
						else
						{
							binding.texture = pass->renderPassChain.Textures.at(binding.attachment_index);
						}
					}

					imageInfo.imageView = binding.texture.imageView;
					imageInfo.sampler = binding.texture.sampler;

					descriptorWrites[index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWrites[index].dstSet = material.descriptorSets[i];
					descriptorWrites[index].dstBinding = binding.bindingPoint;
					descriptorWrites[index].dstArrayElement = 0;
					descriptorWrites[index].descriptorType = binding.descriptorBinding;
					descriptorWrites[index].descriptorCount = 1;
					descriptorWrites[index].pImageInfo = &imageInfo;
				}

				j++;
			}

			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = material.bufferInfo[i].buffer; // m_pUniformBuffers[i].buffer;
			bufferInfo.offset = material.bufferOffset;
			bufferInfo.range = material.bufferSize;
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = material.descriptorSets[i];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;
			descriptorWrites[0].pImageInfo = nullptr;		// Optional
			descriptorWrites[0].pTexelBufferView = nullptr; // Optional

			vkUpdateDescriptorSets(m_pVkDataPtr->logicalDevice, numSlots, descriptorWrites.data(), 0, nullptr);
		}
	}

	Ref<PushConstant> VulkanResourceManager::createPushConstant(const std::string &name, PushConstantData push_constant)
	{
		return pushConstantPool.insert(name, push_constant);
	}

	void VulkanResourceManager::pRecreateFrameBuffers(VkExtent2D extent)
	{
		for (auto &renderpass_ref : resizableRenderPasses)
		{
			auto renderPass = renderPassPool.get(renderpass_ref);

			// destroyRenderPass(renderpass_ref);

			for (auto &framebuffer : renderPass->renderPassChain.Framebuffers)
				vkDestroyFramebuffer(m_pVkDataPtr->logicalDevice, framebuffer, nullptr);

			for (int i = 0; i < renderPass->renderPassChain.Textures.size(); i++)
			{
				vkDestroySampler(m_pVkDataPtr->logicalDevice, renderPass->renderPassChain.Textures[i].sampler, nullptr);
				vkDestroyImageView(m_pVkDataPtr->logicalDevice, renderPass->renderPassChain.Textures[i].imageView, nullptr);
				vmaDestroyImage(m_pAllocator, renderPass->renderPassChain.Textures[i].image,
								renderPass->renderPassChain.Textures[i].allocation);
				renderPass->renderPassChain.Textures[i].sampler = VK_NULL_HANDLE;
				renderPass->renderPassChain.Textures[i].imageView = VK_NULL_HANDLE;
				renderPass->renderPassChain.Textures[i].image = VK_NULL_HANDLE;
			}
			{ /*
				if (renderPass->renderPassChain.DepthTexture.sampler != VK_NULL_HANDLE)
					vkDestroySampler(m_pVkDataPtr->logicalDevice, renderPass->renderPassChain.DepthTexture.sampler, nullptr);

				vkDestroyImageView(m_pVkDataPtr->logicalDevice, renderPass->renderPassChain.DepthTexture.imageView, nullptr);
				vmaDestroyImage(m_pAllocator, renderPass->renderPassChain.DepthTexture.image,
					renderPass->renderPassChain.DepthTexture.allocation);
*/

				if (renderPass->renderPassChain.DepthTexture.sampler != VK_NULL_HANDLE && renderPass->renderPassChain.DepthTexture.index < 0)
					vkDestroySampler(m_pVkDataPtr->logicalDevice, renderPass->renderPassChain.DepthTexture.sampler, nullptr);

				if (renderPass->renderPassChain.DepthTexture.imageView != VK_NULL_HANDLE && renderPass->renderPassChain.DepthTexture.index < 0)
					vkDestroyImageView(m_pVkDataPtr->logicalDevice, renderPass->renderPassChain.DepthTexture.imageView, nullptr);

				if (renderPass->renderPassChain.DepthTexture.image != VK_NULL_HANDLE && renderPass->renderPassChain.DepthTexture.index < 0)
					vmaDestroyImage(m_pAllocator, renderPass->renderPassChain.DepthTexture.image, renderPass->renderPassChain.DepthTexture.allocation);
			}

			auto &renderPassInfo = m_pRenderPassInfo[renderPass->id];
			renderPassInfo.dimensions.width = extent.width;
			renderPassInfo.dimensions.height = extent.height;

			renderPass->renderPassChain.Extent = extent;

			for (size_t i = 0; i < renderPassInfo.attachments.size(); i++)
			{
				auto &attachment = renderPassInfo.attachments[i];

				vk::VulkanTextureInfo texInfo;
				texInfo.format = attachment.isDepthAttachment ? findDepthFormat(*m_pVkDataPtr) : resolveFormat(renderPassInfo.attachments[i].format);

				texInfo.addressMode = resolveWrapping(attachment.wrapMode);
				texInfo.compareOp = resolveCompareOp(attachment.compareFunc);
				texInfo.filter = resolveFilter(attachment.filtering);
				texInfo.width = renderPassInfo.dimensions.width;
				texInfo.height = renderPassInfo.dimensions.height;
				texInfo.num_channels = attachment.isDepthAttachment ? 1 : resolveNumChannels(attachment.format);
				texInfo.usage = (attachment.isDepthAttachment || texInfo.format == VK_FORMAT_D32_SFLOAT) ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT : VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

				texInfo.compareEnable = attachment.depthCompare;

				vk::VulkanTexture texture = pCreateTextureBuffer(texInfo);

				texture.bindingType = vk::RENDER_BUFFER_SAMPLER;
				texture.info.mipLevels = 1;
				createImageView(*m_pVkDataPtr, texture,
								(attachment.isDepthAttachment || texInfo.format == VK_FORMAT_D32_SFLOAT) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT);

				texture.imageLayout = attachment.isDepthAttachment ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				if (attachment.isSampler)
				{
					vk::createTextureSampler(*m_pVkDataPtr, texture);
				}

				if (attachment.isDepthAttachment)
				{
					texture.isDepthAttachment = true;
					renderPass->renderPassChain.DepthTexture = texture;

					if (attachment.isSampler)
					{
						renderPass->renderPassChain.hasDepthSampler = false;
						renderPass->renderPassChain.ImageViews.at(i) = texture.imageView;
						renderPass->renderPassChain.Textures.at(i) = texture;
						renderPass->renderPassChain.DepthTexture.index = i;
					}
				}
				else
				{
					renderPass->renderPassChain.ImageViews.at(i) = texture.imageView;
					renderPass->renderPassChain.Textures.at(i) = texture;
				}
			}

			vk::createFramebuffers(*m_pVkDataPtr, *renderPass, renderPass->renderPassChain);
			// createRenderPass(m_pRenderPassInfo[renderPass->id]);

			VkViewport viewport = {
				.x = 0,
				.y = 0,
				.width = (float)renderPass->renderPassChain.Extent.width,
				.height = (float)renderPass->renderPassChain.Extent.height,
				.minDepth = 0.0f,
				.maxDepth = 1.0f};

			VkRect2D rect;
			rect.extent.width = renderPass->renderPassChain.Extent.width;
			rect.extent.height = renderPass->renderPassChain.Extent.height;

			rect.offset.x = 0;
			rect.offset.y = 0;

			renderPass->renderPassChain.setViewport(viewport);
			renderPass->renderPassChain.setScissor(rect);
		}
	}

	void VulkanResourceManager::pGenerateMipMaps(TextureCreationInfo texInfo, vk::VulkanTexture &texture)
	{

		/* Begin generate mips*/
		// Copy down mips from n-1 to n

		VkCommandBuffer blitCommandBuffer = vk::beginSingleTimeCommands(*m_pVkDataPtr, m_pTransferCommandPool);
		if (!texInfo.isCompressed && texInfo.mipLevels > 1)
		{
			for (int32_t i = 1; i < texInfo.mipLevels; i++)
			{
				VkImageBlit imageBlit{};

				// Source
				imageBlit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				imageBlit.srcSubresource.layerCount = 1;
				imageBlit.srcSubresource.mipLevel = i - 1;
				imageBlit.srcOffsets[1].x = int32_t(texInfo.width >> (i - 1));
				imageBlit.srcOffsets[1].y = int32_t(texInfo.height >> (i - 1));
				imageBlit.srcOffsets[1].z = 1;

				// Destination
				imageBlit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				imageBlit.dstSubresource.layerCount = 1;
				imageBlit.dstSubresource.mipLevel = i;
				imageBlit.dstOffsets[1].x = int32_t(texInfo.width >> i);
				imageBlit.dstOffsets[1].y = int32_t(texInfo.height >> i);
				imageBlit.dstOffsets[1].z = 1;

				VkImageSubresourceRange mipSubRange = {};
				mipSubRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				mipSubRange.baseMipLevel = i;
				mipSubRange.levelCount = 1;
				mipSubRange.layerCount = 1;

				vk::imageMemoryBarrier(*m_pVkDataPtr,
									   m_pTransferCommandPool,
									   texture.image,
									   texture.format,
									   VK_IMAGE_LAYOUT_UNDEFINED,
									   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
									   texture.info,
									   blitCommandBuffer,
									   mipSubRange, i, 1);

				// Blit from previous level
				if (!texInfo.isCompressed)
					vkCmdBlitImage(
						blitCommandBuffer,
						texture.image,
						VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
						texture.image,
						VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
						1,
						&imageBlit,
						VK_FILTER_LINEAR);

				vk::imageMemoryBarrier(*m_pVkDataPtr,
									   m_pTransferCommandPool,
									   texture.image,
									   texture.format,
									   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
									   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
									   texture.info,
									   blitCommandBuffer,
									   mipSubRange, i, 1);
			}
		}
		else {
		}

		// After the loop, all mip layers are in TRANSFER_SRC layout, so transition all to SHADER_READ

		VkImageSubresourceRange subresourceRange = {};
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.layerCount = 1;
		subresourceRange.levelCount = texInfo.mipLevels;

		vk::imageMemoryBarrier(*m_pVkDataPtr,
							   m_pTransferCommandPool,
							   texture.image,
							   texture.format,
							   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
							   VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
							   texture.info,
							   blitCommandBuffer,
							   subresourceRange,
							   0, texInfo.mipLevels);

		vk::endSingleTimeCommands(*m_pVkDataPtr, m_pTransferCommandPool, blitCommandBuffer);

		/*transitionImageLayout(*m_pVkDataPtr, m_pVkDataPtr->m_pCommandPools.back(), texture.image,
			format, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, texture.info);*/
		/* End generate mips*/
	}

}