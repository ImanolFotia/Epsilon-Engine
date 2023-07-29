#undef VMA_DEBUG_LOG
#undef VMA_DEBUG_LOG_FORMAT

#include "resource_manager.hpp"
#include "helpers.hpp"

#include "core/framework/exception.hpp"

#define VMA_DEBUG_LOG_FORMAT(format, ...)

#define VMA_DEBUG_LOG_FORMAT(format, ...) \
	do                                    \
	{                                     \
		printf((format), __VA_ARGS__);    \
		printf("\n");                     \
	} while (false)

#define VMA_DEBUG_LOG(str) VMA_DEBUG_LOG_FORMAT("%s", (str))

#ifdef WIN32
#if !defined(__PRETTY_FUNCTION__) && !defined(__GNUC__)
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

#endif

namespace engine
{
	VulkanResourceManager::VulkanResourceManager()
	{
	}

	VulkanResourceManager::~VulkanResourceManager()
	{
	}

	void VulkanResourceManager::Init()
	{

		std::cout << "Initiating Resource Manager\n";
		VmaAllocatorCreateInfo allocatorInfo = {};
		allocatorInfo.physicalDevice = m_pVkDataPtr->physicalDevice;
		allocatorInfo.device = m_pVkDataPtr->logicalDevice;
		allocatorInfo.instance = m_pVkDataPtr->instance;
#if !defined(__ANDROID__)
		allocatorInfo.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
#endif

		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(m_pVkDataPtr->physicalDevice, &deviceProperties);
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(m_pVkDataPtr->physicalDevice, &deviceFeatures);

		ResourcesMemory.GPUName = std::string(deviceProperties.deviceName);
		/*IO::Log("Selected device: ", deviceProperties.deviceName);
		IO::Log("\tApi version: ", deviceProperties.apiVersion);
		IO::Log("\tID: ", deviceProperties.deviceID);
		IO::Log("\tDriver version: ", deviceProperties.driverVersion);
		IO::Log("\tVendor ID: ", deviceProperties.vendorID);
		IO::Log("\tIs discrete device: ", (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? "yes" : "no"));*/

		vmaCreateAllocator(&allocatorInfo, &m_pAllocator);

		m_pCommandPools.emplace_back();

		vk::createCommandPool(*m_pVkDataPtr, m_pCommandPools.back());
		vk::createTransferCommandPool(*m_pVkDataPtr, m_pTransferCommandPool);

		pCreateGlobalDescriptorPool();
		pCreateDescriptorPool();

		vk::createDescriptorSetLayout(
			*m_pVkDataPtr,
			m_pGlobalDescriptorSetLayout,
			{ {0, 0, 0, 1, UniformBindingType::TEXTURE_IMAGE_COMBINED_SAMPLER, {}, {}, "", true, MAX_BINDLESS_RESOURCES, "GlobalBindlessTextures"} });

		pCreateGlobalDescriptorSets(m_pGlobalDescriptorSetLayout,
			m_pGlobalDescriptorPool,
			m_pGlobalDescriptorSets,
			MAX_BINDLESS_RESOURCES);

		pCreateBuffer(m_pIndirectBuffer,
			sizeof(VkDrawIndexedIndirectCommand) * 100000,
			VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT,
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
			VMA_MEMORY_USAGE_AUTO);
	}

