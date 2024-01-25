#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <cstring>
#include <set>

#include <vulkan/vulkan.h>

#include "extensions.hpp"
#include "vk_data.hpp"

#include "validation_layers.hpp"

namespace vk
{


    static void createInstance(const char *appName, VulkanData &vk_data)
    {
        if (enableValidationLayers && !checkValidationLayerSupport())
        {
            throw std::runtime_error("validation layers requested, but not available!");
        }
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Android test";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Epsilon Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 1, 0);

        appInfo.apiVersion = VK_API_VERSION_1_0;
#if defined(__ANDROID__)
        appInfo.apiVersion = VK_API_VERSION_1_1;
#endif


        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions;

#if USE_GLFW

        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
#endif
        auto extensions = getRequiredExtensions();
#if !defined(__ANDROID__)
        extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
        //extensions.push_back(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);
#endif
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        if constexpr (enableValidationLayers)
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

        VkResult result = vkCreateInstance(&createInfo, nullptr, &vk_data.instance);

        if (vkCreateInstance(&createInfo, nullptr, &vk_data.instance) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create instance!");
        }

        // checkExtensions();
    }

}