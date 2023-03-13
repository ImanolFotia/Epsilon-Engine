#pragma once

#include <vector>
#include <stdexcept>

#if (BUILD_ANDROID == 0)
#include <vulkan/vulkan.hpp>
#endif

#include "core/framework/common.hpp"
#include "queues.hpp"
#include "swap_chain.hpp"
#include "validation_layers.hpp"
#include "pipeline.hpp"
#include "instance.hpp"
#include "command.hpp"
#include "sync_objects.hpp"
#include "vk_data.hpp"

#if defined(__linux__) && (!defined(ANDROID) && !defined(__ANDROID__))
#include <bits/stdc++.h>
#endif

namespace vk
{
    static void createLogicalDevice(VulkanData &vk_data)
    {
        QueueFamilyIndices indices = findQueueFamilies(vk_data.physicalDevice, vk_data);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        vk_data.uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : vk_data.uniqueQueueFamilies)
        {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        //!!! TODO: Should ask where these extensions are avaliable
        VkPhysicalDeviceFeatures deviceFeatures{};
        deviceFeatures.samplerAnisotropy = VK_TRUE;
        deviceFeatures.multiDrawIndirect = VK_TRUE;
        deviceFeatures.shaderSampledImageArrayDynamicIndexing = VK_TRUE;
        VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures layoutFeatures{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SEPARATE_DEPTH_STENCIL_LAYOUTS_FEATURES};
        VkPhysicalDeviceDescriptorIndexingFeatures indexing_features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT, nullptr};
        VkPhysicalDeviceFeatures2 device_features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, &indexing_features};
        VkPhysicalDeviceFeatures2 device_features2{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, &layoutFeatures};

        vkGetPhysicalDeviceFeatures2(vk_data.physicalDevice, &device_features);
        vkGetPhysicalDeviceFeatures2(vk_data.physicalDevice, &device_features2);

        bool bindless_supported = indexing_features.descriptorBindingPartiallyBound && indexing_features.runtimeDescriptorArray;
        bool separateStencilSupported = layoutFeatures.separateDepthStencilLayouts;

        VkPhysicalDeviceFeatures2 physical_features2 = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2};
        vkGetPhysicalDeviceFeatures2(vk_data.physicalDevice, &physical_features2);

        if (separateStencilSupported)
        {
            indexing_features.pNext = &layoutFeatures;
        }
        if (bindless_supported)
        {
            indexing_features.descriptorBindingPartiallyBound = VK_TRUE;
            indexing_features.descriptorBindingSampledImageUpdateAfterBind = VK_TRUE;
            indexing_features.runtimeDescriptorArray = VK_TRUE;

            physical_features2.pNext = &indexing_features;
            vk_data.bindless_supported = true;
        }
        else
        {
            std::cout << "bindless is not supported" << std::endl;
        }

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());

        createInfo.enabledExtensionCount = static_cast<uint32_t>(vk_data.deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = vk_data.deviceExtensions.data();
        createInfo.pNext = &physical_features2;
        // createInfo.pEnabledFeatures = &deviceFeatures;
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

        if (vkCreateDevice(vk_data.physicalDevice, &createInfo, nullptr, &vk_data.logicalDevice) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create logical device!");
        }
        vkGetDeviceQueue(vk_data.logicalDevice, indices.presentFamily.value(), 0, &vk_data.presentQueue);
        //????????
        vkGetDeviceQueue(vk_data.logicalDevice, indices.graphicsFamily.value(), 0, &vk_data.graphicsQueue);
    }

    static VkSampleCountFlagBits getMaxUsableSampleCount(VkPhysicalDevice physicalDevice)
    {
        VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

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
                VkPhysicalDeviceProperties deviceProperties;
                vkGetPhysicalDeviceProperties(device, &deviceProperties);
                std::string deviceName = deviceProperties.deviceName;
                std::transform(deviceName.begin(), deviceName.end(), deviceName.begin(), ::toupper);
                // if (deviceName.find("NVIDIA") != std::string::npos)

                if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                {
                    physicalDevice = device;
                    std::cout << "found discrete gpu" << std::endl;
                    found = true;
                    break;
                }
                candidate = device;
            }
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