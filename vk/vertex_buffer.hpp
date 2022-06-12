#pragma once

#include <array>

#include <vulkan/vulkan.hpp>

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

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions(uint32_t binding, std::initializer_list<std::pair<VkFormat, size_t>> vertexLayout)
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
        attributeDescriptions.resize(vertexLayout.size());
        uint32_t index = 0;
        for (auto &[format, offset] : vertexLayout)
        {
            attributeDescriptions[index].binding = binding;
            attributeDescriptions[index].location = index;
            attributeDescriptions[index].format = format;
            attributeDescriptions[index].offset = offset;
            index++;
        }
        return attributeDescriptions;
    }


    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions(uint32_t binding, std::vector<std::pair<VkFormat, size_t>> vertexLayout)
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
        attributeDescriptions.resize(vertexLayout.size());
        uint32_t index = 0;
        for (auto &[format, offset] : vertexLayout)
        {
            attributeDescriptions[index].binding = binding;
            attributeDescriptions[index].location = index;
            attributeDescriptions[index].format = format;
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