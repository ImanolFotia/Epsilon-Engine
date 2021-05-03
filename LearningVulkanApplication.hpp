#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <string>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "framework/common.hpp"

#include "framework/window.hpp"

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
            cleanup();
        }

    private:
        void initWindow()
        {
            mWindow.init(mApplicationName, 1280, 720);
        }

        void initVulkan()
        {
            createInstance();
        }

        void mainLoop()
        {
            mWindow.mainLoop();
        }

        void cleanup()
        {
            vkDestroyInstance(instance, nullptr);
            mWindow.cleanup();
        }

        void createInstance()
        {
            VkApplicationInfo appInfo{};
            appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            appInfo.pApplicationName = mApplicationName.c_str();
            appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
            appInfo.pEngineName = "No Engine";
            appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
            appInfo.apiVersion = VK_API_VERSION_1_0;

            VkInstanceCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            createInfo.pApplicationInfo = &appInfo;

            uint32_t glfwExtensionCount = 0;
            const char **glfwExtensions;

            glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

            createInfo.enabledExtensionCount = glfwExtensionCount;
            createInfo.ppEnabledExtensionNames = glfwExtensions;
            createInfo.enabledLayerCount = 0;

            VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

            if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create instance!");
            }
            
            checkExtensions();
        }

        void checkExtensions()
        {
            uint32_t extensionCount = 0;
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
            std::vector<VkExtensionProperties> extensions(extensionCount);
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
            std::cout << "available extensions:\n";

            for (const auto &extension : extensions)
            {
                std::cout << '\t' << extension.extensionName << '\n';
            }
        }

    private:
        Window mWindow;
        std::string mApplicationName = "Default";
        VkInstance instance;
    };
}