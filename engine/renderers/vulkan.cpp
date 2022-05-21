#include <framework/window.hpp>

#include "vulkan.hpp"

namespace engine
{

    VulkanRenderer::VulkanRenderer() {

    }
    VulkanRenderer::~VulkanRenderer() {

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
        vk::createGraphicsPipeline(m_pVkData, m_pRenderPipelines[0]);
        vk::createFramebuffers(m_pVkData, m_pRenderPipelines[0]);

        m_pCommandPools.emplace_back();

        vk::createCommandPool(m_pVkData, m_pCommandPools[0]);
        vk::createCommandBuffers(m_pVkData, m_pCommandPools[0], m_pCommandBuffers);
        vk::createSyncObjects(m_pVkData);
    }

    void VulkanRenderer::Begin()
    {
        
        m_pImageIndex = vk::prepareSyncObjects(m_pVkData, m_pWindow->getWindow(), m_pCurrentFrame, m_pRenderPipelines[0]);
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
        vk::draw(m_pCommandBuffers[m_pCurrentFrame], 6, 1, 0, 0);
    }

    void VulkanRenderer::Cleanup()
    {
        vkDeviceWaitIdle(m_pVkData.logicalDevice);
        vk::cleanupSyncObjects(m_pVkData);
        vk::cleanCommandPool(m_pVkData, m_pCommandPools[0]);

        vk::destroyGraphicsPipeline(m_pVkData, m_pRenderPipelines[0]);
        vk::cleanupRenderPass(m_pVkData, m_pRenderPipelines[0].renderPass);
        vk::cleanup(m_pVkData);
    }
}