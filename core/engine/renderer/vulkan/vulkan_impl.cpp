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
        pCreateBuffer(buffer, sizeof(Vertex) * MAX_VERTICES_PER_BUFFER, VERTEX_BUFFER_USAGE, VERTEX_BUFFER_PROP, VERTEX_BUFFER_MEM_USAGE);
        if(0) IO::Info("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "allocating ", sizeof(Vertex) * MAX_VERTICES_PER_BUFFER, " bytes in local vertex buffer");
        return buffer;
    }

    vk::VulkanBuffer VulkanResourceManager::pCreateIndexBuffer()
    {
        vk::VulkanBuffer buffer;
        pCreateBuffer(buffer, sizeof(IndexType) * MAX_INDICES_PER_BUFFER, INDEX_BUFFER_USAGE, INDEX_BUFFER_PROP, INDEX_BUFFER_MEM_USAGE);
        if(0) IO::Info("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "allocating ", sizeof(IndexType) * MAX_INDICES_PER_BUFFER, " bytes in local index buffer");
        return buffer;
    }

    vk::VulkanUniformBuffer VulkanResourceManager::pCreateUniformBuffer(UniformBindingInfo bindingInfo)
    {
        //uniformBufferPool
        vk::VulkanUniformBuffer buffer;
        buffer.size = bindingInfo.size;
        for(auto & i : buffer.buffers) {
            i.size = buffer.size;
            pCreateBuffer(i, bindingInfo.size, UNIFORM_BUFFER_USAGE, UNIFORM_BUFFER_PROP,
                          UNIFORM_BUFFER_MEM_USAGE);
            IO::Info("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "allocating ", size,
                     " bytes in local uniform buffer");
        }
        return buffer;
    }

    vk::VulkanBuffer VulkanResourceManager::pCreateStagingTextureBuffer(unsigned char *pixels, TextureInfo textureInfo)
    {
        vk::VulkanBuffer stagingBuffer;
        auto imageSize = textureInfo.width * textureInfo.height * textureInfo.numChannels;
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

        vmaCreateBuffer(m_pAllocator, &bufferInfo, &allocInfo, &buffer.buffer, &buffer.allocation, nullptr);
    }

    void VulkanResourceManager::pCreateUniformBuffers()
    {
        //VkDeviceSize bufferSize = sizeof(ShaderData);

        // m_pUniformBuffers.resize(vk::MAX_FRAMES_IN_FLIGHT);

        //for (size_t i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++)
        //{
        //    pCreateUniformBuffer(bufferSize);
        //}
    }

    vk::VulkanTexture VulkanResourceManager::pCreateTextureBuffer(vk::VulkanTextureInfo texInfo)
    {
        vk::VulkanTexture texture;

        texture.info = texInfo;

        texture.imageInfo = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
        texture.imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

        texture.imageInfo.extent.width = static_cast<uint32_t>(texInfo.width);
        texture.imageInfo.extent.height = static_cast<uint32_t>(texInfo.height);
        texture.imageInfo.extent.depth = 1;
        texture.compareEnable = texInfo.compareEnable;
        texture.imageInfo.mipLevels = 1;
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

        vmaCreateImage(m_pAllocator, &texture.imageInfo, &allocInfo, &texture.image, &texture.allocation, nullptr);
        // vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &buffer, &allocation, nullptr);

        // auto texture = vk::createImage(m_pVkData, texInfo);
        // buffer.deviceMemory = vk::allocateTextureMemory(m_pVkData, texture, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        //IO::Info("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "allocating ", texture.image.size, " bytes in local texture buffer");
        return texture;
    }

    Ref<Buffer> VulkanResourceManager::pFetchVertexBuffer(uint32_t numVertices) {
        for(auto& bufferRef: vertexBufferReferences) {
            auto buffer = vertexBufferPool.get(bufferRef);
            if(MAX_VERTICES_PER_BUFFER * sizeof(Vertex) >= buffer->allocatedVertices * sizeof(Vertex) + numVertices * sizeof(Vertex)) {
                return bufferRef;
            }
        }

        auto vertexBuffer = pCreateVertexBuffer();
        vertexBuffer.id = vertexBufferCount;
        vertexBuffer.allocatedVertices = 0;
        vertexBufferCount++;
        auto ref = vertexBufferPool.insert(vertexBuffer);
        vertexBufferReferences.push_back(ref);

        return ref;
    }

    Ref<Buffer> VulkanResourceManager::pFetchIndexBuffer(uint32_t numIndices, uint32_t maxOffset) {
        for(auto& bufferRef: indexBufferReferences) {
            auto buffer = indexBufferPool.get(bufferRef);

            int maxAllocatingSize = sizeof(IndexType) * ( buffer->allocatedVertices + numIndices);
            if(MAX_INDICES_PER_BUFFER * sizeof(IndexType) >= maxAllocatingSize) {
                return bufferRef;
            }
        }

        auto indexBuffer = pCreateIndexBuffer();
        indexBuffer.id = indexBufferCount;
        indexBuffer.allocatedVertices = 0;
        indexBufferCount++;
        auto ref = indexBufferPool.insert(indexBuffer);
        indexBufferReferences.push_back(ref);
        
        return ref;
    }

    void VulkanResourceManager::pCreateDescriptorPool()
    {
        std::array<VkDescriptorPoolSize, 2> poolSizes{};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(vk::MAX_FRAMES_IN_FLIGHT * m_pNumCommandPools);
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(vk::MAX_FRAMES_IN_FLIGHT * m_pNumCommandPools);

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();

        poolInfo.maxSets = static_cast<uint32_t>(vk::MAX_FRAMES_IN_FLIGHT * m_pNumCommandPools);
        if (vkCreateDescriptorPool(m_pVkDataPtr->logicalDevice, &poolInfo, nullptr, &m_pDescriptorPool) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    void VulkanResourceManager::pCreateDescriptorSets(vk::VulkanMaterial &material)
    {
        std::vector<VkDescriptorSetLayout> layouts(vk::MAX_FRAMES_IN_FLIGHT, material.descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_pDescriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(vk::MAX_FRAMES_IN_FLIGHT);
        allocInfo.pSetLayouts = layouts.data();
        // material.descriptorSets.resize(vk::MAX_FRAMES_IN_FLIGHT);
        material.descriptorSets.resize(vk::MAX_FRAMES_IN_FLIGHT);
        if (vkAllocateDescriptorSets(m_pVkDataPtr->logicalDevice, &allocInfo, material.descriptorSets.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        pUpdateMaterial(material);
    }

    void VulkanResourceManager::pRecreateDescriptorSets() {
        for (auto &material: materialPool) {
            pCreateDescriptorSets(material);
        }
    }
    
    vk::VulkanBuffer VulkanResourceManager::pCreateStagingBuffer(const std::vector<Vertex> &vertices)
    {
        vk::VulkanBuffer stagingBuffer;
        pCreateBuffer(stagingBuffer, vertices.size() * sizeof(Vertex), STAGING_BUFFER_USAGE, STAGING_BUFFER_PROP, STAGING_BUFFER_MEM_USAGE);
        
        void *data;
        vmaMapMemory(m_pAllocator, stagingBuffer.allocation, &data);
        memcpy(data, vertices.data(), vertices.size() * sizeof(Vertex));
        vmaUnmapMemory(m_pAllocator, stagingBuffer.allocation);
        if(0)
        IO::Info("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "allocating ", vertices.size() * sizeof(Vertex), " bytes in hosted staging buffer");
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
        if(0)
        IO::Info("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "allocating ", indices.size() * sizeof(IndexType), " bytes in hosted staging buffer");
        return stagingBuffer;
    }


    void VulkanResourceManager::pUpdateMaterial(vk::VulkanMaterial &material)
    {
        for (int i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++)
        {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = material.bufferInfo[i].buffer;//m_pUniformBuffers[i].buffer;
            bufferInfo.offset = material.bufferOffset;
            bufferInfo.range = material.bufferSize;

            VkDescriptorImageInfo imageInfo{};
            uint32_t numSlots = 1;
            std::vector<VkWriteDescriptorSet> descriptorWrites{};
            descriptorWrites.resize(numSlots);
            if(material.textures.size() > 0 || material.renderBufferBindings.size() > 0) {

                if(material.renderBufferBindings.size() > 0) {
                    numSlots += material.renderBufferBindings.size();
                    descriptorWrites.resize(numSlots);
                } if(material.textures.size() > 0) {
                    numSlots += material.textures.size();
                    descriptorWrites.resize(numSlots);
                }
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                int index = 1;

                for(auto& bindings: material.renderBufferBindings) {

                    imageInfo.imageView = bindings.imageView;
                    imageInfo.sampler = bindings.sampler;

                    descriptorWrites[index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    descriptorWrites[index].dstSet = material.descriptorSets[i];
                    descriptorWrites[index].dstBinding = index;
                    descriptorWrites[index].dstArrayElement = 0;
                    descriptorWrites[index].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    descriptorWrites[index].descriptorCount = 1;
                    descriptorWrites[index].pImageInfo = &imageInfo;
                    index++;
                }
                for(auto& texture: material.textures) {

                    imageInfo.imageView = texture.imageView;
                    imageInfo.sampler = texture.sampler;

                    descriptorWrites[index].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    descriptorWrites[index].dstSet = material.descriptorSets[i];
                    descriptorWrites[index].dstBinding = index;
                    descriptorWrites[index].dstArrayElement = 0;
                    descriptorWrites[index].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    descriptorWrites[index].descriptorCount = 1;
                    descriptorWrites[index].pImageInfo = &imageInfo;
                    index++;
                }
            }


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
            // vk::updateDescriptorSet(m_pVkData, material);
        }
    }

    Ref<PushConstant> VulkanResourceManager::createPushConstant(PushConstantData push_constant) {
        return pushConstantPool.insert(push_constant);
    }

}