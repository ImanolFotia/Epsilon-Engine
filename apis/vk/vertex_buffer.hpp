#pragma once

#include <array>

#if (BUILD_ANDROID == 0)
#include <vulkan/vulkan.hpp>
#endif
#include "core/engine/types.hpp"

namespace vk
{
    static VkVertexInputBindingDescription getBindingDescription(size_t size)
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = size;
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    static VkFormat resolveVertexType(enum engine::VertexFormat format) {

        if(format == engine::XY_UINT) return VK_FORMAT_R32G32_UINT;
        if(format == engine::XY_FLOAT) return VK_FORMAT_R32G32_SFLOAT;

        if(format == engine::XYZ_UINT) return VK_FORMAT_R32G32B32_UINT;
        if(format == engine::XYZ_FLOAT) return VK_FORMAT_R32G32B32_SFLOAT;

        if(format == engine::XYZW_UINT) return VK_FORMAT_R32G32B32A32_UINT;
        if(format == engine::XYZW_FLOAT) return VK_FORMAT_R32G32B32A32_SFLOAT;

        return VK_FORMAT_R32_SFLOAT;
    }

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions(uint32_t binding, std::vector<engine::VertexDescriptorInfo> vertexLayout)
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
        attributeDescriptions.resize(vertexLayout.size());
        uint32_t index = 0;
        for (auto &[format, offset] : vertexLayout)
        {
            attributeDescriptions[index].binding = binding;
            attributeDescriptions[index].location = index;
            attributeDescriptions[index].format = resolveVertexType(format);
            attributeDescriptions[index].offset = offset;
            index++;
        }
        return attributeDescriptions;
    }

    static VkBufferCreateInfo createVertexBuffer(VulkanData &vkData,
                                                 VkBuffer &vertexBuffer,
                                                 size_t size,
                                                 VkBufferUsageFlags usage)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(vkData.logicalDevice, &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create vertex buffer!");
        }

        return bufferInfo;
    }

    static void bindVertexBuffer(VulkanData &vkData, VkCommandBuffer &commandBuffer, VkBuffer &buffer)
    {
        VkBuffer vertexBuffers[] = {buffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    }

    static void copyBuffer(VulkanData &vkData, VkCommandPool &commandPool, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, size_t dstOffset)
    {

        VkCommandBuffer commandBuffer = beginSingleTimeCommands(vkData, commandPool);

        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0;         // Optional
        copyRegion.dstOffset = dstOffset; // Optional
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);


        endSingleTimeCommands(vkData, commandPool, commandBuffer);

    }

    static void destroyVertexBuffer(VulkanData &vkData, VkBuffer &buffer)
    {
        vkDestroyBuffer(vkData.logicalDevice, buffer, nullptr);
    }
}