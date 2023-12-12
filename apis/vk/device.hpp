#pragma once

#include <vector>
#include <stdexcept>

#if !defined(__ANDROID__)
#include <vulkan/vulkan.hpp>
#endif

#include "core/framework/common.hpp"
#include "queues.hpp"
#include "swap_chain.hpp"
// #include "validation_layers.hpp"
#include "pipeline.hpp"
#include "instance.hpp"
#include "command.hpp"
#include "sync_objects.hpp"
#include "vk_data.hpp"

#if defined(__linux__) && (!defined(ANDROID) && !defined(__ANDROID__))
#include <bits/stdc++.h>
#endif

#if defined(ANDROID) || defined(__ANDROID__)

#include <android/log.h>

#define LOG_TAG "Epsilon"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#endif

namespace vk
{
    static void createLogicalDevice(VulkanData &vk_data)
    {
        QueueFamilyIndices indices = findQueueFamilies(vk_data.physicalDevice, vk_data);

        vk_data.uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value(), indices.computeFamily.value(), indices.transferFamily.value()};

        std::vector<float> queuePriority;
        queuePriority.resize(indices.queueCount);
        std::fill(queuePriority.begin(), queuePriority.end(), 1.0f); // [3] = { 1.0f , 1.0f, 1.0f };
        /*for (uint32_t queueFamily : vk_data.uniqueQueueFamilies)
        {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }*/

        //!!! TODO: Should ask where these extensions are avaliable
        VkPhysicalDeviceFeatures deviceFeatures{};

#if !defined(__ANDROID__)
        deviceFeatures.samplerAnisotropy = VK_TRUE;
        deviceFeatures.multiDrawIndirect = VK_TRUE;
        deviceFeatures.shaderSampledImageArrayDynamicIndexing = VK_TRUE;
        VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures layoutFeatures{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SEPARATE_DEPTH_STENCIL_LAYOUTS_FEATURES};
        VkPhysicalDeviceDescriptorIndexingFeatures indexing_features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT, nullptr};
        VkPhysicalDeviceSynchronization2Features sync_features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES, nullptr, VK_TRUE};
        VkPhysicalDeviceMemoryProperties memory_properties{};
        VkPhysicalDeviceFeatures2 device_features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, &indexing_features};
        VkPhysicalDeviceFeatures2 device_features2{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, &layoutFeatures};

        vkGetPhysicalDeviceFeatures2(vk_data.physicalDevice, &device_features);
        vkGetPhysicalDeviceFeatures2(vk_data.physicalDevice, &device_features2);

        vkGetPhysicalDeviceMemoryProperties(vk_data.physicalDevice, &memory_properties);

        vk_data.max_memory_heaps = memory_properties.memoryHeapCount;

        bool bindless_supported = indexing_features.descriptorBindingPartiallyBound && indexing_features.runtimeDescriptorArray;
        bool separateStencilSupported = layoutFeatures.separateDepthStencilLayouts;

        VkPhysicalDeviceFeatures2 physical_features2 = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2};
        vkGetPhysicalDeviceFeatures2(vk_data.physicalDevice, &physical_features2);

        if (separateStencilSupported)
        {
            indexing_features.pNext = &layoutFeatures;
            IO::Log("Separate Stencil is supported");
        }
        if (bindless_supported)
        {
            IO::Log("bindless is supported");
            indexing_features.descriptorBindingPartiallyBound = VK_TRUE;
            indexing_features.descriptorBindingSampledImageUpdateAfterBind = VK_TRUE;
            indexing_features.runtimeDescriptorArray = VK_TRUE;
            indexing_features.pNext = &sync_features;

            physical_features2.pNext = &indexing_features;
            vk_data.bindless_supported = true;
        }
        else
        {
            IO::Log("bindless is not supported");
        }

