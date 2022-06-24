#include "vulkan.hpp"

#include <vk_mem_alloc.h>

/**
 * @brief Implementation of the Vulkan renderer private methods
 *
 */
namespace engine
{

    void VulkanRenderer::pUpdateUniforms()
    {
        void *data;
        vmaMapMemory(m_pAllocator, m_pFrame.UniformBuffer().allocation, &data);
        memcpy(data, &m_pCameraData, sizeof(m_pCameraData));
        vmaUnmapMemory(m_pAllocator, m_pFrame.UniformBuffer().allocation);
    }

    void VulkanRenderer::pCreateVertexBuffer()
    {
        auto &buffer = m_pVertexBuffers.emplace_back();
        pCreateBuffer(buffer, sizeof(Vertex) * MAX_VERTICES_PER_BUFFER, VERTEX_BUFFER_USAGE, VERTEX_BUFFER_PROP, VERTEX_BUFFER_MEM_USAGE);
        // auto allocation = pGetOrCreateDeviceMemory(VERTEX_BUFFER_PROP, buffer);
        // vkBindBufferMemory(m_pVkData.logicalDevice, buffer.buffer, allocation.deviceMemory, allocation.allocatedBytes);
        IO::Log("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "allocating ", sizeof(Vertex) * MAX_VERTICES_PER_BUFFER, " bytes in local vertex buffer");
    }

    void VulkanRenderer::pCreateIndexBuffer()
    {
        auto &buffer = m_pIndexBuffers.emplace_back();
        pCreateBuffer(buffer, sizeof(IndexType) * MAX_INDICES_PER_BUFFER, INDEX_BUFFER_USAGE, INDEX_BUFFER_PROP, INDEX_BUFFER_MEM_USAGE);

        // vkBindBufferMemory(m_pVkData.logicalDevice, buffer.buffer, allocation.deviceMemory, allocation.allocatedBytes);
        IO::Log("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "allocating ", sizeof(IndexType) * MAX_INDICES_PER_BUFFER, " bytes in local index buffer");
    }

    void VulkanRenderer::pCreateUniformBuffer(size_t size)
    {
        auto &buffer = m_pUniformBuffers.emplace_back();
        pCreateBuffer(buffer, size, UNIFORM_BUFFER_USAGE, UNIFORM_BUFFER_PROP, UNIFORM_BUFFER_MEM_USAGE);
        // auto allocation = pGetOrCreateDeviceMemory(UNIFORM_BUFFER_PROP, buffer);
        // vkBindBufferMemory(m_pVkData.logicalDevice, buffer.buffer, allocation.deviceMemory, allocation.allocatedBytes);

        IO::Log("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "allocating ", size, " bytes in local uniform buffer");
    }

    void VulkanRenderer::pCreateStagingTextureBuffer(unsigned char *pixels, TextureInfo textureInfo)
    {
        auto imageSize = textureInfo.width * textureInfo.height * textureInfo.numChannels;
        pCreateBuffer(m_pStagingTextureBuffer, imageSize, TEXTURE_BUFFER_USAGE, TEXTURE_BUFFER_PROP, TEXTURE_BUFFER_MEM_USAGE);

        void *data;
        vmaMapMemory(m_pAllocator, m_pStagingTextureBuffer.allocation, &data);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
        vmaUnmapMemory(m_pAllocator, m_pStagingTextureBuffer.allocation);
    }

    void VulkanRenderer::pCreateBuffer(vk::VulkanBuffer &buffer, size_t size, VkBufferUsageFlags usage, VmaAllocationCreateFlags properties, VmaMemoryUsage mem_usage)
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

        allocations_count++;
    }

    void VulkanRenderer::pCreateUniformBuffers()
    {
        VkDeviceSize bufferSize = sizeof(ShaderData);

        // m_pUniformBuffers.resize(vk::MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++)
        {
            pCreateUniformBuffer(bufferSize);
        }
    }

    vk::VulkanTexture VulkanRenderer::pCreateTextureBuffer(vk::VulkanTextureInfo texInfo)
    {
        auto &buffer = m_pTextureBuffers.emplace_back();

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

    void VulkanRenderer::pCreateDescriptorPool()
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
        if (vkCreateDescriptorPool(m_pVkData.logicalDevice, &poolInfo, nullptr, &m_pDescriptorPool) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    void VulkanRenderer::pCreateDescriptorSets(vk::VulkanMaterial &material)
    {
        std::vector<VkDescriptorSetLayout> layouts(vk::MAX_FRAMES_IN_FLIGHT, m_pRenderPasses.at(0).renderPipelines.back().descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_pDescriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(vk::MAX_FRAMES_IN_FLIGHT);
        allocInfo.pSetLayouts = layouts.data();
        // material.descriptorSets.resize(vk::MAX_FRAMES_IN_FLIGHT);
        material.descriptorSets.resize(vk::MAX_FRAMES_IN_FLIGHT);
        if (vkAllocateDescriptorSets(m_pVkData.logicalDevice, &allocInfo, material.descriptorSets.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        pUpdateMaterial(material);
    }

    void VulkanRenderer::pUpdateMaterial(vk::VulkanMaterial &material)
    {

        for (int i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++)
        {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = m_pUniformBuffers[i].buffer;
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(ShaderData);

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
            descriptorWrites[0].pBufferInfo = &bufferInfo;
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

    void VulkanRenderer::pCreateStagingBuffer(const std::vector<Vertex> &vertices)
    {

        pCreateBuffer(m_pStagingBuffer, vertices.size() * sizeof(Vertex), STAGING_BUFFER_USAGE, STAGING_BUFFER_PROP, STAGING_BUFFER_MEM_USAGE);
        allocations_count++;
        void *data;
        vmaMapMemory(m_pAllocator, m_pStagingBuffer.allocation, &data);
        memcpy(data, vertices.data(), vertices.size() * sizeof(Vertex));
        vmaUnmapMemory(m_pAllocator, m_pStagingBuffer.allocation);

        IO::Log("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "allocating ", vertices.size() * sizeof(Vertex), " bytes in hosted staging buffer");
    }

    void VulkanRenderer::pCreateStagingIndexBuffer(const std::vector<IndexType> &indices)
    {
        pCreateBuffer(m_pStagingIndexBuffer, indices.size() * sizeof(IndexType), STAGING_BUFFER_USAGE, STAGING_BUFFER_PROP, STAGING_BUFFER_MEM_USAGE);
        allocations_count++;
        
        // void* data;
        void *data;
        vmaMapMemory(m_pAllocator, m_pStagingIndexBuffer.allocation, &data);
        memcpy(data, indices.data(), indices.size() * sizeof(IndexType));
        vmaUnmapMemory(m_pAllocator, m_pStagingIndexBuffer.allocation);
        IO::Log("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "allocating ", indices.size() * sizeof(IndexType), " bytes in hosted staging buffer");
    }

    std::pair<vk::VulkanBuffer *, vk::VulkanBuffer *> VulkanRenderer::pGetBuffers(const VertexContainer &vertices, const IndexContainer &indices)
    {
        vk::VulkanBuffer *buffer;
        vk::VulkanBuffer *ibuffer;

        if (m_pVertexBuffers.size() < 1)
            &m_pVertexBuffers.emplace_back();
        if (m_pIndexBuffers.size() < 1)
            &m_pIndexBuffers.emplace_back();

        buffer = &m_pVertexBuffers.back();
        ibuffer = &m_pIndexBuffers.back();

        if (buffer->allocatedVertices + vertices.size() > MAX_VERTICES_PER_BUFFER ||
            ibuffer->allocatedVertices + indices.size() > MAX_INDICES_PER_BUFFER)
        {
            pCreateVertexBuffer();
            buffer = &m_pVertexBuffers.back();
            pCreateIndexBuffer();
            ibuffer = &m_pIndexBuffers.back();
        }

        return {buffer, ibuffer};
    }

}