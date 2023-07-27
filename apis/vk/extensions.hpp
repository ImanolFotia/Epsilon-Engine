#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <cstring>
#include <set>

#include <vulkan/vulkan.h>

namespace vk
{

    static void checkExtensions()
    {
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
        IO::Log("available extensions:\n");

        for (const auto &extension : extensions)
        {
            IO::Log('\t', extension.extensionName, '\n');
        }

    }

    static std::vector<const char *> getRequiredExtensions()
    {
        uint32_t glfwExtensionCount = 0;
#if USE_GLFW
        const char **glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (enableValidationLayers)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }


        return extensions;
#endif

#if defined(__ANDROID__)
        std::vector<const char*> extensions;
            extensions.push_back("VK_KHR_surface");
            extensions.push_back("VK_KHR_android_surface");
            if (enableValidationLayers) {
                extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            }
            return extensions;

#endif
    }
}