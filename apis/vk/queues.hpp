#pragma once

#include <optional>
#include <vector>

#if !defined(__ANDROID__)
#include <vulkan/vulkan.hpp>
#endif

#include "surface.hpp"
#include "vk_data.hpp"

namespace vk
{

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;
        std::optional<uint32_t> computeFamily;
        std::optional<uint32_t> transferFamily;

        int queueCount = 0;

        bool isComplete()
        {
            return graphicsFamily.has_value() && presentFamily.has_value() && computeFamily.has_value() && transferFamily.has_value();
        }
    };

    static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice & physicalDevice, const VulkanData& vk_data)
    {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

        uint32_t i = 0;
        for (const auto &queueFamily : queueFamilies)
        {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.graphicsFamily = i;
            }

            if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) {
                indices.computeFamily = i;
            }

            if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.transferFamily = i;
            }


            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, vk_data.surface, &presentSupport);
            if (presentSupport)
            {
                indices.presentFamily = i;
            }

            if (indices.isComplete())
            {
                indices.queueCount = queueFamily.queueCount;
                break;
            }

            i++;
        }

        return indices;
    }

}