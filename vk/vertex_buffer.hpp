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
    static std::array<VkVertexInputAttributeDescription, num_descriptors> getAttributeDescriptions(uint32_t binding, std::initializer_list<std::pair<uint32_t, size_t>> vertexLayout)
    {
        std::array<VkVertexInputAttributeDescription, num_descriptors> attributeDescriptions{};
        uint32_t index = 0;
        for(auto& [format, offset]: vertexLayout) {
            attributeDescriptions[index].binding = binding;
            attributeDescriptions[index].location = index;
            attributeDescriptions[index].format = format;
            attributeDescriptions[index].offset = offset;
            index++;
        }
        return attributeDescriptions;
    }
}