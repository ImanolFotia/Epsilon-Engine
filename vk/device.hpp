#pragma once

#include <vector>
#include <stdexcept>

#include <vulkan/vulkan.hpp>

#include "../framework/common.hpp"
#include "queues.hpp"
#include "swap_chain.hpp"
#include "validation_layers.hpp"
#include "pipeline.hpp"
#include "instance.hpp"
#include "command.hpp"
#include "sync_objects.hpp"

namespace vk
{



    static void createLogicalDevice(engine::VulkanData& vk_data)
    {
        QueueFamilyIndices indices = findQueueFamilies(vk_data.physicalDevice, vk_data);

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

        createInfo.enabledExtensionCount = static_cast<uint32_t>(vk_data.deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = vk_data.deviceExtensions.data();

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

        if (vkCreateDevice(vk_data.physicalDevice, &createInfo, nullptr, &vk_data.logicalDevice) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create logical device!");
        }
        vkGetDeviceQueue(vk_data.logicalDevice, indices.presentFamily.value(), 0, &vk_data.presentQueue);
        //????????
        vkGetDeviceQueue(vk_data.logicalDevice, indices.graphicsFamily.value(), 0, &vk_data.graphicsQueue);
    }

    static void showDeviceFeatures(VkPhysicalDevice device)
    {

        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
        IO::Log("Device found: ", deviceProperties.deviceName);
        IO::Log("\tApi version: ", deviceProperties.apiVersion);
        IO::Log("\tID: ", deviceProperties.deviceID);
        IO::Log("\tDriver version: ", deviceProperties.driverVersion);
        IO::Log("\tVendor ID: ", deviceProperties.vendorID);
    }

    static bool checkDeviceExtensionSupport(VkPhysicalDevice device, const engine::VulkanData& vk_data)
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(vk_data.deviceExtensions.begin(), vk_data.deviceExtensions.end());

        for (const auto &extension : availableExtensions)
        {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    static bool isDeviceSuitable(VkPhysicalDevice device, const engine::VulkanData& vk_data)
    {
        showDeviceFeatures(device);

        bool extensionsSupported = checkDeviceExtensionSupport(device, vk_data);


        QueueFamilyIndices indices = findQueueFamilies(device, vk_data);

        bool swapChainAdequate = false;

        if (extensionsSupported)
        {
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device, vk_data);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        return indices.isComplete() && extensionsSupported && swapChainAdequate;
    }

    static VkPhysicalDevice pickPhysicalDevice(engine::VulkanData& vk_data)
    {
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(vk_data.instance, &deviceCount, nullptr);

        if (deviceCount == 0)
        {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(vk_data.instance, &deviceCount, devices.data());

        for (const auto &device : devices)
        {
            if (isDeviceSuitable(device, vk_data))
            {
                physicalDevice = device;
                break;
            }
        }

        if (physicalDevice == VK_NULL_HANDLE)
        {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
        vk_data.physicalDevice = physicalDevice;

        return physicalDevice;
    }

    static void cleanup(engine::VulkanData& vk_data)
    {



        vkDestroySwapchainKHR(vk_data.logicalDevice, vk_data.swapChain, nullptr);


        for (auto framebuffer : vk_data.swapChainFramebuffers)
        {
            vkDestroyFramebuffer(vk_data.logicalDevice, framebuffer, nullptr);
        }

        for (auto imageView : vk_data.swapChainImageViews)
        {
            vkDestroyImageView(vk_data.logicalDevice, imageView, nullptr);
        }

        if (enableValidationLayers)
        {
            DestroyDebugUtilsMessengerEXT(vk_data.instance, debugMessenger, nullptr);
        }


        vkDestroyDevice(vk_data.logicalDevice, nullptr);

        cleanupSurface(vk_data);
    }
}