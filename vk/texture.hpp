#pragma once

#include <vulkan/vulkan.hpp>

#include "vk_data.hpp"

namespace vk
{
    static VulkanTextureBuffer createImage(VulkanData &vkData, VulkanTextureInfo textureInfo)
    {
        VulkanTextureBuffer textureBuffer{};
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = static_cast<uint32_t>(textureInfo.width);
        imageInfo.extent.height = static_cast<uint32_t>(textureInfo.height);
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.flags = 0; // Optional

        textureBuffer.imageInfo = imageInfo;

        if (vkCreateImage(vkData.logicalDevice, &imageInfo, nullptr, &textureBuffer.image) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create image!");
        }

        return textureBuffer;
    }

    static bool destroyImage(VulkanData& vkData, VulkanTextureBuffer& textureBuffer) {
        vkDestroyImage(vkData.logicalDevice, textureBuffer.image, nullptr);
    }
}