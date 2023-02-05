#include "core/framework/window.hpp"
#include "core/framework/exception.hpp"

#include "vulkan.hpp"
#include "core/framework/common.hpp"
#include "resource_manager.hpp"
#undef VMA_DEBUG_LOG
#undef VMA_DEBUG_LOG_FORMAT
/**
 * @brief Implementation of the Vulkan renderer public API
 *
 */

#define VMA_IMPLEMENTATION

#if defined(ANDROID) || defined(__ANDROID__)
#define VMA_VULKAN_VERSION 1000000
#endif

#include "vk_mem_alloc.h"

#ifdef WIN32
#if !defined(__PRETTY_FUNCTION__) && !defined(__GNUC__)
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

#endif


namespace engine {

    VulkanRenderer::VulkanRenderer() {
    }

    VulkanRenderer::~VulkanRenderer() {
    }

    void VulkanRenderer::Init(const char *appName, framework::Window &window) {
        m_pWindow = &window;
        m_pResourceManagerRef->m_pVkDataPtr = &m_pVkData;

        vk::createInstance(appName, m_pVkData);
        vk::setupDebugMessenger(m_pVkData.instance);
        vk::createSurface(m_pVkData, window.getWindow());
        vk::pickPhysicalDevice(m_pVkData);
        vk::createLogicalDevice(m_pVkData);

        vk::createSwapChain(m_pVkData, window.getWindow());
        vk::createImageViews(m_pVkData);

        m_pVkData.m_pCommandPools.emplace_back();

        vk::createCommandPool(m_pVkData, m_pVkData.m_pCommandPools.back());

        m_pResourceManagerRef->pCreateDescriptorPool();

        vk::createCommandBuffers(m_pVkData, m_pVkData.m_pCommandPools.back(), m_pVkData.m_pCommandBuffers);
        vk::createSyncObjects(m_pVkData);
    }

    engine::Renderer::ObjectDataId
    VulkanRenderer::RegisterMesh(const std::vector<Vertex> &, std::vector<IndexType> &indices, bool) {
        throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
    }

    engine::Renderer::TexturesDataId VulkanRenderer::RegisterTexture(unsigned char *, TextureInfo) {


        throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
    }

    Material VulkanRenderer::CreateMaterial(Ref<Material>) {


        throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
    }

    void VulkanRenderer::Push(ObjectData object) {
        DrawCommand drawCommand;
        auto mesh = m_pResourceManagerRef->meshPool.get(object.mesh);
        drawCommand.indexBuffer = mesh->indexBuffer;
        drawCommand.indexOffset = mesh->indexOffset;
        drawCommand.numIndices = mesh->numIndices;
        drawCommand.numVertices = mesh->numVertices;
        drawCommand.vertexBuffer = mesh->vertexBuffer;
        drawCommand.vertexOffset = mesh->vertexOffset;
        drawCommand.layoutIndex = object.layout_index;

        drawCommand.material = object.material;
        auto pushConstant = m_pResourceManagerRef->pushConstantPool.get(object.pushConstant);
        drawCommand.objectData = pushConstant->data;
        drawCommand.object_data_size = pushConstant->size;
        m_pCurrentCommandQueue.push_back(drawCommand);
    }

    void VulkanRenderer::Begin(Ref<RenderPass> renderPassRef) {

        m_pImageIndex = pPrepareSyncObjects();

        if (m_pImageIndex == -1)
            return;

        if (m_pImageIndex >= vk::MAX_FRAMES_IN_FLIGHT)
            m_pImageIndex = vk::MAX_FRAMES_IN_FLIGHT - 1;

        m_pFrame.FrameIndex(m_pCurrentFrame);

        m_pFrame.CommandBuffer(&m_pVkData.m_pCommandBuffers.at(m_pCurrentFrame));
        // m_pFrame.UniformBuffer(&m_pUniformBuffers.at(m_pCurrentFrame));

        m_pFrame.SyncObjects(&m_pVkData.syncObjects.at(m_pCurrentFrame));

        // std::cout << "m_pImageIndex " << m_pImageIndex << std::endl;

        vkResetCommandBuffer(m_pFrame.CommandBuffer(), 0);
        //auto renderPass = m_pResourceManagerRef->getRenderPass(renderPassRef);

        vk::recordCommandBuffer(m_pFrame.CommandBuffer(), m_pImageIndex);
    }