	Ref<Texture> VulkanResourceManager::createTexture(TextureCreationInfo texInfo)
	{
		Ref<Texture> refTexture;

		auto format = resolveFormat(texInfo.format, texInfo.isCompressed);
		uint32_t mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texInfo.width, texInfo.height)))) + 1;

		auto size = texInfo.width * texInfo.height * texInfo.numChannels;
		if (texInfo.isKTX) {
			size = texInfo.size;
			mipLevels = texInfo.mipLevels;
		}
		else {
			texInfo.mipLevels = mipLevels;
		}

		auto stagingBuffer = pCreateStagingTextureBuffer(texInfo.pixels, texInfo);
		

		auto texture = pCreateTextureBuffer({ .width = texInfo.width,
											 .height = texInfo.height,
											 .num_channels = texInfo.numChannels,
											 .mipLevels = mipLevels,
											 .format = format,
											 .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT |
													  VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
													  VK_IMAGE_USAGE_SAMPLED_BIT });

		texture.format = format;

		transitionImageLayout(*m_pVkDataPtr, m_pTransferCommandPool, texture.image,
			format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			texture.info);
		copyBufferToImage(*m_pVkDataPtr, m_pTransferCommandPool, stagingBuffer.buffer, texture.image,
			static_cast<uint32_t>(texInfo.width), static_cast<uint32_t>(texInfo.height), texInfo.offsets, texInfo.mipLevels);
		transitionImageLayout(*m_pVkDataPtr, m_pTransferCommandPool, texture.image,
			format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, texture.info);

		vmaDestroyBuffer(m_pAllocator, stagingBuffer.buffer, stagingBuffer.allocation);
		// vmaFreeMemory(m_pAllocator, m_pStagingTextureBuffer.allocation);

		texture.filter = resolveFilter(texInfo.filtering);

		texture.info.mipLevels = mipLevels;
		vk::createImageView(*m_pVkDataPtr, texture, VK_IMAGE_ASPECT_COLOR_BIT);

		ResourcesMemory.m_pTextureBufferAllocationSize += size;

		//Call gen mips
		if (texInfo.offsets.size() <= 0) {
			pGenerateMipMaps(texInfo, texture);
		}


		vk::createTextureSampler(*m_pVkDataPtr, texture);

		texture.bindingType = vk::MATERIAL_SAMPLER;

		auto ref = texPool.insert(texInfo.name, texture);
		texture.index = ref.m_pIndex;
		texPool.get(ref)->index = ref.m_pIndex;

		if (m_pVkDataPtr->bindless_supported)
		{
			int count = 1;
			VkWriteDescriptorSet bindless_descriptor_writes;
			VkDescriptorImageInfo bindless_image_info;

			bindless_descriptor_writes = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
			bindless_descriptor_writes.descriptorCount = 1;
			bindless_descriptor_writes.dstArrayElement = texture.index;
			bindless_descriptor_writes.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			bindless_descriptor_writes.dstSet = m_pGlobalDescriptorSets;
			bindless_descriptor_writes.dstBinding = 0;

			bindless_image_info.sampler = texture.sampler;
			bindless_image_info.imageView = texture.imageView;
			bindless_image_info.imageLayout = texture.isDepthAttachment ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			bindless_descriptor_writes.pImageInfo = &bindless_image_info;

			if (count > 0)
				vkUpdateDescriptorSets(m_pVkDataPtr->logicalDevice, count, &bindless_descriptor_writes, 0, nullptr);
		}

		return ref;
	}

	Ref<BindGroup> VulkanResourceManager::createBindGroup(BindGroupInfo material)
	{
		try
		{
			vk::VulkanMaterial vkMaterial;
			// auto renderPass = renderPassPool.get(renderPassRef);
			uint32_t id = std::hash<std::string>{}(material.renderPass);
			auto renderPass = renderPassPool.get(id);
			vkMaterial.descriptorSetLayout = renderPass->renderPipelines.back().descriptorSetLayouts.at(RENDERPASS_LAYOUT);
			// auto &materialdata = m_pMaterials.emplace_back();

			for (auto& binding : material.inputs)
			{
				auto pass = renderPassPool.get(std::hash<std::string>{}(binding.renderPass));
				vkMaterial.slots++;
				if (binding.index == pass->renderPassChain.Textures.size())
				{
					vk::VulkanShaderBinding shaderBinding = {
						.texture = pass->renderPassChain.DepthTexture,
						.descriptorBinding = VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
						.bindingPoint = binding.bindingPoint,
						.isRenderPassAttachment = true,
						.renderpass = binding.renderPass,
					.attachment_index = (int32_t)binding.index };

					shaderBinding.texture.isDepthAttachment = true;

					vkMaterial.shaderBindings.push_back(shaderBinding);
				}
				else
				{
					vk::VulkanShaderBinding shaderBinding = {
						.texture = pass->renderPassChain.Textures.at(binding.index),
						.descriptorBinding = VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
						.bindingPoint = binding.bindingPoint,
						.isRenderPassAttachment = true,
						.renderpass = binding.renderPass,
					.attachment_index = (int32_t)binding.index };

					vkMaterial.shaderBindings.push_back(shaderBinding);
				}
			}

			for (auto& binding : material.bindingInfo)
			{
				if (binding.type == UniformBindingType::TEXTURE_IMAGE_COMBINED_SAMPLER)
				{
					vkMaterial.slots++;
					auto tex = createTexture(binding.textureInfo);
					vk::VulkanShaderBinding shaderBinding = {
						.texture = *texPool.get(tex),
						.descriptorBinding = VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
						.bindingPoint = binding.binding,
						.isRenderPassAttachment = false };

					vkMaterial.shaderBindings.push_back(shaderBinding);
				}
				else if (binding.type == UniformBindingType::SHADER_STORAGE)
				{
					vkMaterial.slots++;
					auto buff = gpuBufferPool.get(std::hash<std::string>{}(binding.buffer))->buffers;
					for (auto& b : buff) b.size = binding.size;

					vk::VulkanShaderBinding shaderBinding = {
						.buffers = buff,
						.descriptorBinding = VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
						.bindingPoint = binding.binding,
						.isRenderPassAttachment = false };

					vkMaterial.shaderBindings.push_back(shaderBinding);
				}
			}

			auto buffers = renderPass->uniformBuffer.front().buffers;

			for (int i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++)
			{
				vkMaterial.bufferInfo[i].offset = buffers[i].offset;
				vkMaterial.bufferInfo[i].buffer = buffers[i].buffer;
			}

			vkMaterial.bufferSize = renderPass->uniformBuffer.front().size;

			m_pNumCommandPools++;

			//pCreateDescriptorPool();
			//pRecreateDescriptorSets();
			pCreateDescriptorSets(vkMaterial.descriptorSetLayout,
				m_pDescriptorPool,
				vkMaterial.descriptorSets,
				vk::MAX_FRAMES_IN_FLIGHT);
			pUpdateMaterial(vkMaterial);


			Ref<BindGroup> materialRef = materialPool.insert(material.name, vkMaterial);
			return materialRef;
		}
		catch (std::exception& e)
		{

			throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
		}
	}

	void VulkanResourceManager::pRecreateSwapChain(framework::Window::windowType* window)
	{
	}

	Ref<RenderPass> VulkanResourceManager::createDefaultRenderPass(RenderPassInfo renderPassInfo)
	{
		m_pVkDataPtr->defaultRenderPass.id = std::numeric_limits<uint32_t>::max();
		m_pDefaultRenderPassInfo = renderPassInfo;

		m_pVkDataPtr->defaultRenderPass.clearValues.resize(renderPassInfo.numAttachments + (renderPassInfo.depthAttachment ? 1 : 0));
		for (int i = 0; i < renderPassInfo.numAttachments; i++)
		{
			m_pVkDataPtr->defaultRenderPass.clearValues[i].color.float32[0] = renderPassInfo.attachments[i].clearColor[0];
			m_pVkDataPtr->defaultRenderPass.clearValues[i].color.float32[1] = renderPassInfo.attachments[i].clearColor[1];
			m_pVkDataPtr->defaultRenderPass.clearValues[i].color.float32[2] = renderPassInfo.attachments[i].clearColor[2];
			m_pVkDataPtr->defaultRenderPass.clearValues[i].color.float32[3] = renderPassInfo.attachments[i].clearColor[3];
		}
		if (renderPassInfo.depthAttachment)
		{
			ResourcesMemory.m_pTextureBufferAllocationSize += renderPassInfo.dimensions.width * renderPassInfo.dimensions.height * 4;
			m_pVkDataPtr->defaultRenderPass.clearValues.back()
				.depthStencil = {
				(float)renderPassInfo.attachments.back().depthStencilValue[0],
				renderPassInfo.attachments.back().depthStencilValue[1] };


		}

		vk::createRenderPass(*m_pVkDataPtr, m_pVkDataPtr->defaultRenderPass, renderPassInfo, true);

		// for (int i = 0; i < renderPassInfo.numLayouts; i++)
		{

			m_pVkDataPtr->defaultRenderPass.renderPipelines.resize(renderPassInfo.numLayouts);
			// m_pVkDataPtr->defaultRenderPass.renderPipelines.emplace_back();
			m_pVkDataPtr->defaultRenderPass.renderPipelines.back().descriptorSetLayouts.resize(2);
			vk::createDescriptorSetLayout(*m_pVkDataPtr,
				m_pVkDataPtr->defaultRenderPass.renderPipelines.back().descriptorSetLayouts.at(RENDERPASS_LAYOUT), renderPassInfo.bindingInfo);

			m_pVkDataPtr->defaultRenderPass.numAttachments = renderPassInfo.numAttachments;

			for (auto& pipeline : m_pVkDataPtr->defaultRenderPass.renderPipelines)
			{
				pipeline.descriptorSetLayouts.resize(1);
				if (m_pVkDataPtr->bindless_supported)
				{
					pipeline.descriptorSetLayouts.resize(2);
					pipeline.descriptorSetLayouts.at(GLOBAL_LAYOUT) = m_pGlobalDescriptorSetLayout;
				}
			}
			vk::createGraphicsPipeline(*m_pVkDataPtr,
				m_pVkDataPtr->defaultRenderPass,
				renderPassInfo);
		}

		vk::createSwapChainFramebuffers(*m_pVkDataPtr, m_pVkDataPtr->defaultRenderPass,
			m_pVkDataPtr->defaultRenderPass.renderPassChain);

		m_pVkDataPtr->defaultRenderPass.name = renderPassInfo.name;

		uint32_t id = std::hash<std::string>{}(renderPassInfo.name);
		auto pass = renderPassPool.get(id);

		VkViewport viewport = {
			.x = 0,
			.y = 0,
			.width = (float)m_pVkDataPtr->defaultRenderPass.renderPassChain.Extent.width,
			.height = (float)m_pVkDataPtr->defaultRenderPass.renderPassChain.Extent.height,
			.minDepth = 0.0f,
			.maxDepth = 1.0f
		};

		VkRect2D rect;
		rect.extent.width = m_pVkDataPtr->defaultRenderPass.renderPassChain.Extent.width;
		rect.extent.height = m_pVkDataPtr->defaultRenderPass.renderPassChain.Extent.height;

		rect.offset.x = 0;
		rect.offset.y = 0;

		m_pVkDataPtr->defaultRenderPass.renderPassChain.setViewport(viewport);
		m_pVkDataPtr->defaultRenderPass.renderPassChain.setScissor(rect);
		

		for (auto& binding : renderPassInfo.bindingInfo)
		{
			if (binding.type == UniformBindingType::UNIFORM_BUFFER)
			{
				auto uniformRef = createUniformData(binding);
				m_pVkDataPtr->defaultRenderPass.uniformBuffer.push_back(*uniformBufferPool.get(uniformRef));
			}
		}

		m_pDefaultRenderPassRef = renderPassPool.insert(renderPassInfo.name, m_pVkDataPtr->defaultRenderPass);

		return m_pDefaultRenderPassRef;
	}

	Ref<RenderPass> VulkanResourceManager::createRenderPass(RenderPassInfo renderPassInfo)
	{
		vk::VulkanRenderPass renderPass = {};

		renderPass.clearValues.resize(renderPassInfo.numAttachments + (renderPassInfo.depthAttachment ? 1 : 0));
		for (int i = 0; i < renderPassInfo.numAttachments; i++)
		{
			renderPass.clearValues[i].color = { renderPassInfo.attachments[i].clearColor[0],
											   renderPassInfo.attachments[i].clearColor[1],
											   renderPassInfo.attachments[i].clearColor[2],
											   renderPassInfo.attachments[i].clearColor[3] };
		}
		if (renderPassInfo.depthAttachment)
			renderPass.clearValues.back().depthStencil = {
				(float)renderPassInfo.attachments.back().depthStencilValue[0],
				renderPassInfo.attachments.back().depthStencilValue[1] };

		renderPass.id = m_pRenderPassCount;
		m_pRenderPassInfo[renderPass.id] = renderPassInfo;
		renderPass.renderPassChain.ImageFormats.resize(renderPassInfo.attachments.size());

		for (int index = 0; index < renderPassInfo.attachments.size(); index++)
			renderPass.renderPassChain.ImageFormats[index] = resolveFormat(renderPassInfo.attachments[index].format);

		vk::createRenderPass(*m_pVkDataPtr, renderPass, renderPassInfo, false);

		for (size_t i = 0; i < renderPassInfo.attachments.size(); i++)
		{
			auto& attachment = renderPassInfo.attachments[i];

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

			if (attachment.isSampler)
			{
				vk::createTextureSampler(*m_pVkDataPtr, texture);
			}

			if (attachment.isDepthAttachment)
			{
				texture.isDepthAttachment = true;
				renderPass.renderPassChain.DepthTexture = texture;
			}
			else
			{
				renderPass.renderPassChain.ImageViews.push_back(texture.imageView);
				renderPass.renderPassChain.Textures.push_back(texture);
			}
		}

		renderPass.renderPassChain.Extent.width = renderPassInfo.dimensions.width;
		renderPass.renderPassChain.Extent.height = renderPassInfo.dimensions.height;
		renderPass.name = renderPassInfo.name;

		renderPass.renderPipelines.resize(renderPassInfo.numLayouts); //.emplace_back();

		for (auto& pipeline : renderPass.renderPipelines)
		{
			pipeline.descriptorSetLayouts.resize(1);
			if (m_pVkDataPtr->bindless_supported)
			{
				pipeline.descriptorSetLayouts.resize(2);
				pipeline.descriptorSetLayouts.at(GLOBAL_LAYOUT) = m_pGlobalDescriptorSetLayout;
			}
		}

		vk::createGraphicsPipeline(*m_pVkDataPtr,
			renderPass,
			renderPassInfo);

		vk::createFramebuffers(*m_pVkDataPtr, renderPass, renderPass.renderPassChain);

		VkViewport viewport = {
			.x = 0,
			.y = 0,
			.width = (float)renderPass.renderPassChain.Extent.width,
			.height = (float)renderPass.renderPassChain.Extent.height,
			.minDepth = 0.0f,
			.maxDepth = 1.0f
		};

		VkRect2D rect;
		rect.extent.width = renderPass.renderPassChain.Extent.width;
		rect.extent.height = renderPass.renderPassChain.Extent.height;

		rect.offset.x = 0;
		rect.offset.y = 0;

		renderPass.renderPassChain.setViewport(viewport);
		renderPass.renderPassChain.setScissor(rect);


		for (auto& binding : renderPassInfo.bindingInfo)
		{
			if (binding.type == UniformBindingType::UNIFORM_BUFFER)
			{
				auto uniformRef = createUniformData(binding);
				renderPass.uniformBuffer.push_back(*uniformBufferPool.get(uniformRef));
			}
			else if (binding.type == UniformBindingType::SHADER_STORAGE)
			{
			}
		}
		auto ref = renderPassPool.insert(renderPassInfo.name, renderPass);

		if (renderPassInfo.resizeWithSwapChain) {
			resizableRenderPasses.push_back(ref);
		}

		m_pRenderPassCount++;
		return ref;
	}

	void VulkanResourceManager::clean()
	{

		vkDeviceWaitIdle(m_pVkDataPtr->logicalDevice);
		for (auto& buffer : vertexBufferPool)
		{
			vmaDestroyBuffer(m_pAllocator, buffer.buffer, buffer.allocation);
		}

		for (auto& buffer : indexBufferPool)
		{
			vmaDestroyBuffer(m_pAllocator, buffer.buffer, buffer.allocation);
		}

		for (auto& buffer : uniformBufferPool)
		{
			for (auto& b : buffer.buffers)
			{
				vmaDestroyBuffer(m_pAllocator, b.buffer, b.allocation);
			}
		}
		for (auto& buffer : gpuBufferPool)
		{
			for (auto& b : buffer.buffers) {
				vmaDestroyBuffer(m_pAllocator,b.buffer, b.allocation);
			}
		}

		vk::cleanupSyncObjects(*m_pVkDataPtr);

		for (auto& commandPool : m_pVkDataPtr->m_pCommandPools)
			vk::cleanCommandPool(*m_pVkDataPtr, commandPool);

		for (auto& commandPool : m_pCommandPools)
			vk::cleanCommandPool(*m_pVkDataPtr, commandPool);

		for (auto& texture : texPool)
		{
			vkDestroySampler(m_pVkDataPtr->logicalDevice, texture.sampler, nullptr);
			vkDestroyImageView(m_pVkDataPtr->logicalDevice, texture.imageView, nullptr);
			vmaDestroyImage(m_pAllocator, texture.image, texture.allocation);
		}

		for (auto& pass : renderPassPool)
		{
			vk::cleanupRenderPass(*m_pVkDataPtr, pass.renderPass);

			for (auto& pipeline : pass.renderPipelines)
				vk::destroyGraphicsPipeline(*m_pVkDataPtr, pipeline);

			if (pass.id == std::numeric_limits<uint32_t>::max())
				continue;

			for (auto& framebuffer : pass.renderPassChain.Framebuffers)
				vkDestroyFramebuffer(m_pVkDataPtr->logicalDevice, framebuffer, nullptr);

			for (int i = 0; i < pass.renderPassChain.Textures.size(); i++)
			{
				vkDestroySampler(m_pVkDataPtr->logicalDevice, pass.renderPassChain.Textures[i].sampler, nullptr);
				vkDestroyImageView(m_pVkDataPtr->logicalDevice, pass.renderPassChain.Textures[i].imageView, nullptr);
				vmaDestroyImage(m_pAllocator, pass.renderPassChain.Textures[i].image,
					pass.renderPassChain.Textures[i].allocation);
			}

			if (pass.renderPassChain.DepthTexture.sampler != VK_NULL_HANDLE)
				vkDestroySampler(m_pVkDataPtr->logicalDevice, pass.renderPassChain.DepthTexture.sampler, nullptr);

			if (pass.renderPassChain.DepthTexture.imageView != VK_NULL_HANDLE)
				vkDestroyImageView(m_pVkDataPtr->logicalDevice, pass.renderPassChain.DepthTexture.imageView, nullptr);

			vmaDestroyImage(m_pAllocator, pass.renderPassChain.DepthTexture.image,
				pass.renderPassChain.DepthTexture.allocation);
		}
		vkDestroyDescriptorPool(m_pVkDataPtr->logicalDevice, m_pDescriptorPool, nullptr);

		vkDestroyDescriptorSetLayout(m_pVkDataPtr->logicalDevice, m_pGlobalDescriptorSetLayout, nullptr);
		vkDestroyDescriptorPool(m_pVkDataPtr->logicalDevice, m_pGlobalDescriptorPool, nullptr);

		vmaDestroyBuffer(m_pAllocator, m_pIndirectBuffer.buffer, m_pIndirectBuffer.allocation);

		

		vmaDestroyAllocator(m_pAllocator);
		vk::cleanup(*m_pVkDataPtr);
	}

	/**
	 * Create functions
	 */
	Ref<Buffer> VulkanResourceManager::destroyBuffer(BufferInfo)
	{

		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	Ref<UniformBindings> VulkanResourceManager::createUniformData(UniformBindingInfo bindingInfo)
	{
		auto buffer = pCreateUniformBuffer(bindingInfo);
		return uniformBufferPool.insert(bindingInfo.name, buffer);
	}

	Ref<Mesh> VulkanResourceManager::createMesh(MeshInfo meshInfo)
	{
		std::vector<common::Vertex>* vertices = &meshInfo.vertices;
		std::vector<uint32_t>* indices = &meshInfo.indices;

		auto maxOffset = [](auto& indices) -> uint32_t
		{
			uint32_t out = 0;
			for (auto& i : indices)
				out = i > out ? i : out;
			return out;
		};

		Ref<Buffer> vertexBufferRef = pFetchVertexBuffer(vertices->size());
		vk::VulkanBuffer* vertexBuffer = vertexBufferPool.get(vertexBufferRef);

		Ref<Buffer> indexBufferRef = pFetchIndexBuffer(indices->size(), vertexBuffer->allocatedVertices);
		vk::VulkanBuffer* indexBuffer = indexBufferPool.get(indexBufferRef);

		auto vertexStagingBuffer = pCreateStagingBuffer(*vertices);
		auto indexStagingBuffer = pCreateStagingIndexBuffer(*indices);

		vk::copyBuffer(*m_pVkDataPtr, m_pTransferCommandPool, vertexStagingBuffer.buffer, vertexBuffer->buffer,
			vertices->size() * sizeof(common::Vertex), vertexBuffer->allocatedVertices * sizeof(common::Vertex));

		vmaDestroyBuffer(m_pAllocator, vertexStagingBuffer.buffer, vertexStagingBuffer.allocation);

		vk::copyBuffer(*m_pVkDataPtr, m_pTransferCommandPool, indexStagingBuffer.buffer, indexBuffer->buffer,
			indices->size() * sizeof(IndexType), indexBuffer->allocatedVertices * sizeof(IndexType));

		vmaDestroyBuffer(m_pAllocator, indexStagingBuffer.buffer, indexStagingBuffer.allocation);


		MeshResource meshResource = {
			.vertexBuffer = vertexBufferRef,
			.indexBuffer = indexBufferRef,
			.vertexOffset = vertexBuffer->allocatedVertices,
			.indexOffset = indexBuffer->allocatedVertices,
			.numVertices = (uint32_t)vertices->size(),
			.numIndices = (uint32_t)indices->size() };
		auto ref = meshPool.insert(meshInfo.name, meshResource);

		int maxAllocatingSize = sizeof(IndexType) * (indexBuffer->allocatedVertices + indices->size());

		vertexBuffer->allocatedVertices += vertices->size();
		indexBuffer->allocatedVertices += indices->size();

		return ref;
	}

	Ref<Mesh> VulkanResourceManager::createMesh(AnimatedMeshInfo meshInfo)
	{
		std::vector<common::AnimatedVertex>* vertices = &meshInfo.vertices;
		std::vector<uint32_t>* indices = &meshInfo.indices;

		auto maxOffset = [](auto& indices) -> uint32_t
		{
			uint32_t out = 0;
			for (auto& i : indices)
				out = i > out ? i : out;
			return out;
		};

		Ref<Buffer> vertexBufferRef = pFetchVertexBuffer(vertices->size(), sizeof(common::AnimatedVertex));
		vk::VulkanBuffer* vertexBuffer = vertexBufferPool.get(vertexBufferRef);

		Ref<Buffer> indexBufferRef = pFetchIndexBuffer(indices->size(), vertexBuffer->allocatedVertices);
		vk::VulkanBuffer* indexBuffer = indexBufferPool.get(indexBufferRef);

		auto vertexStagingBuffer = pCreateStagingBuffer(*vertices);
		auto indexStagingBuffer = pCreateStagingIndexBuffer(*indices);

		vk::copyBuffer(*m_pVkDataPtr, m_pTransferCommandPool, vertexStagingBuffer.buffer, vertexBuffer->buffer,
			vertices->size() * sizeof(common::AnimatedVertex), vertexBuffer->allocatedVertices * sizeof(common::AnimatedVertex));

		vmaDestroyBuffer(m_pAllocator, vertexStagingBuffer.buffer, vertexStagingBuffer.allocation);

		vk::copyBuffer(*m_pVkDataPtr, m_pTransferCommandPool, indexStagingBuffer.buffer, indexBuffer->buffer,
			indices->size() * sizeof(IndexType), indexBuffer->allocatedVertices * sizeof(IndexType));

		vmaDestroyBuffer(m_pAllocator, indexStagingBuffer.buffer, indexStagingBuffer.allocation);

		MeshResource meshResource = {
			.vertexBuffer = vertexBufferRef,
			.indexBuffer = indexBufferRef,
			.vertexOffset = vertexBuffer->allocatedVertices,
			.indexOffset = indexBuffer->allocatedVertices,
			.numVertices = (uint32_t)vertices->size(),
			.numIndices = (uint32_t)indices->size() };
		auto ref = meshPool.insert(meshInfo.name, meshResource);

		int maxAllocatingSize = sizeof(IndexType) * (indexBuffer->allocatedVertices + indices->size());

		vertexBuffer->allocatedVertices += vertices->size();
		indexBuffer->allocatedVertices += indices->size();

		return ref;
	}

	Ref<Buffer> VulkanResourceManager::createGPUBuffer(const std::string& name, uint32_t size, BufferStorageType type)
	{

		vk::VulkanGPUMappedBuffer buffer;
		buffer.buffers.resize(vk::MAX_FRAMES_IN_FLIGHT);

		for (unsigned i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++)
		{
			if (type == BufferStorageType::UNIFORM_BUFFER)
			{
				pCreateBuffer(buffer.buffers[i], size, UNIFORM_BUFFER_USAGE, UNIFORM_BUFFER_PROP, UNIFORM_BUFFER_MEM_USAGE);
			}
			else
			{
				pCreateBuffer(buffer.buffers[i], size, STORAGE_BUFFER_USAGE, STORAGE_BUFFER_PROP, STORAGE_BUFFER_MEM_USAGE);
			}
		}

		buffer.size = size;
		return gpuBufferPool.insert(name, buffer);
	}

	/**
	 * Get functions
	 *
	 */

	vk::VulkanRenderPass* VulkanResourceManager::getRenderPass(Ref<RenderPass> ref)
	{

		return renderPassPool.get(ref);
	}

	/**
	 * Destroy functions
	 */
	void VulkanResourceManager::destroyTexture(Ref<Texture> textureRef)
	{
		auto texture = texPool.get(textureRef);
		vkDestroySampler(m_pVkDataPtr->logicalDevice, texture->sampler, nullptr);
		vkDestroyImageView(m_pVkDataPtr->logicalDevice, texture->imageView, nullptr);
		vmaDestroyImage(m_pAllocator, texture->image, texture->allocation);
	}

	void VulkanResourceManager::destroyBuffer(Ref<Buffer>)
	{
		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	void VulkanResourceManager::destroyUniformData(Ref<UniformBindings>)
	{

		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	void VulkanResourceManager::destroyMaterial(Ref<BindGroup>)
	{

		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	void VulkanResourceManager::destroyMesh(Ref<Mesh>)
	{

		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	void VulkanResourceManager::destroyRenderPass(Ref<RenderPass> renderPassRef)
	{
		auto pass = renderPassPool.get(renderPassRef);
		vk::cleanupRenderPass(*m_pVkDataPtr, pass->renderPass);
		for (auto& pipeline : pass->renderPipelines)
			vk::destroyGraphicsPipeline(*m_pVkDataPtr, pipeline);

		if (pass->id == std::numeric_limits<uint32_t>::max())
			return;

		for (int i = 0; i < pass->renderPassChain.Textures.size(); i++)
		{
			vkDestroySampler(m_pVkDataPtr->logicalDevice, pass->renderPassChain.Textures[i].sampler, nullptr);
			vkDestroyImageView(m_pVkDataPtr->logicalDevice, pass->renderPassChain.Textures[i].imageView, nullptr);
			vmaDestroyImage(m_pAllocator, pass->renderPassChain.Textures[i].image,
				pass->renderPassChain.Textures[i].allocation);
		}

		if (pass->renderPassChain.DepthTexture.sampler != VK_NULL_HANDLE)
			vkDestroySampler(m_pVkDataPtr->logicalDevice, pass->renderPassChain.DepthTexture.sampler, nullptr);

		if (pass->renderPassChain.DepthTexture.imageView != VK_NULL_HANDLE)
			vkDestroyImageView(m_pVkDataPtr->logicalDevice, pass->renderPassChain.DepthTexture.imageView, nullptr);

		vmaDestroyImage(m_pAllocator, pass->renderPassChain.DepthTexture.image,
			pass->renderPassChain.DepthTexture.allocation);
	}

	void* VulkanResourceManager::mapBuffer(Ref<Buffer> bufferRef, uint32_t currentFrame)
	{
		auto buffer = gpuBufferPool.get(bufferRef);
		buffer->buffers[currentFrame].mapped = true;
		void* data;
		vmaMapMemory(m_pAllocator, buffer->buffers[currentFrame].allocation, &data);
		return data;
	}
	void VulkanResourceManager::unmapBuffer(Ref<Buffer> bufferRef, uint32_t currentFrame)
	{
		auto buffer = gpuBufferPool.get(bufferRef);

		if (!buffer->buffers[currentFrame].mapped) return;
		buffer->buffers[currentFrame].mapped = false;
		vmaUnmapMemory(m_pAllocator, buffer->buffers[currentFrame].allocation);
	}

	void* VulkanResourceManager::getMappedBuffer(Ref<Buffer> bufferRef, uint32_t currentFrame) {
		auto buffer = gpuBufferPool.get(bufferRef);

		buffer->buffers[currentFrame].mapped = true;

		void* data;
		vmaMapMemory(m_pAllocator, buffer->buffers[currentFrame].allocation, &data);

		return data;
	}
}