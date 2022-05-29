#include <framework/window.hpp>

#include "vulkan.hpp"
#include <framework/common.hpp>

namespace engine
{

    VulkanRenderer::VulkanRenderer()
    {
    }
    VulkanRenderer::~VulkanRenderer()
    {
    }
    void VulkanRenderer::Init(const char *appName, framework::Window &window)
    {
        m_pWindow = &window;
        vk::createInstance(appName, m_pVkData);
        vk::setupDebugMessenger(m_pVkData.instance);
        vk::createSurface(m_pVkData, window.getWindow());
        vk::pickPhysicalDevice(m_pVkData);
        vk::createLogicalDevice(m_pVkData);
        vk::createSwapChain(m_pVkData, window.getWindow());
        vk::createImageViews(m_pVkData);

        m_pRenderPipelines.emplace_back();

        vk::createRenderPass(m_pVkData, m_pRenderPipelines[0]);

        m_pVertexInfo.attributeDescriptions =
            vk::getAttributeDescriptions<6>(0,
                                            {{VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)},
                                             {VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCoords)},
                                             {VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)},
                                             {VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, color)},
                                             {VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, tangent)},
                                             {VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, bitangent)}});

        m_pVertexInfo.bindingDescription = vk::getBindingDescription<Vertex>();
        vk::createDescriptorSetLayout(m_pVkData, m_pRenderPipelines.back());

        vk::createGraphicsPipeline(m_pVkData, m_pRenderPipelines.back(), m_pVertexInfo);
        vk::createFramebuffers(m_pVkData, m_pRenderPipelines.back());

        m_pCommandPools.emplace_back();

        vk::createCommandPool(m_pVkData, m_pCommandPools.back());

        createVertexBuffer();
        createIndexBuffer();
        createUniformBuffers();

        createDescriptorPool();
        createDescriptorSets();

        vk::createCommandBuffers(m_pVkData, m_pCommandPools.back(), m_pCommandBuffers);
        vk::createSyncObjects(m_pVkData);
    }

    vk::VulkanBuffer *VulkanRenderer::createVertexBuffer()
    {
        auto *buffer = &m_pVertexBuffers.emplace_back();

        buffer->bufferInfo = vk::createVertexBuffer(m_pVkData, buffer->buffer, MAX_VERTICES_PER_BUFFER * sizeof(Vertex), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        buffer->deviceMemory = vk::allocateMemory(m_pVkData, buffer->buffer, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        allocations_count++;
        IO::Log("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "allocating ", MAX_VERTICES_PER_BUFFER * sizeof(Vertex), " bytes in local vertex buffer");

        return buffer;
    }

    vk::VulkanBuffer *VulkanRenderer::createIndexBuffer()
    {
        auto buffer = &m_pIndexBuffers.emplace_back();

        buffer->bufferInfo = vk::createVertexBuffer(m_pVkData, buffer->buffer, MAX_INDICES_PER_BUFFER * sizeof(IndexType), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
        buffer->deviceMemory = vk::allocateMemory(m_pVkData, buffer->buffer, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        allocations_count++;
        IO::Log("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "allocating ", MAX_INDICES_PER_BUFFER * sizeof(IndexType), " bytes in local index buffer");

        return buffer;
    }

    vk::VulkanBuffer *VulkanRenderer::createUniformBuffer(size_t size)
    {
        auto buffer = &m_pUniformBuffers.emplace_back();

        buffer->bufferInfo = vk::createVertexBuffer(m_pVkData, buffer->buffer, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
        buffer->deviceMemory = vk::allocateMemory(m_pVkData, buffer->buffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        allocations_count++;
        IO::Log("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "allocating ", size, " bytes in local uniform buffer");
        
        return buffer;
    }

    void VulkanRenderer::createUniformBuffers()
    {
        VkDeviceSize bufferSize = sizeof(CameraData);

        //m_pUniformBuffers.resize(vk::MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++)
        {
            createUniformBuffer(bufferSize);
        }
    }

    void VulkanRenderer::createDescriptorPool()
    {
        VkDescriptorPoolSize poolSize{};
        poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSize.descriptorCount = static_cast<uint32_t>(vk::MAX_FRAMES_IN_FLIGHT);
        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;
        poolInfo.maxSets = static_cast<uint32_t>(vk::MAX_FRAMES_IN_FLIGHT);
        if (vkCreateDescriptorPool(m_pVkData.logicalDevice, &poolInfo, nullptr, &m_pDescriptorPool) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    void VulkanRenderer::createDescriptorSets()
    {
        std::vector<VkDescriptorSetLayout> layouts(vk::MAX_FRAMES_IN_FLIGHT, m_pRenderPipelines.back().descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_pDescriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(vk::MAX_FRAMES_IN_FLIGHT);
        allocInfo.pSetLayouts = layouts.data();

        m_pDescriptorSets.resize(vk::MAX_FRAMES_IN_FLIGHT);
        if (vkAllocateDescriptorSets(m_pVkData.logicalDevice, &allocInfo, m_pDescriptorSets.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        for (size_t i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++)
        {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = m_pUniformBuffers[i].buffer;
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(CameraData);

            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = m_pDescriptorSets[i];
            descriptorWrite.dstBinding = 0;
            descriptorWrite.dstArrayElement = 0;

            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrite.descriptorCount = 1;

            descriptorWrite.pBufferInfo = &bufferInfo;
            descriptorWrite.pImageInfo = nullptr;       // Optional
            descriptorWrite.pTexelBufferView = nullptr; // Optional

            vkUpdateDescriptorSets(m_pVkData.logicalDevice, 1, &descriptorWrite, 0, nullptr);
        }
    }

    void VulkanRenderer::createStagingBuffer(const std::vector<Vertex> &vertices)
    {
        m_pStagingBuffer.bufferInfo = vk::createVertexBuffer(m_pVkData, m_pStagingBuffer.buffer, vertices.size() * sizeof(Vertex), VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
        m_pStagingBuffer.deviceMemory = vk::allocateMemory(m_pVkData, m_pStagingBuffer.buffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        allocations_count++;
        vk::mapMemory(m_pVkData, m_pStagingBuffer.deviceMemory, vertices.size() * sizeof(Vertex), 0, vertices.data());
        IO::Log("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "allocating ", vertices.size() * sizeof(Vertex), " bytes in hosted staging buffer");
    }

    void VulkanRenderer::createStagingIndexBuffer(const std::vector<IndexType> &indices)
    {
        m_pStagingIndexBuffer.bufferInfo = vk::createVertexBuffer(m_pVkData, m_pStagingIndexBuffer.buffer, indices.size() * sizeof(IndexType), VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
        m_pStagingIndexBuffer.deviceMemory = vk::allocateMemory(m_pVkData, m_pStagingIndexBuffer.buffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        allocations_count++;
        vk::mapMemory(m_pVkData, m_pStagingIndexBuffer.deviceMemory, indices.size() * sizeof(IndexType), 0, indices.data());
        IO::Log("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "allocating ", indices.size() * sizeof(IndexType), " bytes in hosted staging buffer");
    }

    uint32_t VulkanRenderer::Submit(const std::vector<Vertex> &vertices, const std::vector<IndexType> &indices, const MaterialInfo &)
    {
        auto [buffer, indexBuffer] = getBuffers(vertices, indices);

        createStagingBuffer(vertices);
        createStagingIndexBuffer(indices);

        vk::copyBuffer(m_pVkData, m_pCommandPools.back(), m_pStagingBuffer.buffer, buffer->buffer, vertices.size() * sizeof(Vertex), current_vertex_count * sizeof(Vertex));

        vkDestroyBuffer(m_pVkData.logicalDevice, m_pStagingBuffer.buffer, nullptr);
        vkFreeMemory(m_pVkData.logicalDevice, m_pStagingBuffer.deviceMemory, nullptr);

        IO::Log("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "copied ", vertices.size(), " vertices, of size ", vertices.size() * sizeof(Vertex), " bytes, at offset ", current_vertex_count * sizeof(Vertex), " to local buffer");

        vk::copyBuffer(m_pVkData, m_pCommandPools.back(), m_pStagingIndexBuffer.buffer, indexBuffer->buffer, indices.size() * sizeof(IndexType), current_index_count * sizeof(IndexType));

        vkDestroyBuffer(m_pVkData.logicalDevice, m_pStagingIndexBuffer.buffer, nullptr);
        vkFreeMemory(m_pVkData.logicalDevice, m_pStagingIndexBuffer.deviceMemory, nullptr);

        IO::Log("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "copied ", indices.size(), " indices, of size ", indices.size() * sizeof(IndexType), " bytes, at offset ", current_index_count * sizeof(IndexType), " to local buffer");

        current_vertex_count += vertices.size();
        current_index_count += indices.size();

        buffer->allocatedVertices += vertices.size();
        indexBuffer->allocatedVertices += indices.size();

        return m_pVertexBuffers.size() - 1;
    }

    std::pair<vk::VulkanBuffer *, vk::VulkanBuffer *> VulkanRenderer::getBuffers(const VertexContainer &vertices, const IndexContainer &indices)
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
            buffer = createVertexBuffer();
            ibuffer = createIndexBuffer();
        }

        return {buffer, ibuffer};
    }

    void VulkanRenderer::Push(uint32_t object_id)
    {
        m_pCurrentFrameObjects.push_back(object_id);
    }

    void VulkanRenderer::PushCameraData(const CameraData &camData)
    {
        m_pCameraData = camData;
    }

    void VulkanRenderer::updateUniforms()
    {
        void *data;
        vkMapMemory(m_pVkData.logicalDevice, m_pUniformBuffers[m_pCurrentFrame].deviceMemory, 0, sizeof(CameraData), 0, &data);
        memcpy(data, &m_pCameraData, sizeof(m_pCameraData));
        vkUnmapMemory(m_pVkData.logicalDevice, m_pUniformBuffers[m_pCurrentFrame].deviceMemory);
    }

    void VulkanRenderer::Begin()
    {
        m_pImageIndex = vk::prepareSyncObjects(m_pVkData,
                                               m_pWindow->getWindow(),
                                               m_pCurrentFrame,
                                               m_pRenderPipelines[0],
                                               m_pVertexInfo);

        vkResetCommandBuffer(m_pCommandBuffers[m_pCurrentFrame], 0);

        updateUniforms();
        vk::recordCommandBuffer(m_pCommandBuffers[m_pCurrentFrame], m_pImageIndex);
        vk::createRenderPassInfo(m_pImageIndex, m_pVkData, m_pRenderPipelines[0]);
        vk::beginRenderPass(m_pCommandBuffers[m_pCurrentFrame], m_pRenderPipelines[0]);
    }

    void VulkanRenderer::End()
    {
        vk::endRenderPass(m_pCommandBuffers[m_pCurrentFrame], m_pVkData);
        vk::endRecording(m_pCommandBuffers[m_pCurrentFrame]);

        VkSemaphore signalSemaphores[] = {m_pVkData.renderFinishedSemaphores[m_pCurrentFrame]};

        vk::Sync(m_pVkData, m_pCommandBuffers[m_pCurrentFrame], m_pCurrentFrame);

        vk::Present(m_pVkData, signalSemaphores, m_pImageIndex);

        m_pCurrentFrame = (m_pCurrentFrame + 1) % vk::MAX_FRAMES_IN_FLIGHT;
    }

    void VulkanRenderer::Flush()
    {
        vk::bindPipeline(m_pRenderPipelines[0], m_pCommandBuffers[m_pCurrentFrame]);

        vk::bindVertexBuffer(m_pVkData, m_pCommandBuffers[m_pCurrentFrame], m_pVertexBuffers[0].buffer);

        vkCmdBindIndexBuffer(m_pCommandBuffers[m_pCurrentFrame], m_pIndexBuffers[0].buffer, 0, VK_INDEX_TYPE_UINT32);

        vkCmdBindDescriptorSets(m_pCommandBuffers[m_pCurrentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pRenderPipelines.back().pipelineLayout, 0, 1, &m_pDescriptorSets[m_pCurrentFrame], 0, nullptr);

        vk::drawIndexed(m_pCommandBuffers[m_pCurrentFrame], current_index_count, 1, 0, 0, 0);

        m_pCurrentFrameObjects.clear();
    }

    void VulkanRenderer::Cleanup()
    {
        vkDeviceWaitIdle(m_pVkData.logicalDevice);

        vkDestroyDescriptorPool(m_pVkData.logicalDevice, m_pDescriptorPool, nullptr);

        vkDestroyDescriptorSetLayout(m_pVkData.logicalDevice, m_pRenderPipelines.back().descriptorSetLayout, nullptr);

        for (size_t i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++)
        {
            vkDestroyBuffer(m_pVkData.logicalDevice, m_pUniformBuffers[i].buffer, nullptr);
            vkFreeMemory(m_pVkData.logicalDevice, m_pUniformBuffers[i].deviceMemory, nullptr);
        }

        vkDestroyDescriptorSetLayout(m_pVkData.logicalDevice, m_pRenderPipelines.back().descriptorSetLayout, nullptr);

        vk::cleanupSyncObjects(m_pVkData);
        vk::cleanCommandPool(m_pVkData, m_pCommandPools[0]);

        for (auto &buffer : m_pVertexBuffers)
        {
            vk::destroyVertexBuffer(m_pVkData, buffer.buffer);
            vk::freeMemory(m_pVkData, buffer.deviceMemory);
        }

        for (auto &buffer : m_pIndexBuffers)
        {
            vk::destroyVertexBuffer(m_pVkData, buffer.buffer);
            vk::freeMemory(m_pVkData, buffer.deviceMemory);
        }

        vk::destroyGraphicsPipeline(m_pVkData, m_pRenderPipelines[0]);
        vk::cleanupRenderPass(m_pVkData, m_pRenderPipelines[0].renderPass);
        vk::cleanup(m_pVkData);
    }
}