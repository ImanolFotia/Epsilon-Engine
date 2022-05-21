#pragma once

#include <vulkan/vulkan.hpp>

#include "render_pass.hpp"
#include "sync_objects.hpp"
#include "../engine/renderers/vk_data.hpp"

namespace vk
{

    static void createCommandPool(engine::VulkanData& vk_data, VkCommandPool& commandPool)
    {
        QueueFamilyIndices queueFamilyIndices = findQueueFamilies(vk_data.physicalDevice, vk_data);

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        if (vkCreateCommandPool(vk_data.logicalDevice, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create command pool!");
        }
    }

    static void createCommandBuffers(const engine::VulkanData& vk_data, VkCommandPool& commandPool, std::vector<VkCommandBuffer>& commandBuffers)
    {

        commandBuffers.resize(vk::MAX_FRAMES_IN_FLIGHT);
        VkCommandBuffer commandBuffer;
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = commandBuffers.size();

        if (vkAllocateCommandBuffers(vk_data.logicalDevice, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }

    static VkCommandBuffer recordCommandBuffer(const VkCommandBuffer& commandBuffer, uint32_t imageIndex)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;                  // Optional
        beginInfo.pInheritanceInfo = nullptr; // Optional

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        return commandBuffer;
    }

    static void endRecording(const VkCommandBuffer& commandBuffer) {
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    static void cleanCommandPool(const engine::VulkanData& vk_data, VkCommandPool& commandPool)
    {
        vkDestroyCommandPool(vk_data.logicalDevice, commandPool, nullptr);
    }
}