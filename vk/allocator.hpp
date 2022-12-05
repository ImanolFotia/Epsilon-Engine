/**
 * @file allocator.hpp
 * @author Imanol Fotia (imanofotia@gmail.com)
 * @brief My custom memory allocator, created only for learning purposes, it would be better to use
 * a battle tested one like VMA by AMD, or anything that at least doesn't use the standard library.
 * The idea is to allocate memory only if the required properties change, to minimize allocations as
 * much as possible.
 * @version 0.1
 * @date 2022-06-04
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include <vulkan/vulkan.hpp>
#include "vk_data.hpp"
#include "vertex_buffer.hpp"
#include "memory.hpp"

#include <unordered_map>

namespace vk
{
    struct ABuffer
    {
        size_t size = 0;
        size_t offset = 0;
        VkMemoryPropertyFlags key;
    };
    class Allocator
    {

        struct internalAllocation
        {
            VkDeviceMemory deviceMemory;
            VkMemoryPropertyFlags properties;
            std::vector<VkBuffer> ownedBuffers;
            size_t allocatedBytes;
        };

        /**
         * @brief every memory pool is stored in a map indexed by the allocation properties.
         *
         */
        using MemoryAllocations = std::unordered_map<VkMemoryPropertyFlags, internalAllocation>;

    public:
        Allocator() = default;

        ABuffer CreateBuffer(size_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
        {
            ABuffer a_buffer{};
            VkBuffer buffer{};
            internalAllocation alloc{};

            a_buffer.size = size;
            a_buffer.key = properties;

            VkBufferCreateInfo bufferInfo = vk::createVertexBuffer(*vkStateReference, buffer, size, usage);
            alloc.deviceMemory = vk::allocateMemory(*vkStateReference, buffer, properties);
        }

        void MapMemory() {}
        void UnMapMemory() {}

    private:
        void allocate() {}
        VkBuffer createStagingBuffer() {}
        VulkanData* vkStateReference;
    };
}