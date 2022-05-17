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

#include "vk_data.hpp"

namespace LearningVulkan
{
    class LearningVulkanApplication
    {

        uint32_t m_CurrentFrame = 0;
        Window m_Window;
        std::string m_ApplicationName = "Default";

    public:
        LearningVulkanApplication() = default;

        LearningVulkanApplication(std::string appName) : m_ApplicationName(appName) {}

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

        vk::vk_data_t m_VkData;
        std::vector<VkCommandBuffer> m_CommandBuffers;

        bool mShouldClose = false;
        void ShouldClose()
        {
            mShouldClose = true;
        }


    private:
        void initWindow()
        {
            m_Window.init(m_ApplicationName, 1280, 720);
        }

        void initVulkan()
        {
            vk::createInstance(m_ApplicationName, m_VkData);
            vk::setupDebugMessenger(m_VkData.instance);
            vk::createSurface(m_VkData, m_Window.getWindow());
            vk::pickPhysicalDevice(m_VkData);
            vk::createLogicalDevice(m_VkData);
            vk::createSwapChain(m_VkData, m_Window.getWindow());
            vk::createImageViews(m_VkData);
            vk::createRenderPass(m_VkData);
            vk::createGraphicsPipeline(m_VkData);
            vk::createFramebuffers(m_VkData);
            vk::createCommandPool(m_VkData);

            vk::createCommandBuffers(m_VkData, m_CommandBuffers);
            vk::createSyncObjects(m_VkData);
        }

        void mainLoop()
        {
            while (!m_Window.ShouldClose())
            {
                if (mShouldClose)
                    break;
                onRender();
                drawFrame();
                m_Window.PollEvents();
            }

            vkDeviceWaitIdle(m_VkData.logicalDevice);
        }

        void drawFrame()
        {
            uint32_t imageIndex = prepareSyncObjects(m_VkData, m_Window.getWindow(), m_CurrentFrame);
            // vkAcquireNextImageKHR(vk::logicalDevice, vk::swapChain, UINT64_MAX, vk::imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
            vkResetCommandBuffer(m_CommandBuffers[m_CurrentFrame], 0);

            vk::recordCommandBuffer(m_CommandBuffers[m_CurrentFrame], imageIndex);
            vk::createRenderPassInfo(imageIndex, m_VkData);
            vk::beginRenderPass(m_CommandBuffers[m_CurrentFrame], m_VkData);
            vk::bindPipeline(m_VkData, m_CommandBuffers[m_CurrentFrame]);
            vk::draw(m_CommandBuffers[m_CurrentFrame], 6, 1, 0, 0);
            vk::endRenderPass(m_CommandBuffers[m_CurrentFrame], m_VkData);
            vk::endRecording(m_CommandBuffers[m_CurrentFrame]);

            VkSemaphore signalSemaphores[] = {m_VkData.renderFinishedSemaphores[m_CurrentFrame]};

            vk::Sync(m_VkData, m_CommandBuffers[m_CurrentFrame], m_CurrentFrame);

            vk::Present(m_VkData, signalSemaphores, imageIndex);

            m_CurrentFrame = (m_CurrentFrame + 1) % vk::MAX_FRAMES_IN_FLIGHT;
        }

        void exit()
        {
            onExit();
            vk::cleanup(m_VkData);
            m_Window.cleanup();
        }

    };
}