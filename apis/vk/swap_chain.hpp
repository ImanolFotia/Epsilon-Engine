#pragma once

#include <limits>
#include <cstddef>
#include <vector>
#include <cstdint>   // Necessary for UINT32_MAX
#include <algorithm> // Necessary for std::clamp

#if (BUILD_ANDROID == 0)
#include <vulkan/vulkan.hpp>
#include "GLFW/glfw3.h"
#endif

#include "surface.hpp"
#include "render_pass.hpp"
#include "pipeline.hpp"
#include "framebuffer.hpp"
#include "texture.hpp"
#include "memory.hpp"

#include "vk_data.hpp"

#ifdef _WIN32
#undef min
#undef max
#endif

namespace vk
{
    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice &physicalDevice, const VulkanData &vk_data)
    {
        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, vk_data.surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, vk_data.surface, &formatCount, nullptr);

        if (formatCount != 0)
        {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, vk_data.surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, vk_data.surface, &presentModeCount, nullptr);

        if (presentModeCount != 0)
        {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, vk_data.surface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    static VkSurfaceFormatKHR chooseSwapSurfaceFormat(VulkanData &vk_data, const std::vector<VkSurfaceFormatKHR> &availableFormats)
    {
        for (const auto &availableFormat : availableFormats)
        {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(vk_data.physicalDevice, availableFormat.format, &props);
            VkFormatFeatureFlags features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
            // if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            if ((props.optimalTilingFeatures & features) == features)
            {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes)
    {
        for (const auto &availablePresentMode : availablePresentModes)
        {
            if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
            {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    static VkExtent2D chooseSwapExtent(framework::Window::windowType *window, const VkSurfaceCapabilitiesKHR &capabilities)
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {

            std::cout << "swapchain extent size:\n x: " << capabilities.currentExtent.width << "\n y: " << capabilities.currentExtent.height << std::endl;
            return capabilities.currentExtent;
        }
        else
        {
            int width, height;

#ifdef USE_GLFW
            glfwGetFramebufferSize(window, &width, &height);
#endif

            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)};

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            std::cout << "swapchain extent size:\n x: " << actualExtent.width << "\n y: " << actualExtent.height << std::endl;
            return actualExtent;
        }
    }

    static void createSwapChain(VulkanData &vk_data, framework::Window::windowType *window)
    {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(vk_data.physicalDevice, vk_data);

        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(vk_data, swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = chooseSwapExtent(window, swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
        {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }
        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = vk_data.surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        QueueFamilyIndices indices = findQueueFamilies(vk_data.physicalDevice, vk_data);
        uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        if (indices.graphicsFamily != indices.presentFamily)
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;     // Optional
            createInfo.pQueueFamilyIndices = nullptr; // Optional
        }
        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(vk_data.logicalDevice, &createInfo, nullptr, &vk_data.swapChain) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create swap chain!");
        }
        // vkGetSwapchainImagesKHR(vk_data.logicalDevice, vk_data.swapChain, &imageCount, nullptr);
        vk_data.defaultRenderPass.renderPassChain.Images.clear();
        vk_data.defaultRenderPass.renderPassChain.Images.resize(imageCount);
        vkGetSwapchainImagesKHR(vk_data.logicalDevice, vk_data.swapChain, &imageCount, vk_data.defaultRenderPass.renderPassChain.Images.data());

        vk_data.defaultRenderPass.renderPassChain.ImageFormats.push_back(surfaceFormat.format);
        vk_data.defaultRenderPass.renderPassChain.Extent = extent;
        vk_data.swapChainWidth = extent.width;
        vk_data.swapChainHeight = extent.height;
    }

    static void createImageViews(VulkanData &vk_data)
    {
        vk_data.defaultRenderPass.renderPassChain.ImageViews.clear();
        vk_data.defaultRenderPass.renderPassChain.ImageViews.resize(vk_data.defaultRenderPass.renderPassChain.Images.size());

        vk_data.defaultRenderPass.renderPassChain.DepthTextureInfo.format = findDepthFormat(vk_data);
        vk_data.defaultRenderPass.renderPassChain.DepthTextureInfo.width = vk_data.defaultRenderPass.renderPassChain.Extent.width;
        vk_data.defaultRenderPass.renderPassChain.DepthTextureInfo.height = vk_data.defaultRenderPass.renderPassChain.Extent.height;
        vk_data.defaultRenderPass.renderPassChain.DepthTextureInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

        vk_data.defaultRenderPass.renderPassChain.DepthTexture = createImage(vk_data, vk_data.defaultRenderPass.renderPassChain.DepthTextureInfo);
        vk_data.defaultRenderPass.renderPassChain.DepthTexture.format = findDepthFormat(vk_data);
        vk_data.defaultRenderPass.renderPassChain.DepthTextureBuffer.deviceMemory = allocateTextureMemory(vk_data, vk_data.defaultRenderPass.renderPassChain.DepthTexture, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        createImageView(vk_data, vk_data.defaultRenderPass.renderPassChain.DepthTexture, VK_IMAGE_ASPECT_DEPTH_BIT);

        for (size_t i = 0; i < vk_data.defaultRenderPass.renderPassChain.Images.size(); i++)
        {

            VulkanTexture texture;
            texture.format = vk_data.defaultRenderPass.renderPassChain.ImageFormats[0];
            texture.image = vk_data.defaultRenderPass.renderPassChain.Images[i];

            createImageView(vk_data, texture, VK_IMAGE_ASPECT_COLOR_BIT);

            vk_data.defaultRenderPass.renderPassChain.ImageViews[i] = texture.imageView;
        }
    }

    static void cleanupSwapChain(const VulkanData &vk_data)
    {
        for (size_t i = 0; i < vk_data.defaultRenderPass.renderPassChain.Framebuffers.size(); i++)
        {
            vkDestroyFramebuffer(vk_data.logicalDevice, vk_data.defaultRenderPass.renderPassChain.Framebuffers[i], nullptr);
        }

        for (size_t i = 0; i < vk_data.defaultRenderPass.renderPassChain.ImageViews.size(); i++)
        {
            vkDestroyImageView(vk_data.logicalDevice, vk_data.defaultRenderPass.renderPassChain.ImageViews[i], nullptr);
        }

        vkDestroySwapchainKHR(vk_data.logicalDevice, vk_data.swapChain, nullptr);
    }
    /*
        static void cleanup(const VulkanData &vk_data, VulkanRenderPass &renderPass)
        {
            cleanupSwapChain(vk_data);

            for (auto &renderPipeline : renderPass.renderPipelines)
            {
                vkDestroyPipeline(vk_data.logicalDevice, renderPipeline.graphicsPipeline, nullptr);

                for (auto &layout : renderPipeline.pipelineLayout)
                    vkDestroyPipelineLayout(vk_data.logicalDevice, layout, nullptr);
            }

            vkDestroyRenderPass(vk_data.logicalDevice, renderPass.renderPass, nullptr);

            vkDestroyImageView(vk_data.logicalDevice, vk_data.defaultRenderPass.renderPassChain.DepthTexture.imageView, nullptr);
            vkDestroyImage(vk_data.logicalDevice, vk_data.defaultRenderPass.renderPassChain.DepthTexture.image, nullptr);
            vkFreeMemory(vk_data.logicalDevice, vk_data.defaultRenderPass.renderPassChain.DepthTextureBuffer.deviceMemory, nullptr);
        }*/

}