#pragma once

#include <vk/vk.hpp>

#include "resource_manager.hpp"

namespace engine
{
    class Frame
    {
        Ref<Buffer> m_pUniformBuffer = Ref<Buffer>::makeEmpty();
        Ref<Buffer> m_pIndirectBuffer = Ref<Buffer>::makeEmpty();
        Ref<RenderPass> m_pRenderPass;
        
        vk::VulkanSyncObject* m_pSyncObjects = nullptr;
        VkCommandBuffer* m_pCommandBuffer = nullptr;

        uint32_t m_pFrameIndex = 0;
        uint32_t m_pCurrentImage = 0;

    public:
        Frame() = default;
        /**
         * Getters
         */
        Ref<RenderPass> getRenderPass()
        {
            return m_pRenderPass;
        }
        
        Ref<Buffer> UniformBuffer()
        {
            return m_pUniformBuffer;
        }

        Ref<Buffer> IndirectBuffer()
        {
            return m_pIndirectBuffer;
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

        void setRenderPass(Ref<RenderPass> renderPass)
        {
            m_pRenderPass = renderPass;
        }
        void UniformBuffer(Ref<Buffer> buffer)
        {
            m_pUniformBuffer = buffer;
        }
        void IndirectBuffer(Ref<Buffer> buffer)
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