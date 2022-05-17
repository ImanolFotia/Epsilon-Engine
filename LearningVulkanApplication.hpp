#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <cstring>
#include <set>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "framework/common.hpp"
#include "framework/def.hpp"
#include "framework/window.hpp"

#include "vk/vk.hpp"

namespace LearningVulkan
{
    class LearningVulkanApplication
    {
    public:
        LearningVulkanApplication() = default;

        LearningVulkanApplication(std::string appName) : mApplicationName(appName) {}

        void run()
        {
            initWindow();
            initVulkan();
            onCreate();
            mainLoop();
            exit();
        }

        virtual void onCreate() = 0;
        virtual void onRender() = 0;
        virtual void onExit() = 0;

    protected:
        bool mShouldClose = false;
        void ShouldClose()
        {
            mShouldClose = true;
        }

    private:
        void initWindow()
        {
            mWindow.init(mApplicationName, 1280, 720);
        }

        void initVulkan()
        {
            vk::createInstance(mApplicationName);
            vk::setupDebugMessenger(vk::instance);
            vk::createSurface(vk::instance, mWindow.getWindow());
            vk::physicalDevice = vk::pickPhysicalDevice(vk::instance);
            vk::createLogicalDevice();
            vk::createSwapChain(vk::logicalDevice, vk::physicalDevice, mWindow.getWindow());
            vk::createImageViews(vk::logicalDevice);
            vk::createRenderPass(vk::logicalDevice);
            vk::createGraphicsPipeline(vk::logicalDevice);
            vk::createFramebuffers(vk::logicalDevice);
            vk::createCommandPool(vk::physicalDevice, vk::logicalDevice);
        }

        void recreateSwapChain()
        {

            vkDeviceWaitIdle(vk::logicalDevice);

            cleanupSwapChain();

            vk::createSwapChain(vk::logicalDevice, vk::physicalDevice, mWindow.getWindow());
            vk::createImageViews(vk::logicalDevice);
            vk::createRenderPass(vk::logicalDevice);
            vk::createGraphicsPipeline(vk::logicalDevice);
            vk::createFramebuffers(vk::logicalDevice);
        }

        void cleanupSwapChain()
        {
            for (size_t i = 0; i < vk::swapChainFramebuffers.size(); i++)
            {
                vkDestroyFramebuffer(vk::logicalDevice, vk::swapChainFramebuffers[i], nullptr);
            }

            vkDestroyPipeline(vk::logicalDevice, vk::graphicsPipeline, nullptr);
            vkDestroyPipelineLayout(vk::logicalDevice, vk::pipelineLayout, nullptr);
            vkDestroyRenderPass(vk::logicalDevice, vk::myRenderPass, nullptr);

            for (size_t i = 0; i < vk::swapChainImageViews.size(); i++)
            {
                vkDestroyImageView(vk::logicalDevice, vk::swapChainImageViews[i], nullptr);
            }

            vkDestroySwapchainKHR(vk::logicalDevice, vk::swapChain, nullptr);
        }

        void mainLoop()
        {
            const uint32_t imageIndex = 0;

            auto commandBuffer = vk::createCommandBuffer(vk::logicalDevice);

            vk::createSyncObjects(vk::logicalDevice);

            while (!mWindow.ShouldClose())
            {
                if (mShouldClose)
                    break;
                onRender();
                drawFrame(commandBuffer);
                mWindow.PollEvents();

                // mShouldClose = true;
            }

            vkDeviceWaitIdle(vk::logicalDevice);
        }

        void drawFrame(const VkCommandBuffer &commandBuffer)
        {
            vkWaitForFences(vk::logicalDevice, 1, &vk::inFlightFence, VK_TRUE, UINT64_MAX);

            uint32_t imageIndex;
            VkResult result = vkAcquireNextImageKHR(vk::logicalDevice, vk::swapChain, UINT64_MAX, vk::imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

            if (result == VK_ERROR_OUT_OF_DATE_KHR)
            {
                recreateSwapChain();
                return;
            }
            else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
            {
                throw std::runtime_error("failed to acquire swap chain image!");
            }

            // Only reset the fence if we are submitting work
            vkResetFences(vk::logicalDevice, 1, &vk::inFlightFence);

            //vkAcquireNextImageKHR(vk::logicalDevice, vk::swapChain, UINT64_MAX, vk::imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
            vkResetCommandBuffer(commandBuffer, 0);

            vk::recordCommandBuffer(commandBuffer, imageIndex);
            vk::createRenderPassInfo(imageIndex);
            vk::beginRenderPass(commandBuffer);
            vk::bindPipeline(commandBuffer, vk::graphicsPipeline);
            vk::draw(commandBuffer, 3, 1, 0, 0);
            vk::endRenderPass(commandBuffer);
            vk::endRecording(commandBuffer);

            VkSubmitInfo submitInfo{};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

            VkSemaphore waitSemaphores[] = {vk::imageAvailableSemaphore};
            VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = waitSemaphores;
            submitInfo.pWaitDstStageMask = waitStages;

            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &commandBuffer;

            VkSemaphore signalSemaphores[] = {vk::renderFinishedSemaphore};
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = signalSemaphores;

            if (vkQueueSubmit(vk::graphicsQueue, 1, &submitInfo, vk::inFlightFence) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to submit draw command buffer!");
            }

            // PResentation
            VkPresentInfoKHR presentInfo{};
            presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores = signalSemaphores;

            VkSwapchainKHR swapChains[] = {vk::swapChain};
            presentInfo.swapchainCount = 1;
            presentInfo.pSwapchains = swapChains;
            presentInfo.pImageIndices = &imageIndex;

            presentInfo.pResults = nullptr; // Optional

            vkQueuePresentKHR(vk::presentQueue, &presentInfo);
        }

        void exit()
        {
            onExit();
            vk::cleanup();
            mWindow.cleanup();
        }

    private:
        Window mWindow;
        std::string mApplicationName = "Default";
    };
}