#pragma once

#if !defined(__ANDROID__)
#include <vulkan/vulkan.hpp>
#endif
#include <cmath>

#include "vk_data.hpp"
#include <ktx.h>
#include "command.hpp"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#undef min
#undef max
#endif

namespace vk
{

    static VulkanTexture createImage(VulkanData &vkData, VulkanTextureInfo textureInfo)
    {
        VulkanTexture texture{};
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = static_cast<uint32_t>(textureInfo.width);
        imageInfo.extent.height = static_cast<uint32_t>(textureInfo.height);
        imageInfo.extent.depth = 1;

        imageInfo.mipLevels = textureInfo.mipLevels;
        imageInfo.arrayLayers = 1;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

        imageInfo.format = textureInfo.format;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = textureInfo.usage;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.flags = 0; // Optional


        if (vkCreateImage(vkData.logicalDevice, &imageInfo, nullptr, &texture.image) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create image!");
        }
        texture.imageInfo = imageInfo;

        return texture;
    }

    static void createTextureSampler(const VulkanData &vkData, VulkanTexture &texture)
    {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = texture.filter;
        samplerInfo.minFilter = texture.filter;
        samplerInfo.addressModeU = texture.addressMode;
        samplerInfo.addressModeV = texture.addressMode;
        samplerInfo.addressModeW = texture.addressMode;
        samplerInfo.anisotropyEnable = VK_TRUE;
        

        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(vkData.physicalDevice, &properties);
        samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;

        if (texture.compareEnable)
        {
            samplerInfo.compareEnable = VK_TRUE;
            samplerInfo.compareOp = texture.compareOp;
        }
        else
        {
            samplerInfo.compareEnable = VK_FALSE;
            samplerInfo.compareOp = texture.compareOp;
        }

        uint32_t mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texture.info.width, texture.info.height)))) + 1;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = (float)mipLevels;

        if (vkCreateSampler(vkData.logicalDevice, &samplerInfo, nullptr, &texture.sampler) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture sampler!");
        }
    }

    static void createImageView(const VulkanData &vkData, VulkanTexture &texture, VkImageAspectFlags aspectMask)
    {

        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = texture.image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = texture.format;
        viewInfo.subresourceRange.aspectMask = aspectMask;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = texture.info.mipLevels;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;
        

        if (vkCreateImageView(vkData.logicalDevice, &viewInfo, nullptr, &texture.imageView) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture image view!");
        }
    }

    static bool destroyImage(VulkanData &vkData, VulkanTexture &texture)
    {
        vkDestroyImage(vkData.logicalDevice, texture.image, nullptr);
        return true;
    }

    static void imageMemoryBarrier(VulkanData &vkData, VkCommandPool &commandPool, VkImage image, VkFormat format,
                                   VkImageLayout oldLayout, VkImageLayout newLayout, VulkanTextureInfo info, VkCommandBuffer commandBuffer,
                                   VkImageSubresourceRange subResourceRange, unsigned mipLevel = 0, unsigned mipCount = 1)
    {

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = mipLevel;
        barrier.subresourceRange.levelCount = mipCount;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        // barrier.subresourceRange.levelCount = mipLevels;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
                 newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL &&
                 newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
                 newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else
        {
            throw std::invalid_argument("unsupported layout transition!");
        }

        vkCmdPipelineBarrier(
            commandBuffer,
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier);
    }

    static void transitionImageLayout(VulkanData &vkData, VkCommandPool &commandPool, VkImage image, VkFormat format,
                                      VkImageLayout oldLayout, VkImageLayout newLayout, VulkanTextureInfo info, unsigned mipLevel = 0, unsigned mipCount = 1)
    {

        // int mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(info.width, info.height)))) + 1;
        VkCommandBuffer commandBuffer = beginSingleTimeCommands(vkData, commandPool);

        VkImageSubresourceRange subresourceRange = {};
        subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        subresourceRange.levelCount = mipCount;
        subresourceRange.layerCount = 1;

        imageMemoryBarrier(vkData, commandPool, image, format, oldLayout, newLayout, info, commandBuffer, subresourceRange, mipLevel, mipCount);

        endSingleTimeCommands(vkData, commandPool, commandBuffer);
    }

    static void
    copyBufferToImage(VulkanData &vkData, VkCommandPool &commandPool, VkBuffer& buffer, VkImage& image, uint32_t width,
                      uint32_t height, std::vector<size_t> offsets, uint32_t mipLevels)
    {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands(vkData, commandPool);

        std::vector<VkBufferImageCopy> bufferCopyRegions;
        if (offsets.size()  <= 1) {
            VkBufferImageCopy& region = bufferCopyRegions.emplace_back();
            region.bufferOffset = 0;
            region.bufferRowLength = 0;
            region.bufferImageHeight = 0;
            region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            region.imageSubresource.mipLevel = 0;
            region.imageSubresource.baseArrayLayer = 0;
            region.imageSubresource.layerCount = 1;
            region.imageOffset = { 0, 0, 0 };
            region.imageExtent = {
                width,
                height,
                1 };
        }
        else {
            for (uint32_t i = 0; i < mipLevels; i++)
            {

                VkBufferImageCopy bufferCopyRegion = {};
                bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                bufferCopyRegion.imageSubresource.mipLevel = i;
                bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
                bufferCopyRegion.imageSubresource.layerCount = 1;
                bufferCopyRegion.imageExtent.width = std::max(1u, width >> i);
                bufferCopyRegion.imageExtent.height = std::max(1u,  height >> i);
                bufferCopyRegion.imageExtent.depth = 1;
                bufferCopyRegion.bufferOffset = offsets.at(i);

                bufferCopyRegions.push_back(bufferCopyRegion);
            }
        }

        vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, bufferCopyRegions.size(), bufferCopyRegions.data());

        endSingleTimeCommands(vkData, commandPool, commandBuffer);
    }

    static bool hasStencilComponent(VkFormat format)
    {
        return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
    }

    static VkFormat
    findSupportedFormat(VulkanData &vkData, const std::vector<VkFormat> &candidates, VkImageTiling tiling,
                        VkFormatFeatureFlags features)
    {
        for (VkFormat format : candidates)
        {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(vkData.physicalDevice, format, &props);

            if (!hasStencilComponent(format))
                continue;

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
            {
                return format;
            }
            else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
            {
                return format;
            }
        }
        throw std::runtime_error("failed to find supported format!");
    }

    static VkFormat findDepthFormat(VulkanData &vkData)
    {
        return findSupportedFormat(vkData,
                                   {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
                                   VK_IMAGE_TILING_OPTIMAL,
                                   VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    }

}