    void VulkanRenderer::Sync() {

        VkSemaphore signalSemaphores[] = {m_pFrame.SyncObjects().renderFinishedSemaphores};

        vk::Sync(m_pVkData, m_pFrame.CommandBuffer(), m_pCurrentFrame);

        bool should_recreate_swapchain = vk::Present(m_pVkData, signalSemaphores, m_pImageIndex);

        if (should_recreate_swapchain) {
            pRecreateSwapChain();
            vkDestroyDescriptorPool(m_pVkData.logicalDevice, m_pResourceManagerRef->m_pDescriptorPool, nullptr);
            m_pResourceManagerRef->pCreateDescriptorPool();

            std::cout << "recreating descriptor sets\n";

            m_pResourceManagerRef->pRecreateDescriptorSets();
            std::cout << "swap chain recreated\n";

        }

        m_pCurrentFrame = (m_pCurrentFrame + 1) % vk::MAX_FRAMES_IN_FLIGHT;
    }

    void VulkanRenderer::End() {
        if (m_pImageIndex == -1)
            return;

        vk::endRecording(m_pFrame.CommandBuffer());

    }

    void VulkanRenderer::Flush(engine::Ref<engine::RenderPass> renderPassRef) {
        if (m_pImageIndex == -1)
            return;
        /*
                if (m_pRenderPasses.at(attachedRenderPass).renderPipelines.at(DefaultRenderPass).graphicsPipeline == NULL)
                    std::cout << "pipeline is null\n";
        */
        auto renderPass = m_pResourceManagerRef->getRenderPass(renderPassRef);

        pUpdateUniforms(renderPass->uniformBuffer.buffers[m_pCurrentFrame]);

        vk::createRenderPassInfo(m_pImageIndex, m_pVkData, *renderPass);
        vk::beginRenderPass(m_pFrame.CommandBuffer(), *renderPass);


        int num_indices = 0;
        int curr_offset = 0;
        int curr_indices = 0;

        bool prev_group = false;
        int32_t prev_layout = -1;
        int32_t prev_vertex_buffer = -1;
        int32_t prev_index_buffer = -1;
        int changed = 0;
        for (auto &command: m_pCurrentCommandQueue) {
            if (prev_layout != command.layoutIndex) {
                vk::bindPipeline(renderPass->renderPipelines[command.layoutIndex].graphicsPipeline[command.layoutIndex],
                                 m_pFrame.CommandBuffer());
                prev_layout = command.layoutIndex;
            }

            auto material = m_pResourceManagerRef->materialPool.get(command.material);
            auto vertexBuffer = m_pResourceManagerRef->vertexBufferPool.get(
                    command.vertexBuffer);//m_pResourceManagerRef->getBuffer(command.vertexBuffer);
            auto indexBuffer = m_pResourceManagerRef->indexBufferPool.get(
                    command.indexBuffer);//m_pResourceManagerRef->getBuffer(command.indexBuffer);
            //auto material = m_pResourceManagerRef->getMaterial(command.material);
            if (prev_vertex_buffer != vertexBuffer->id) {
                vk::bindVertexBuffer(m_pVkData, m_pFrame.CommandBuffer(), vertexBuffer->buffer);
                prev_vertex_buffer = vertexBuffer->id;
            }

            if (prev_index_buffer != indexBuffer->id) {
                vkCmdBindIndexBuffer(m_pFrame.CommandBuffer(), indexBuffer->buffer, 0, VK_INDEX_TYPE_UINT32);
                prev_index_buffer = indexBuffer->id;
            }

            vkCmdBindDescriptorSets(m_pFrame.CommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    renderPass->renderPipelines.back().pipelineLayout.back(), 0, 1,
                                    &material->descriptorSets[m_pCurrentFrame], 0, nullptr);

            /** TODO:
             *  Find a way to cleanly implement push constants*/
            vkCmdPushConstants(m_pFrame.CommandBuffer(), renderPass->renderPipelines.back().pipelineLayout.back(),
                               VK_SHADER_STAGE_VERTEX_BIT, 0, command.object_data_size, command.objectData);

            vk::drawIndexed(m_pFrame.CommandBuffer(), command.numIndices, 1, command.indexOffset, command.vertexOffset, 0);
        }

        vk::endRenderPass(m_pFrame.CommandBuffer(), m_pVkData);
        m_pCurrentCommandQueue.clear();
    }

