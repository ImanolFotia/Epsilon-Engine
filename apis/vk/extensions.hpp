#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <cstring>
#include <set>

#if (BUILD_ANDROID == 0)
#include <vulkan/vulkan.hpp>
#endif

namespace vk
{

    static void checkExtensions()
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
    }
}