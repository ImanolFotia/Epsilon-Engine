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
#include "vk_aux/surface.hpp"

#include "vk_aux/device.hpp"
#include "vk_aux/swap_chain.hpp"
#include "vk_aux/validation_layers.hpp"

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
            createInstance();
            setupDebugMessenger(instance);
            createSurface(instance, mWindow.getWindow());
            physicalDevice = pickPhysicalDevice(instance);
            createLogicalDevice();
            createSwapChain(logicalDevice, physicalDevice, mWindow.getWindow());

            createImageViews(logicalDevice);
        }

        void createLogicalDevice()
        {
            QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

            std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
            std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

            float queuePriority = 1.0f;
            for (uint32_t queueFamily : uniqueQueueFamilies)
            {
                VkDeviceQueueCreateInfo queueCreateInfo{};
                queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queueCreateInfo.queueFamilyIndex = queueFamily;
                queueCreateInfo.queueCount = 1;
                queueCreateInfo.pQueuePriorities = &queuePriority;
                queueCreateInfos.push_back(queueCreateInfo);
            }

            VkPhysicalDeviceFeatures deviceFeatures{};

            VkDeviceCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            createInfo.pQueueCreateInfos = queueCreateInfos.data();
            createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());

            createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
            createInfo.ppEnabledExtensionNames = deviceExtensions.data();

            createInfo.pEnabledFeatures = &deviceFeatures;
            // createInfo.enabledExtensionCount = 0;

            if (enableValidationLayers)
            {
                createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
                createInfo.ppEnabledLayerNames = validationLayers.data();
            }
            else
            {
                createInfo.enabledLayerCount = 0;
            }

            if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create logical device!");
            }
            vkGetDeviceQueue(logicalDevice, indices.presentFamily.value(), 0, &presentQueue);
            //????????
            vkGetDeviceQueue(logicalDevice, indices.graphicsFamily.value(), 0, &graphicsQueue);
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

        void cleanup()
        {
            vkDestroySwapchainKHR(logicalDevice, swapChain, nullptr);

            for (auto imageView : swapChainImageViews)
            {
                vkDestroyImageView(logicalDevice, imageView, nullptr);
            }
            vkDeviceWaitIdle(logicalDevice);
            vkDestroyDevice(logicalDevice, nullptr);

            if (enableValidationLayers)
            {
                DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
            }

            cleanupSurface(instance);

            mWindow.cleanup();
        }

        void createInstance()
        {
            if (enableValidationLayers && !checkValidationLayerSupport())
            {
                throw std::runtime_error("validation layers requested, but not available!");
            }
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

            auto extensions = getRequiredExtensions();
            createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
            createInfo.ppEnabledExtensionNames = extensions.data();

            VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
            if (enableValidationLayers)
            {
                createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
                createInfo.ppEnabledLayerNames = validationLayers.data();

                populateDebugMessengerCreateInfo(debugCreateInfo);
                createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
            }
            else
            {
                createInfo.enabledLayerCount = 0;
            }

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

            std::cout << std::endl;
        }

        std::vector<const char *> getRequiredExtensions()
        {
            uint32_t glfwExtensionCount = 0;
            const char **glfwExtensions;
            glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

            std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

            if (enableValidationLayers)
            {
                extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            }

            return extensions;
        }

    private:
        Window mWindow;
        std::string mApplicationName = "Default";
        VkInstance instance;
        VkDevice logicalDevice;
        VkPhysicalDevice physicalDevice;
        VkQueue graphicsQueue;
    };
}