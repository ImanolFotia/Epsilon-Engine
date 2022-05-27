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

        vk::createGraphicsPipeline(m_pVkData, m_pRenderPipelines.back(), m_pVertexInfo);
        vk::createFramebuffers(m_pVkData, m_pRenderPipelines.back());

        m_pCommandPools.emplace_back();

        vk::createCommandPool(m_pVkData, m_pCommandPools.back());

        vk::createCommandBuffers(m_pVkData, m_pCommandPools.back(), m_pCommandBuffers);
        vk::createSyncObjects(m_pVkData);

        createVertexBuffer();
        createIndexBuffer();
    }

    vk::VulkanBuffer *VulkanRenderer::createVertexBuffer()
    {
        auto *buffer = &m_pVertexBuffers.emplace_back();

        buffer->bufferInfo = vk::createVertexBuffer(m_pVkData, buffer->buffer, MAX_VERTICES_PER_BUFFER * sizeof(Vertex), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        buffer->deviceMemory = vk::allocateMemory(m_pVkData, buffer->buffer, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        IO::Log("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "allocating ", MAX_VERTICES_PER_BUFFER * sizeof(Vertex), " bytes in local vertex buffer");

        return buffer;
    }

    vk::VulkanBuffer *VulkanRenderer::createIndexBuffer()
    {
        auto buffer = &m_pIndexBuffers.emplace_back();

        buffer->bufferInfo = vk::createVertexBuffer(m_pVkData, buffer->buffer, MAX_INDICES_PER_BUFFER * sizeof(IndexType), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
        buffer->deviceMemory = vk::allocateMemory(m_pVkData, buffer->buffer, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        IO::Log("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "allocating ", MAX_INDICES_PER_BUFFER * sizeof(IndexType), " bytes in local index buffer");

        return buffer;
    }

    void VulkanRenderer::createStagingBuffer(const std::vector<Vertex> &vertices)
    {
        m_pStagingBuffer.bufferInfo = vk::createVertexBuffer(m_pVkData, m_pStagingBuffer.buffer, vertices.size() * sizeof(Vertex), VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
        m_pStagingBuffer.deviceMemory = vk::allocateMemory(m_pVkData, m_pStagingBuffer.buffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        vk::mapMemory(m_pVkData, m_pStagingBuffer.deviceMemory, vertices.size() * sizeof(Vertex), 0, vertices.data());
        IO::Log("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "allocating ", vertices.size() * sizeof(Vertex), " bytes in hosted staging buffer");
    }

    void VulkanRenderer::createStagingIndexBuffer(const std::vector<IndexType> &indices)
    {
        m_pStagingIndexBuffer.bufferInfo = vk::createVertexBuffer(m_pVkData, m_pStagingIndexBuffer.buffer, indices.size() * sizeof(IndexType), VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
        m_pStagingIndexBuffer.deviceMemory = vk::allocateMemory(m_pVkData, m_pStagingIndexBuffer.buffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        vk::mapMemory(m_pVkData, m_pStagingIndexBuffer.deviceMemory, indices.size() * sizeof(IndexType), 0, indices.data());
        IO::Log("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "allocating ", indices.size() * sizeof(IndexType), " bytes in hosted staging buffer");
    }

    uint32_t VulkanRenderer::Submit(const std::vector<Vertex> &vertices, const std::vector<IndexType>& indices, const MaterialInfo &)
    {
        vk::VulkanBuffer *buffer;
        if (m_pVertexBuffers.size() < 1)
            &m_pVertexBuffers.emplace_back();

        buffer = &m_pVertexBuffers.back();

        if (buffer->allocatedVertices >= MAX_VERTICES_PER_BUFFER)
        {
            buffer = createVertexBuffer();
        }

        createStagingBuffer(vertices);

        vk::copyBuffer(m_pVkData, m_pCommandPools.back(), m_pStagingBuffer.buffer, buffer->buffer, vertices.size() * sizeof(Vertex), current_vertex_count * sizeof(Vertex));

        vkDestroyBuffer(m_pVkData.logicalDevice, m_pStagingBuffer.buffer, nullptr);
        vkFreeMemory(m_pVkData.logicalDevice, m_pStagingBuffer.deviceMemory, nullptr);

        IO::Log("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "copied ", vertices.size(), " vertices, of size ", vertices.size() * sizeof(Vertex), " bytes, at offset ", current_vertex_count * sizeof(Vertex), " to local buffer");

        //Index Buffer
        if (m_pIndexBuffers.size() < 1)
            &m_pIndexBuffers.emplace_back();

        vk::VulkanBuffer *indexBuffer = &m_pIndexBuffers.back();

        if (indexBuffer->allocatedVertices >= MAX_INDICES_PER_BUFFER)
        {
            indexBuffer = createIndexBuffer();
        }

        createStagingIndexBuffer(indices);

        vk::copyBuffer(m_pVkData, m_pCommandPools.back(), m_pStagingIndexBuffer.buffer, indexBuffer->buffer, indices.size() * sizeof(IndexType), current_index_count * sizeof(IndexType));

        vkDestroyBuffer(m_pVkData.logicalDevice, m_pStagingIndexBuffer.buffer, nullptr);
        vkFreeMemory(m_pVkData.logicalDevice, m_pStagingIndexBuffer.deviceMemory, nullptr);

        IO::Log("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "copied ", indices.size(), " indices, of size ", indices.size() * sizeof(IndexType), " bytes, at offset ", current_index_count * sizeof(IndexType), " to local buffer");

        // vk::mapMemory(m_pVkData, buffer->deviceMemory, vertices.size() * sizeof(Vertex), current_vertex_count* sizeof(Vertex), vertices.data());
        current_vertex_count += vertices.size();
        current_index_count += indices.size();

        buffer->allocatedVertices += vertices.size();
        return m_pVertexBuffers.size() - 1;
    }

    void VulkanRenderer::Push(uint32_t object_id)
    {
        m_pCurrentFrameObjects.push_back(object_id);
    }

    void VulkanRenderer::Begin()
    {
        m_pImageIndex = vk::prepareSyncObjects(m_pVkData,
                                               m_pWindow->getWindow(),
                                               m_pCurrentFrame,
                                               m_pRenderPipelines[0],
                                               m_pVertexInfo);
        // vkAcquireNextImageKHR(vk::logicalDevice, vk::swapChain, UINT64_MAX, vk::imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
        vkResetCommandBuffer(m_pCommandBuffers[m_pCurrentFrame], 0);

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
        
        vk::drawIndexed(m_pCommandBuffers[m_pCurrentFrame], current_index_count, 1, 0, 0, 0);
        
        m_pCurrentFrameObjects.clear();
    }

    void VulkanRenderer::Cleanup()
    {
        vkDeviceWaitIdle(m_pVkData.logicalDevice);
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