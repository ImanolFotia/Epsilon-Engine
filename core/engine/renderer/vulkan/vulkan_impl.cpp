#undef VMA_DEBUG_LOG
#undef VMA_DEBUG_LOG_FORMAT
#include "resource_manager.hpp"

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
        auto imageSize = textureInfo.width * textureInfo.height * /*textureInfo.numChannels*/ 4;
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
        texture.imageInfo.arrayLayers = 1;
        texture.imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        texture.imageInfo.imageType = VK_IMAGE_TYPE_2D;

        texture.imageInfo.format = texInfo.format;
        texture.format = texInfo.format;
        texture.imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        texture.imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        texture.imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        texture.imageInfo.flags = 0; // Optional
        texture.imageInfo.usage = texInfo.usage;

        texture.addressMode = texInfo.addressMode;
        texture.compareOp = texInfo.compareOp;
        texture.filter = texInfo.filter;

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO;

        int num_channels = 0;
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
        ResourcesMemory.m_pTextureBufferAllocationSize += texInfo.width * texInfo.height * num_channels * size;

        //std::cout << texInfo.width * texInfo.height * num_channels * size << std::endl;
        return texture;
    }

    Ref<Buffer> VulkanResourceManager::pFetchVertexBuffer(uint32_t numVertices)
    {
        for (auto &bufferRef : vertexBufferReferences)
        {
            auto buffer = vertexBufferPool.get(bufferRef);
            if (MAX_VERTICES_PER_BUFFER * sizeof(common::Vertex) >= buffer->allocatedVertices * sizeof(common::Vertex) + numVertices * sizeof(common::Vertex))
            {
                return bufferRef;
            }
        }

        auto vertexBuffer = pCreateVertexBuffer();
        vertexBuffer.id = vertexBufferCount;
        vertexBuffer.allocatedVertices = 0;
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

            int maxAllocatingSize = sizeof(IndexType) * (buffer->allocatedVertices + numIndices);
            if (MAX_INDICES_PER_BUFFER * sizeof(IndexType) >= maxAllocatingSize)
            {
                return bufferRef;
            }
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

        poolInfo.maxSets = poolSizes.size() * 2;
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
        globalPoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
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

            uint32_t numSlots = material.slots+1;
            std::vector<VkWriteDescriptorSet> descriptorWrites{};
            std::list<VkDescriptorImageInfo> imageInfos;
            std::list<VkDescriptorBufferInfo> bufferInfos;
            // numSlots += material.shaderBindings.size();
            descriptorWrites.resize(numSlots);

            for (int index = 1; index < descriptorWrites.size(); index++)
            {
                int j = 0;
                for (auto &binding : material.shaderBindings)
                {
                    if (binding.descriptorBinding == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER && binding.isRenderPassAttachment)
                    {
                        auto &imageInfo = imageInfos.emplace_back();

                        imageInfo.imageLayout = binding.texture.isDepthAttachment ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                        imageInfo.imageLayout = binding.texture.format == VK_FORMAT_D32_SFLOAT ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL : imageInfo.imageLayout;
                        
                        if (!binding.renderpass.empty()) {
                            auto pass = renderPassPool.get(std::hash<std::string>{}(binding.renderpass));

                            if (binding.attachment_index == pass->renderPassChain.Textures.size()) {
                                if (pass->id == std::numeric_limits<uint32_t>::max()) {
                                    binding.texture = m_pVkDataPtr->defaultRenderPass.renderPassChain.DepthTexture;
                                }
                                else {
                                    binding.texture = pass->renderPassChain.DepthTexture;

                                }
                            }
                            else {
                                binding.texture = pass->renderPassChain.Textures.at(binding.attachment_index);
                            }
                        }

                        imageInfo.imageView = binding.texture.imageView;
                        imageInfo.sampler = binding.texture.sampler;

                        descriptorWrites[binding.bindingPoint].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                        descriptorWrites[binding.bindingPoint].dstSet = material.descriptorSets[i];
                        descriptorWrites[binding.bindingPoint].dstBinding = binding.bindingPoint;
                        descriptorWrites[binding.bindingPoint].dstArrayElement = 0;
                        descriptorWrites[binding.bindingPoint].descriptorType = binding.descriptorBinding;
                        descriptorWrites[binding.bindingPoint].descriptorCount = 1;
                        descriptorWrites[binding.bindingPoint].pImageInfo = &imageInfo;
                    }
                    else if (binding.descriptorBinding == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
                    {
                        VkDescriptorBufferInfo& bufferInfo = bufferInfos.emplace_back();
                        bufferInfo.buffer = binding.buffer.buffer; // m_pUniformBuffers[i].buffer;
                        bufferInfo.offset = binding.buffer.offset;
                        bufferInfo.range = binding.buffer.size;
                        descriptorWrites[binding.bindingPoint].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                        descriptorWrites[binding.bindingPoint].dstSet = material.descriptorSets[i];
                        descriptorWrites[binding.bindingPoint].dstBinding = binding.bindingPoint;
                        descriptorWrites[binding.bindingPoint].dstArrayElement = 0;
                        descriptorWrites[binding.bindingPoint].descriptorType = binding.descriptorBinding;
                        descriptorWrites[binding.bindingPoint].descriptorCount = 1;
                        descriptorWrites[binding.bindingPoint].pBufferInfo = &bufferInfo;
                        descriptorWrites[binding.bindingPoint].pImageInfo = nullptr;       // Optional
                        descriptorWrites[binding.bindingPoint].pTexelBufferView = nullptr; // Optional
                    }
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
            descriptorWrites[0].pImageInfo = nullptr;       // Optional
            descriptorWrites[0].pTexelBufferView = nullptr; // Optional

            vkUpdateDescriptorSets(m_pVkDataPtr->logicalDevice, numSlots, descriptorWrites.data(), 0, nullptr);
        }
    }

    Ref<PushConstant> VulkanResourceManager::createPushConstant(const std::string &name, PushConstantData push_constant)
    {
        return pushConstantPool.insert(name, push_constant);
    }


}