#endif
        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_data.physicalDevice, vk_data.surface, &surfaceCapabilities);

        MIN_FRAMES_IN_FLIGHT = surfaceCapabilities.minImageCount;
        MAX_FRAMES_IN_FLIGHT = std::max(MIN_FRAMES_IN_FLIGHT, MAX_FRAMES_IN_FLIGHT);

        IO::Log("Min frames in flight: ", MIN_FRAMES_IN_FLIGHT);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfo{};
        queueCreateInfo.resize(indices.queueCount);
        for (int i = 0; i < indices.queueCount; i++)
        {
            queueCreateInfo[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo[i].queueFamilyIndex = indices.transferFamily.value();
            queueCreateInfo[i].queueCount = indices.queueCount; // MAX_FRAMES_IN_FLIGHT;
            queueCreateInfo[i].pQueuePriorities = queuePriority.data();
        }

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = queueCreateInfo.data();
        createInfo.queueCreateInfoCount = 1;

        createInfo.enabledExtensionCount = static_cast<uint32_t>(vk_data.deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = vk_data.deviceExtensions.data();

        IO::Log("Required extensions\n");
        for (const auto &extension : vk_data.deviceExtensions)
        {
            IO::Log(extension);
        }
        IO::Log("\n");

#if !defined(__ANDROID__)
        createInfo.pNext = &physical_features2;
#else
        createInfo.pEnabledFeatures = &deviceFeatures;
#endif
        //   createInfo.enabledExtensionCount = 0;

        if (enableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }
        else
        {
            createInfo.enabledLayerCount = 0;
        }

        if (auto deviceResult = vkCreateDevice(vk_data.physicalDevice, &createInfo, nullptr, &vk_data.logicalDevice); deviceResult != VK_SUCCESS)
        {
            IO::Error("Error creating the device: ", deviceResult);
            throw std::runtime_error("failed to create logical device!");
        }

        vk_data.presentQueue.resize(MAX_FRAMES_IN_FLIGHT);
        vk_data.graphicsQueue.resize(MAX_FRAMES_IN_FLIGHT);
        vk_data.computeQueue.resize(MAX_FRAMES_IN_FLIGHT);

        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {

            vkGetDeviceQueue(vk_data.logicalDevice, indices.presentFamily.value(), 0, &vk_data.presentQueue[i]);
            vkGetDeviceQueue(vk_data.logicalDevice, indices.graphicsFamily.value(), 0, &vk_data.graphicsQueue[i]);
            vkGetDeviceQueue(vk_data.logicalDevice, indices.computeFamily.value(), 0, &vk_data.computeQueue[i]);
        }
        vkGetDeviceQueue(vk_data.logicalDevice, indices.transferFamily.value(), indices.queueCount > 1 ? 1 : 0, &vk_data.transferQueue);
    }

    static VkSampleCountFlagBits getMaxUsableSampleCount(VkPhysicalDevice physicalDevice)
    {
        VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
        std::cout << "max indirected draws: " << physicalDeviceProperties.limits.maxDrawIndirectCount << "\n";

        VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
        if (counts & VK_SAMPLE_COUNT_64_BIT)
        {
            return VK_SAMPLE_COUNT_64_BIT;
        }
        if (counts & VK_SAMPLE_COUNT_32_BIT)
        {
            return VK_SAMPLE_COUNT_32_BIT;
        }
        if (counts & VK_SAMPLE_COUNT_16_BIT)
        {
            return VK_SAMPLE_COUNT_16_BIT;
        }
        if (counts & VK_SAMPLE_COUNT_8_BIT)
        {
            return VK_SAMPLE_COUNT_8_BIT;
        }
        if (counts & VK_SAMPLE_COUNT_4_BIT)
        {
            return VK_SAMPLE_COUNT_4_BIT;
        }
        if (counts & VK_SAMPLE_COUNT_2_BIT)
        {
            return VK_SAMPLE_COUNT_2_BIT;
        }

        return VK_SAMPLE_COUNT_1_BIT;
    }

    static void showDeviceFeatures(VkPhysicalDevice device)
    {

        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
        IO::Log("Selected device: ", deviceProperties.deviceName);
        IO::Log("\tApi version: ", deviceProperties.apiVersion);
        IO::Log("\tID: ", deviceProperties.deviceID);
        IO::Log("\tDriver version: ", deviceProperties.driverVersion);
        IO::Log("\tVendor ID: ", deviceProperties.vendorID);
        IO::Log("\tIs discrete device: ", (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? "yes" : "no"));
    }

    static bool checkDeviceExtensionSupport(VkPhysicalDevice device, const VulkanData &vk_data)
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

    static bool isDeviceSuitable(VkPhysicalDevice device, const VulkanData &vk_data)
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

        VkPhysicalDeviceFeatures supportedFeatures;
        vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

        return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
    }

    static VkPhysicalDevice pickPhysicalDevice(VulkanData &vk_data)
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
        VkPhysicalDevice candidate;
        bool found = false;
        for (const auto &device : devices)
        {
            if (isDeviceSuitable(device, vk_data))
            {

                vk_data.msaaSamples = getMaxUsableSampleCount(device);
                std::cout << "max samples: " << vk_data.msaaSamples << std::endl;
                VkPhysicalDeviceProperties deviceProperties;
                vkGetPhysicalDeviceProperties(device, &deviceProperties);
                std::string deviceName = deviceProperties.deviceName;
                std::transform(deviceName.begin(), deviceName.end(), deviceName.begin(), ::toupper);
                // if (deviceName.find("INTEL") != std::string::npos)

                if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                {
                    physicalDevice = device;
                    std::cout << "found discrete gpu" << std::endl;
                    found = true;
                    break;
                }
            }
            candidate = device;
        }

        if (!found)
            physicalDevice = candidate;

        if (physicalDevice == VK_NULL_HANDLE)
        {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
        vk_data.physicalDevice = physicalDevice;

        return physicalDevice;
    }

    static void cleanup(VulkanData &vk_data)
    {
        vkDestroySwapchainKHR(vk_data.logicalDevice, vk_data.swapChain, nullptr);

        for (auto framebuffer : vk_data.defaultRenderPass.renderPassChain.Framebuffers)
        {
            vkDestroyFramebuffer(vk_data.logicalDevice, framebuffer, nullptr);
        }

        for (auto imageView : vk_data.defaultRenderPass.renderPassChain.ImageViews)
        {
            vkDestroyImageView(vk_data.logicalDevice, imageView, nullptr);
        }

        vkDestroyImageView(vk_data.logicalDevice, vk_data.defaultRenderPass.renderPassChain.DepthTexture.imageView, nullptr);
        vkDestroyImage(vk_data.logicalDevice, vk_data.defaultRenderPass.renderPassChain.DepthTexture.image, nullptr);
        vkFreeMemory(vk_data.logicalDevice, vk_data.defaultRenderPass.renderPassChain.DepthTextureBuffer.deviceMemory, nullptr);

        if (enableValidationLayers)
        {
            DestroyDebugUtilsMessengerEXT(vk_data.instance, debugMessenger, nullptr);
        }

        vkDestroyDevice(vk_data.logicalDevice, nullptr);

        cleanupSurface(vk_data);
    }
}