    int32_t VulkanRenderer::pPrepareSyncObjects() {

        vkWaitForFences(m_pVkData.logicalDevice, 1, &m_pVkData.syncObjects[m_pCurrentFrame].inFlightFences, VK_TRUE,
                        UINT64_MAX);

        uint32_t imageIndex = 0;
        VkResult result = vkAcquireNextImageKHR(m_pVkData.logicalDevice, m_pVkData.swapChain, UINT64_MAX,
                                                m_pVkData.syncObjects[m_pCurrentFrame].imageAvailableSemaphores,
                                                VK_NULL_HANDLE, &imageIndex);
        // m_pFrame.CurrentImage(imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            m_pResourceManagerRef->pRecreateSwapChain(m_pWindow->getWindow());
            return -1;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        // Only reset the fence if we are submitting work
        vkResetFences(m_pVkData.logicalDevice, 1, &m_pVkData.syncObjects[m_pCurrentFrame].inFlightFences);
        return imageIndex;
    }

    void VulkanRenderer::pUpdateUniforms(const vk::VulkanBuffer &buffer) {
        void *data;
        vmaMapMemory(m_pResourceManagerRef->m_pAllocator, buffer.allocation, &data);
        memcpy(data, perPassData, buffer.size);
        vmaUnmapMemory(m_pResourceManagerRef->m_pAllocator, buffer.allocation);
    }

    void VulkanRenderer::Cleanup() {
        m_pResourceManagerRef->clean();
    }

    void VulkanRenderer::pUpdateMaterial(vk::VulkanMaterial &) {
        throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);

    }

    void VulkanRenderer::pRecreateSwapChain() {
        vkDeviceWaitIdle(m_pVkData.logicalDevice);

        for (auto &pass: m_pResourceManagerRef->renderPassPool) {
            cleanupSwapChain(m_pVkData, pass);
        }

        vk::createSwapChain(m_pVkData, m_pWindow->getWindow());

        vk::createImageViews(m_pVkData);

        vk::createRenderPass(m_pVkData, m_pVkData.defaultRenderPass, m_pResourceManagerRef->m_pDefaultRenderPassInfo, true);

        for (auto i = 0; i < m_pVkData.defaultRenderPass.renderPipelines.size(); i++)
            vk::createGraphicsPipeline(m_pVkData, m_pVkData.defaultRenderPass,
                                       m_pVkData.defaultRenderPass.renderPipelines[i],
                                       m_pResourceManagerRef->m_pDefaultRenderPassInfo);

        vk::createFramebuffers(m_pVkData, m_pVkData.defaultRenderPass, m_pVkData.defaultRenderPass.renderPassChain);


        for (auto &pass: m_pResourceManagerRef->renderPassPool) {
            if (pass.id == std::numeric_limits<uint32_t>::max()) continue;

            vk::createRenderPass(m_pVkData, pass, m_pResourceManagerRef->m_pRenderPassInfo[pass.id], false);

            for (auto i = 0; i < pass.renderPipelines.size(); i++)
                vk::createGraphicsPipeline(m_pVkData, pass, pass.renderPipelines[i],
                                           m_pResourceManagerRef->m_pRenderPassInfo[pass.id]);

            vk::createFramebuffers(m_pVkData, pass, pass.renderPassChain);
        }
        //throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
    }

}