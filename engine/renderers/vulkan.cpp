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
                                            {{VK_FORMAT_R32G32B32_SFLOAT,    offsetof(Vertex, position)},
                                             {VK_FORMAT_R32G32_SFLOAT,       offsetof(Vertex, texCoords)},
                                             {VK_FORMAT_R32G32B32_SFLOAT,    offsetof(Vertex, normal)},
                                             {VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, color)},
                                             {VK_FORMAT_R32G32B32_SFLOAT,    offsetof(Vertex, tangent)},
                                             {VK_FORMAT_R32G32B32_SFLOAT,    offsetof(Vertex, bitangent)}
                                             });

        m_pVertexInfo.bindingDescription = vk::getBindingDescription<Vertex>();

        vk::createGraphicsPipeline(m_pVkData, m_pRenderPipelines.back(), m_pVertexInfo);
        vk::createFramebuffers(m_pVkData, m_pRenderPipelines.back());

        m_pCommandPools.emplace_back();

        vk::createCommandPool(m_pVkData, m_pCommandPools.back());

        vk::createCommandBuffers(m_pVkData, m_pCommandPools.back(), m_pCommandBuffers);
        vk::createSyncObjects(m_pVkData);

        allocateBuffer();
        
    }

    vk::VulkanBuffer* VulkanRenderer::allocateBuffer() {

        auto *buffer = &m_pVertexBuffers.emplace_back();

        buffer->bufferInfo = vk::createVertexBuffer(m_pVkData, buffer->buffer, MAX_VERTICES_PER_BUFFER * sizeof(Vertex));
        buffer->deviceMemory = vk::allocateMemory(m_pVkData, buffer->buffer);
        
        IO::Log("allocating ", MAX_VERTICES_PER_BUFFER * sizeof(Vertex), " bytes");

        return buffer;
    }

    uint32_t VulkanRenderer::Submit(const std::vector<Vertex> &vertices, const MaterialInfo &)
    {
        vk::VulkanBuffer* buffer = &m_pVertexBuffers.back();

        if(buffer->allocatedVertices >= MAX_VERTICES_PER_BUFFER) 
        {
            buffer = allocateBuffer();
        }

        IO::Log("mapping ", vertices.size(),  " vertices, of size ",vertices.size() * sizeof(Vertex), " bytes, at offset ", current_vertex_count * sizeof(Vertex));
        
        vk::mapMemory(m_pVkData, buffer->deviceMemory, vertices.size() * sizeof(Vertex), current_vertex_count* sizeof(Vertex), vertices.data());
        current_vertex_count += vertices.size();
        buffer->allocatedVertices += vertices.size();
        return m_pVertexBuffers.size() - 1;
    }

    void VulkanRenderer::Push(uint32_t object_id) {
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

        //for (auto id: m_pCurrentFrameObjects) {
        vk::bindVertexBuffer(m_pVkData, m_pCommandBuffers[m_pCurrentFrame], m_pVertexBuffers[0].buffer);
        vk::draw(m_pCommandBuffers[m_pCurrentFrame], current_vertex_count, 1, 0, 0);
        //}
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

        vk::destroyGraphicsPipeline(m_pVkData, m_pRenderPipelines[0]);
        vk::cleanupRenderPass(m_pVkData, m_pRenderPipelines[0].renderPass);
        vk::cleanup(m_pVkData);
    }
}