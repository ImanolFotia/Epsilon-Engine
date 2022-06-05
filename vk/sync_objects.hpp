#pragma once

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include <stdexcept>
#include "vk_data.hpp"

namespace vk
{

    static uint32_t MAX_FRAMES_IN_FLIGHT = 3;
    static void createSyncObjects(VulkanData &vk_data)
    {

        vk_data.syncObjects.resize(MAX_FRAMES_IN_FLIGHT);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            if (vkCreateSemaphore(vk_data.logicalDevice, &semaphoreInfo, nullptr, &vk_data.syncObjects[i].imageAvailableSemaphores) != VK_SUCCESS ||
                vkCreateSemaphore(vk_data.logicalDevice, &semaphoreInfo, nullptr, &vk_data.syncObjects[i].renderFinishedSemaphores) != VK_SUCCESS ||
                vkCreateFence(vk_data.logicalDevice, &fenceInfo, nullptr, &vk_data.syncObjects[i].inFlightFences) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
        }
    }

    template <typename T, uint32_t C>
    static int32_t prepareSyncObjects(VulkanData &vk_data, GLFWwindow *window, uint32_t currentFrame, VulkanRenderPass &renderPass, VulkanVertexInfo<C> vertexInfo)
    {

        vkWaitForFences(vk_data.logicalDevice, 1, &vk_data.syncObjects[currentFrame].inFlightFences, VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(vk_data.logicalDevice, vk_data.swapChain, UINT64_MAX, vk_data.syncObjects[currentFrame].imageAvailableSemaphores, VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {   
            recreateSwapChain<T>(vk_data, window, renderPass, vertexInfo);
            return -1;
            // return imageIndex;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        // Only reset the fence if we are submitting work
        vkResetFences(vk_data.logicalDevice, 1, &vk_data.syncObjects[currentFrame].inFlightFences);

        return imageIndex;
    }

    static void Sync(const VulkanData &vk_data, VkCommandBuffer &commandBuffer, uint32_t currentFrame)
    {

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {vk_data.syncObjects[currentFrame].imageAvailableSemaphores};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        VkSemaphore signalSemaphores[] = {vk_data.syncObjects[currentFrame].renderFinishedSemaphores};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(vk_data.graphicsQueue, 1, &submitInfo, vk_data.syncObjects[currentFrame].inFlightFences) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to submit draw command buffer!");
        }
    }

    static bool Present(const VulkanData &vk_data, VkSemaphore *signalSemaphores, uint32_t imageIndex)
    {
        // PResentation
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {vk_data.swapChain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;

        presentInfo.pResults = nullptr; // Optional

        auto result = vkQueuePresentKHR(vk_data.presentQueue, &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            return true;
        }
        else if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to present swap chain image!");
        }

        return false;
    }

    static void cleanupSyncObjects(const VulkanData &vk_data)
    {

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            vkDestroySemaphore(vk_data.logicalDevice, vk_data.syncObjects[i].renderFinishedSemaphores, nullptr);
            vkDestroySemaphore(vk_data.logicalDevice, vk_data.syncObjects[i].imageAvailableSemaphores, nullptr);
            vkDestroyFence(vk_data.logicalDevice, vk_data.syncObjects[i].inFlightFences, nullptr);
        }
    }
}