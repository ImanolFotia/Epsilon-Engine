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
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = commandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(vkData.logicalDevice, &allocInfo, &commandBuffer);
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);
        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0; // Optional
        copyRegion.dstOffset = dstOffset; // Optional
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(vkData.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(vkData.graphicsQueue);
        
        vkFreeCommandBuffers(vkData.logicalDevice, commandPool, 1, &commandBuffer);
    }

    static void destroyVertexBuffer(VulkanData &vkData, VkBuffer &buffer)
    {
        vkDestroyBuffer(vkData.logicalDevice, buffer, nullptr);
    }
}