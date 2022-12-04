#include <framework/window.hpp>
#include <framework/exception.hpp>

#include "vulkan.hpp"
#include <framework/common.hpp>
#include "resource_manager.hpp"

/**
 * @brief Implementation of the Vulkan renderer public API
 *
 */

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

namespace engine
{

    VulkanRenderer::VulkanRenderer()
    {
    }
    VulkanRenderer::~VulkanRenderer()
    {
    }
    void VulkanRenderer::Init(const char *appName, framework::Window &window)
    {
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
        /*
                pCreateVertexBuffer();
                pCreateIndexBuffer();
                pCreateUniformBuffers();
*/
        m_pResourceManagerRef->pCreateDescriptorPool();

        vk::createCommandBuffers(m_pVkData, m_pVkData.m_pCommandPools.back(), m_pVkData.m_pCommandBuffers);
        vk::createSyncObjects(m_pVkData);
    }

    engine::Renderer::ObjectDataId VulkanRenderer::RegisterMesh(const std::vector<Vertex> &, std::vector<IndexType> &indices, bool)
    {
        throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
    }

    engine::Renderer::TexturesDataId VulkanRenderer::RegisterTexture(unsigned char *, TextureInfo)
    {
        
        
        throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
    }

    Material VulkanRenderer::CreateMaterial(Ref<Material>)
    {
        
        
        throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
    }

    void VulkanRenderer::Push(ObjectData object)
    {
        DrawCommand drawCommand;
        auto mesh = m_pResourceManagerRef->meshPool.get(object.mesh);
        drawCommand.indexBuffer = mesh->indexBuffer;
        drawCommand.indexOffset = mesh->indexOffset;
        drawCommand.numIndices = mesh->numIndices;
        drawCommand.numVertices = mesh->numVertices;
        drawCommand.vertexBuffer = mesh->vertexBuffer;
        drawCommand.vertexOffset = mesh->vertexOffset;

        drawCommand.material = object.material;
        m_pCurrentCommandQueue.push_back(drawCommand);
    }

    void VulkanRenderer::PushCameraData(const ShaderData &camData)
    {
        m_pCameraData = camData;
    }

    void VulkanRenderer::Begin(Ref<RenderPass> renderPassRef)
    {

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

        pUpdateUniforms();
        vkResetCommandBuffer(m_pFrame.CommandBuffer(), 0);
        auto renderPass = m_pResourceManagerRef->getRenderPass(renderPassRef);
        m_pFrame.setRenderPass(renderPassRef);
        vk::recordCommandBuffer(m_pFrame.CommandBuffer(), m_pImageIndex);
        vk::createRenderPassInfo(m_pImageIndex, m_pVkData, *renderPass);
        vk::beginRenderPass(m_pFrame.CommandBuffer(), *renderPass);
    }

    void VulkanRenderer::End()
    {
        if (m_pImageIndex == -1)
            return;

        vk::endRenderPass(m_pFrame.CommandBuffer(), m_pVkData);
        vk::endRecording(m_pFrame.CommandBuffer());

        VkSemaphore signalSemaphores[] = {m_pFrame.SyncObjects().renderFinishedSemaphores};

        vk::Sync(m_pVkData, m_pFrame.CommandBuffer(), m_pCurrentFrame);

        bool should_recreate_swapchain = vk::Present(m_pVkData, signalSemaphores, m_pImageIndex);
        /*
                if (should_recreate_swapchain)
                {
                    // vk::recreateSwapChain<MeshPushConstant>(m_pVkData, m_pWindow->getWindow(), m_pRenderPasses.at(attachedRenderPass), m_pVertexInfo);
                    pRecreateSwapChain();
                    vkDestroyDescriptorPool(m_pVkData.logicalDevice, m_pResourceManagerRef->m_pDescriptorPool, nullptr);
                    m_pResourceManagerRef->pCreateDescriptorPool();

                    std::cout << "recreating descriptor sets\n";
                    for (auto &material : m_pMaterials)
                    {
                        pCreateDescriptorSets(material);
                        pUpdateMaterial(material);
                    }
                    std::cout << "swap chain recreated\n";
                }*/
        m_pCurrentFrame = (m_pCurrentFrame + 1) % vk::MAX_FRAMES_IN_FLIGHT;
    }

