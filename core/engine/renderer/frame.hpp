#pragma once

#include "apis/vk/vk.hpp"

#include "resource_manager.hpp"

namespace engine
{
    class Frame
    {
        Ref<Buffer> m_pUniformBuffer = Ref<Buffer>::makeEmpty();
        Ref<Buffer> m_pIndirectBuffer = Ref<Buffer>::makeEmpty();
        Ref<RenderPass> m_pRenderPass;

        vk::VulkanSyncObject *m_pSyncObjects = nullptr;

        VkCommandBuffer *m_pCommandBuffer = nullptr;
        VkCommandBuffer *m_pComputeCommandBuffer = nullptr;

        uint32_t m_pFrameIndex = 0;
        uint32_t m_pCurrentImage = 0;

        uint32_t m_pComputeDispatches = 0;

        uint16_t m_SizeX{}, m_SizeY{};

    public:
        Frame() = default;
        /**
         * Getters
         */
        Ref<RenderPass> getRenderPass()
        {
            return m_pRenderPass;
        }

        bool SizeChanged(int x, int y)
        {
            return m_SizeX != x || m_SizeY != y;
        }

        bool SetSize(int x, int y)
        {
            int old_x = m_SizeX;
            int old_y = m_SizeY;
            m_SizeX = x;
            m_SizeY = y;

            printf("current size: %d %d\nlast size: %d %d\n", x, y, old_x, old_y);
            std::fflush(stdout);

            return m_SizeX != old_x || m_SizeY != old_y;
        }

        uint32_t ComputeDispatches()
        {
            return m_pComputeDispatches;
        }

        void ResetComputeDispatches()
        {
            m_pComputeDispatches = 0;
        }

        void AddComputeDispatch()
        {
            m_pComputeDispatches++;
        }

        Ref<Buffer> UniformBuffer()
        {
            return m_pUniformBuffer;
        }

        Ref<Buffer> IndirectBuffer()
        {
            return m_pIndirectBuffer;
        }

        VkCommandBuffer &CommandBuffer()
        {
            return *m_pCommandBuffer;
        }

        VkCommandBuffer &ComputeCommandBuffer()
        {
            return *m_pComputeCommandBuffer;
        }

        vk::VulkanSyncObject &SyncObjects()
        {
            return *m_pSyncObjects;
        }

        uint32_t FrameIndex() const
        {
            return m_pFrameIndex;
        }

        uint32_t CurrentImage() const
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
        void CommandBuffer(VkCommandBuffer *buffer)
        {
            m_pCommandBuffer = buffer;
        }

        void ComputeCommandBuffer(VkCommandBuffer *buffer)
        {
            m_pComputeCommandBuffer = buffer;
        }

        void SyncObjects(vk::VulkanSyncObject *objects)
        {
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