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

    template <typename T, uint32_t num_descriptors>
    static std::array<VkVertexInputAttributeDescription, num_descriptors> getAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, num_descriptors> attributeDescriptions{};
        for(int i = 0; i < num_descriptors; i++) {
            attributeDescriptions[i].binding = 0;
            attributeDescriptions[i].location = i;
            attributeDescriptions[i].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[i].offset = offset;
        }
        return attributeDescriptions;
    }
}