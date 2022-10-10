#include "resource_manager.hpp"

#include <vk_mem_alloc.h>

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
        IO::Log("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "allocating ", sizeof(Vertex) * MAX_VERTICES_PER_BUFFER, " bytes in local vertex buffer");
        return buffer;
    }

    vk::VulkanBuffer VulkanResourceManager::pCreateIndexBuffer()
    {
        vk::VulkanBuffer buffer;
        pCreateBuffer(buffer, sizeof(IndexType) * MAX_INDICES_PER_BUFFER, INDEX_BUFFER_USAGE, INDEX_BUFFER_PROP, INDEX_BUFFER_MEM_USAGE);
        IO::Log("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "allocating ", sizeof(IndexType) * MAX_INDICES_PER_BUFFER, " bytes in local index buffer");
        return buffer;
    }

    vk::VulkanBuffer VulkanResourceManager::pCreateUniformBuffer(size_t size)
    {
        vk::VulkanBuffer buffer;
        pCreateBuffer(buffer, size, UNIFORM_BUFFER_USAGE, UNIFORM_BUFFER_PROP, UNIFORM_BUFFER_MEM_USAGE);
        IO::Log("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "allocating ", size, " bytes in local uniform buffer");
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
        VkDeviceSize bufferSize = sizeof(ShaderData);

        // m_pUniformBuffers.resize(vk::MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++)
        {
            pCreateUniformBuffer(bufferSize);
        }
    }

    vk::VulkanTexture VulkanResourceManager::pCreateTextureBuffer(vk::VulkanTextureInfo texInfo)
    {
        vk::VulkanTexture texture;
        texInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

        texture.info = texInfo;

        texture.imageInfo = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
        texture.imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

        texture.imageInfo.extent.width = static_cast<uint32_t>(texInfo.width);
        texture.imageInfo.extent.height = static_cast<uint32_t>(texInfo.height);
        texture.imageInfo.extent.depth = 1;

        texture.imageInfo.mipLevels = 1;
        texture.imageInfo.arrayLayers = 1;
        texture.imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        texture.imageInfo.imageType = VK_IMAGE_TYPE_2D;

        texture.imageInfo.format = texInfo.format;
        texture.imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        texture.imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        texture.imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        texture.imageInfo.flags = 0; // Optional

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO;

        VmaAllocation allocation;
        vmaCreateImage(m_pAllocator, &texture.imageInfo, &allocInfo, &texture.image, &allocation, nullptr);
        // vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &buffer, &allocation, nullptr);

        // auto texture = vk::createImage(m_pVkData, texInfo);
        // buffer.deviceMemory = vk::allocateTextureMemory(m_pVkData, texture, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        IO::Log("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "allocating ", size, " bytes in local uniform buffer");
        return texture;
    }

    void VulkanResourceManager::pCreateDescriptorPool()
    {
        std::array<VkDescriptorPoolSize, 2> poolSizes{};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(vk::MAX_FRAMES_IN_FLIGHT * 2);
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(vk::MAX_FRAMES_IN_FLIGHT * 2);

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();

        poolInfo.maxSets = static_cast<uint32_t>(vk::MAX_FRAMES_IN_FLIGHT * 2);
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

        IO::Log("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "allocating ", vertices.size() * sizeof(Vertex), " bytes in hosted staging buffer");
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
        IO::Log("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "allocating ", indices.size() * sizeof(IndexType), " bytes in hosted staging buffer");
        return stagingBuffer;
    }


    void VulkanRenderer::pUpdateMaterial(vk::VulkanMaterial &material)
    {
        for (int i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++)
        {
            /*VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = m_pUniformBuffers[i].buffer;
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(ShaderData);
            */

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            auto &texture = material.textures.at(0);
            // texture->format = VK_FORMAT_R8G8B8A8_SRGB;
            // vk::createImageView(m_pVkData, *texture, VK_IMAGE_ASPECT_COLOR_BIT);
            // vk::createTextureSampler(m_pVkData, *texture);

            imageInfo.imageView = texture.imageView;
            imageInfo.sampler = texture.sampler;

            std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = material.descriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &material.bufferInfo;
            descriptorWrites[0].pImageInfo = nullptr;       // Optional
            descriptorWrites[0].pTexelBufferView = nullptr; // Optional

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = material.descriptorSets[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pImageInfo = &imageInfo;

            vkUpdateDescriptorSets(m_pVkData.logicalDevice, 2, descriptorWrites.data(), 0, nullptr);
            // vk::updateDescriptorSet(m_pVkData, material);
        }
    }

}