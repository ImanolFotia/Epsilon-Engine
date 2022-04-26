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
            createRenderPass();
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
            vk::createGraphicsPipeline(vk::logicalDevice);
        }

        void createRenderPass()
        {
            VkAttachmentDescription colorAttachment{};
            colorAttachment.format = swapChainImageFormat;
            colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
            colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        }

        void mainLoop()
        {
            while (!mWindow.ShouldClose())
            {
                if (mShouldClose)
                    break;
                onRender();
                mWindow.PollEvents();
            }
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