    void VulkanRenderer::Flush()
    {
        if (m_pImageIndex == -1)
            return;
        /*
                if (m_pRenderPasses.at(attachedRenderPass).renderPipelines.at(DefaultRenderPass).graphicsPipeline == NULL)
                    std::cout << "pipeline is null\n";
        */
        auto renderPass = m_pResourceManagerRef->getRenderPass(m_pFrame.getRenderPass());
        vk::bindPipeline(renderPass->renderPipelines.at(DefaultRenderPass), m_pFrame.CommandBuffer());

        int num_indices = 0;
        int curr_offset = 0;
        int curr_indices = 0;

        bool prev_group = false;
        for (auto &command : m_pCurrentCommandQueue)
        {
            auto material = m_pResourceManagerRef->materialPool.get(command.material);
            auto vertexBuffer = m_pResourceManagerRef->vertexBufferPool.get(command.vertexBuffer);//m_pResourceManagerRef->getBuffer(command.vertexBuffer);
            auto indexBuffer = m_pResourceManagerRef->indexBufferPool.get(command.indexBuffer);//m_pResourceManagerRef->getBuffer(command.indexBuffer);
            //auto material = m_pResourceManagerRef->getMaterial(command.material);
            vk::bindVertexBuffer(m_pVkData, m_pFrame.CommandBuffer(), vertexBuffer->buffer);
            vkCmdBindIndexBuffer(m_pFrame.CommandBuffer(), indexBuffer->buffer, 0, VK_INDEX_TYPE_UINT32);

            vkCmdBindDescriptorSets(m_pFrame.CommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, renderPass->renderPipelines.back().pipelineLayout, 0, 1, &material->descriptorSets[m_pCurrentFrame], 0, nullptr);
            /** TODO:
             *  Find a way to cleanly implement push constants
                vkCmdPushConstants(m_pFrame.CommandBuffer(), renderPass->renderPipelines.back().pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MeshPushConstant), &command.objectId->push_constant);
            */
            vk::drawIndexed(m_pFrame.CommandBuffer(), command.numIndices, 1, command.indexOffset, 0, 0);
        }

        m_pCurrentCommandQueue.clear();
    }

    int32_t VulkanRenderer::pPrepareSyncObjects()
    {

        vkWaitForFences(m_pVkData.logicalDevice, 1, &m_pVkData.syncObjects[m_pCurrentFrame].inFlightFences, VK_TRUE, UINT64_MAX);

        uint32_t imageIndex = 0;
        VkResult result = vkAcquireNextImageKHR(m_pVkData.logicalDevice, m_pVkData.swapChain, UINT64_MAX, m_pVkData.syncObjects[m_pCurrentFrame].imageAvailableSemaphores, VK_NULL_HANDLE, &imageIndex);
        // m_pFrame.CurrentImage(imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            m_pResourceManagerRef->pRecreateSwapChain(m_pWindow->getWindow());
            return -1;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        // Only reset the fence if we are submitting work
        vkResetFences(m_pVkData.logicalDevice, 1, &m_pVkData.syncObjects[m_pCurrentFrame].inFlightFences);
        return imageIndex;
    }

    void VulkanRenderer::pUpdateUniforms()
    { /*
         void *data;
         vmaMapMemory(m_pAllocator, m_pFrame.UniformBuffer().allocation, &data);
         memcpy(data, &m_pCameraData, sizeof(m_pCameraData));
         vmaUnmapMemory(m_pAllocator, m_pFrame.UniformBuffer().allocation);*/
    }

    void VulkanRenderer::Cleanup()
    {
        
        throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
    }

}