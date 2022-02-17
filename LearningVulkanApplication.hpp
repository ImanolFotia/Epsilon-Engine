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
            mainLoop();
            onExit();
        }

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
        }

        

        void mainLoop()
        {
            while (!mWindow.ShouldClose())
            {
                if(mShouldClose) break;
                Loop();
                mWindow.PollEvents();
            }
        }

        virtual void Loop() = 0;

        void onExit() {
            vk::cleanup();
            mWindow.cleanup();
        }

    private:
        Window mWindow;
        std::string mApplicationName = "Default";
    };
}