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
        vk::createRenderPass(m_pVkData);
        vk::createGraphicsPipeline(m_pVkData);
        vk::createFramebuffers(m_pVkData);
        vk::createCommandPool(m_pVkData, m_pCommandPool);

        vk::createCommandBuffers(m_pVkData, m_pCommandPool, m_pCommandBuffers);
        vk::createSyncObjects(m_pVkData);
    }

    void VulkanRenderer::Begin()
    {
    }

    void VulkanRenderer::End()
    {
    }

    void VulkanRenderer::Flush()
    {
        uint32_t imageIndex = vk::prepareSyncObjects(m_pVkData, m_pWindow->getWindow(), m_pCurrentFrame);
        // vkAcquireNextImageKHR(vk::logicalDevice, vk::swapChain, UINT64_MAX, vk::imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
        vkResetCommandBuffer(m_pCommandBuffers[m_pCurrentFrame], 0);

        vk::recordCommandBuffer(m_pCommandBuffers[m_pCurrentFrame], imageIndex);
        vk::createRenderPassInfo(imageIndex, m_pVkData);
        vk::beginRenderPass(m_pCommandBuffers[m_pCurrentFrame], m_pVkData);
        vk::bindPipeline(m_pVkData, m_pCommandBuffers[m_pCurrentFrame]);
        vk::draw(m_pCommandBuffers[m_pCurrentFrame], 6, 1, 0, 0);
        vk::endRenderPass(m_pCommandBuffers[m_pCurrentFrame], m_pVkData);
        vk::endRecording(m_pCommandBuffers[m_pCurrentFrame]);

        VkSemaphore signalSemaphores[] = {m_pVkData.renderFinishedSemaphores[m_pCurrentFrame]};

        vk::Sync(m_pVkData, m_pCommandBuffers[m_pCurrentFrame], m_pCurrentFrame);

        vk::Present(m_pVkData, signalSemaphores, imageIndex);

        m_pCurrentFrame = (m_pCurrentFrame + 1) % vk::MAX_FRAMES_IN_FLIGHT;
    }

    void VulkanRenderer::Cleanup()
    {
        vkDeviceWaitIdle(m_pVkData.logicalDevice);
        vk::cleanupSyncObjects(m_pVkData);
        vk::cleanCommandPool(m_pVkData, m_pCommandPool);
        vk::cleanup(m_pVkData);
    }
}