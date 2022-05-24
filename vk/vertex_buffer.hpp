#pragma once

#include <array>

#include <vulkan/vulkan.hpp>

namespace vk
{
    template <typename T>
    static VkVertexInputBindingDescription getBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(T);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    template <uint32_t num_descriptors>
    static std::array<VkVertexInputAttributeDescription, num_descriptors> getAttributeDescriptions(uint32_t binding, std::initializer_list<std::pair<VkFormat, size_t>> vertexLayout)
    {
        std::array<VkVertexInputAttributeDescription, num_descriptors> attributeDescriptions{};
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

    static VkBufferCreateInfo createVertexBuffer(VulkanData &vkData, VkBuffer &vertexBuffer, size_t size)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
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

    static void destroyVertexBuffer(VulkanData &vkData, VkBuffer &buffer)
    {
        vkDestroyBuffer(vkData.logicalDevice, buffer, nullptr);
    }
}