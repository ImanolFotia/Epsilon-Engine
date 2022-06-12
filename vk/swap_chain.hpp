#pragma once

#include <limits>
#include <cstddef>
#include <vector>
#include <cstdint>   // Necessary for UINT32_MAX
#include <algorithm> // Necessary for std::clamp

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include "surface.hpp"
#include "render_pass.hpp"
#include "pipeline.hpp"
#include "framebuffer.hpp"
#include "texture.hpp"

#include "vk_data.hpp"

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

    static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats)
    {
        for (const auto &availableFormat : availableFormats)
        {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
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
            if (availablePresentMode == /*VK_PRESENT_MODE_MAILBOX_KHR*/VK_PRESENT_MODE_IMMEDIATE_KHR)
            {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    static VkExtent2D chooseSwapExtent(GLFWwindow *window, const VkSurfaceCapabilitiesKHR &capabilities)
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {

            std::cout << "swapchain extent size:\n x: " << capabilities.currentExtent.width << "\n y: " << capabilities.currentExtent.height << std::endl;
            return capabilities.currentExtent;
        }
        else
        {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);

            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)};

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            std::cout << "swapchain extent size:\n x: " << actualExtent.width << "\n y: " << actualExtent.height << std::endl;
            return actualExtent;
        }
    }

    static void createSwapChain(VulkanData &vk_data, GLFWwindow *window)
    {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(vk_data.physicalDevice, vk_data);

        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
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
        vkGetSwapchainImagesKHR(vk_data.logicalDevice, vk_data.swapChain, &imageCount, nullptr);
        vk_data.swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(vk_data.logicalDevice, vk_data.swapChain, &imageCount, vk_data.swapChainImages.data());
        vk_data.swapChainImageFormat = surfaceFormat.format;
        vk_data.swapChainExtent = extent;
    }

    static void createImageViews(VulkanData &vk_data)
    {

        vk_data.swapChainImageViews.resize(vk_data.swapChainImages.size());
        for (size_t i = 0; i < vk_data.swapChainImages.size(); i++)
        {

            VulkanTexture texture;
            texture.format = vk_data.swapChainImageFormat;
            texture.image = vk_data.swapChainImages[i] ;
            createImageView(vk_data, texture);
            vk_data.swapChainImageViews[i] = texture.imageView;
        }
    }

    static void cleanupSwapChain(const VulkanData &vk_data, VulkanRenderPass &renderPass)
    {
        for (size_t i = 0; i < vk_data.swapChainFramebuffers.size(); i++)
        {
            vkDestroyFramebuffer(vk_data.logicalDevice, vk_data.swapChainFramebuffers[i], nullptr);
        }

        for (auto &pipeline : renderPass.renderPipelines)
        {
            vkDestroyPipeline(vk_data.logicalDevice, pipeline.graphicsPipeline, nullptr);
            vkDestroyPipelineLayout(vk_data.logicalDevice, pipeline.pipelineLayout, nullptr);
        }
        vkDestroyRenderPass(vk_data.logicalDevice, renderPass.renderPass, nullptr);

        for (size_t i = 0; i < vk_data.swapChainImageViews.size(); i++)
        {
            vkDestroyImageView(vk_data.logicalDevice, vk_data.swapChainImageViews[i], nullptr);
        }

        vkDestroySwapchainKHR(vk_data.logicalDevice, vk_data.swapChain, nullptr);
    }

    template <typename T, uint32_t C>
    static void recreateSwapChain(VulkanData &vk_data, GLFWwindow *window, VulkanRenderPass &renderPass, VulkanVertexInfo<C> vertexInfo)
    {
        vkDeviceWaitIdle(vk_data.logicalDevice);
        cleanupSwapChain(vk_data, renderPass);

        vk::createSwapChain(vk_data, window);
        vk::createImageViews(vk_data);

        // for (auto &pipeline : renderPass.renderPipelines)
        {
            vk::createRenderPass(vk_data, renderPass);

            for (auto i = 0; i < renderPass.renderPipelines.size(); i++)
                vk::createGraphicsPipeline<T>(vk_data, renderPass, renderPass.renderPipelines[i], vertexInfo);

            vk::createFramebuffers(vk_data, renderPass);
        }
    }
}