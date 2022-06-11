#pragma once

#include <vulkan/vulkan.hpp>

#include "vk_data.hpp"

namespace vk
{
    static uint32_t findMemoryType(VulkanData &vkData, uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(vkData.physicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }
        throw std::runtime_error("failed to find suitable memory type!");
    }

    static VkDeviceMemory allocateMemory(VulkanData &vkData,
                                         VkBuffer buffer,
                                         VkMemoryPropertyFlags properties)
    {

        VkDeviceMemory vertexBufferMemory;

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(vkData.logicalDevice, buffer, &memRequirements);
        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(vkData, memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(vkData.logicalDevice, &allocInfo, nullptr, &vertexBufferMemory) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate vertex buffer memory!");
        }
        vkBindBufferMemory(vkData.logicalDevice, buffer, vertexBufferMemory, 0);

        return vertexBufferMemory;
    }

    static VkDeviceMemory allocateTextureMemory(VulkanData &vkData, VulkanTexture& textureBuffer)
    {
        VkDeviceMemory textureBufferMemory;
        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(vkData.logicalDevice, textureBuffer.image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(vkData, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        if (vkAllocateMemory(vkData.logicalDevice, &allocInfo, nullptr, &textureBufferMemory) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate image memory!");
        }

        vkBindImageMemory(vkData.logicalDevice, textureBuffer.image, textureBufferMemory, 0);

        return textureBufferMemory;
    }

    template <typename T>
    static void mapMemory(VulkanData &vkData, VkDeviceMemory &bufferMemory, size_t size, size_t offset, T *vertexData)
    {
        void *data;
        vkMapMemory(vkData.logicalDevice, bufferMemory, offset, size, 0, &data);
        memcpy(data, vertexData, size);
        vkUnmapMemory(vkData.logicalDevice, bufferMemory);
    }

    static void freeMemory(VulkanData &vkData, VkDeviceMemory &bufferMemory)
    {
        vkFreeMemory(vkData.logicalDevice, bufferMemory, nullptr);
    }
}