#pragma once

#include <vk/vk.hpp>

namespace engine
{
    class Frame
    {
        vk::VulkanBuffer* m_pUniformBuffer = nullptr;
        vk::VulkanBuffer* m_pIndirectBuffer = nullptr;
        vk::VulkanSyncObject* m_pSyncObjects = nullptr;
        VkCommandBuffer* m_pCommandBuffer = nullptr;

        uint32_t m_pFrameIndex;
        uint32_t m_pCurrentImage;

    public:
        Frame() = default;
        /**
         * Getters
         */
        vk::VulkanBuffer& UniformBuffer()
        {
            return *m_pUniformBuffer;
        }

        vk::VulkanBuffer& IndirectBuffer()
        {
            return *m_pIndirectBuffer;
        }

        VkCommandBuffer& CommandBuffer()
        {
            return *m_pCommandBuffer;
        }

        vk::VulkanSyncObject& SyncObjects() {
            return *m_pSyncObjects;
        }

        uint32_t FrameIndex()
        {
            return m_pFrameIndex;
        }

        uint32_t CurrentImage()
        {
            return m_pCurrentImage;
        }
        /**
         * Setters
         */
        void UniformBuffer(vk::VulkanBuffer* buffer)
        {
            m_pUniformBuffer = buffer;
        }
        void IndirectBuffer(vk::VulkanBuffer* buffer)
        {
            m_pIndirectBuffer = buffer;
        }
        void CommandBuffer(VkCommandBuffer* buffer)
        {
            m_pCommandBuffer = buffer;
        }

        void SyncObjects(vk::VulkanSyncObject* objects) {
            m_pSyncObjects = objects;
        }

        void FrameIndex(uint32_t index)
        {
            m_pFrameIndex = index;
        }

        void CurrentImage(uint32_t image)
        {
            m_pCurrentImage = image;
        }
    };
}