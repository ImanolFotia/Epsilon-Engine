#include <framework/window.hpp>

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

        pCreateDescriptorPool();*/
        // pCreateDescriptorSets();

        vk::createCommandBuffers(m_pVkData, m_pVkData.m_pCommandPools.back(), m_pVkData.m_pCommandBuffers);
        vk::createSyncObjects(m_pVkData);
    }

    Renderer::ObjectDataId VulkanRenderer::RegisterMesh(const std::vector<Vertex> &vertices, std::vector<IndexType> &indices, bool group = true)
    {
        for (auto &index : indices)
        {
            index += current_vertex_count;
        }
        //auto [buffer, indexBuffer] = pGetBuffers(vertices, indices);

        //m_pResourseManagerRef->pCreateStagingBuffer(vertices);
        //m_pResourseManagerRef->pCreateStagingIndexBuffer(indices);

        vk::copyBuffer(m_pVkData, m_pVkData.m_pCommandPools.back(), m_pStagingBuffer.buffer, buffer->buffer, vertices.size() * sizeof(Vertex), current_vertex_count * sizeof(Vertex));

        vmaDestroyBuffer(m_pAllocator, m_pStagingBuffer.buffer, m_pStagingBuffer.allocation);
        //vmaFreeMemory(m_pAllocator, m_pStagingBuffer.allocation);

        IO::Log("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "copied ", vertices.size(), " vertices, of size ", vertices.size() * sizeof(Vertex), " bytes, at offset ", current_vertex_count * sizeof(Vertex), " to local buffer");

        vk::copyBuffer(m_pVkData, m_pCommandPools.back(), m_pStagingIndexBuffer.buffer, indexBuffer->buffer, indices.size() * sizeof(IndexType), current_index_count * sizeof(IndexType));

        vmaDestroyBuffer(m_pAllocator, m_pStagingIndexBuffer.buffer, m_pStagingIndexBuffer.allocation);
        //vmaFreeMemory(m_pAllocator, m_pStagingIndexBuffer.allocation);

        IO::Log("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "copied ", indices.size(), " indices, of size ", indices.size() * sizeof(IndexType), " bytes, at offset ", current_index_count * sizeof(IndexType), " to local buffer");
        uint16_t curr_vertex_offset = current_vertex_count;
        uint16_t curr_index_offset = current_index_count;
        current_vertex_count += vertices.size();
        current_index_count += indices.size();

        buffer->allocatedVertices += vertices.size();
        indexBuffer->allocatedVertices += indices.size();

        m_pObjectData.push_back({vertices.size(), indices.size(), curr_vertex_offset, curr_index_offset, m_pVertexBuffers.size() - 1, m_pIndexBuffers.size() - 1, group});
        
        
        return std::prev(m_pObjectData.end());
    }

    Renderer::TexturesDataId VulkanRenderer::RegisterTexture(unsigned char *data, TextureInfo textureInfo)
    {

        pCreateStagingTextureBuffer(data, textureInfo);

        auto texture = pCreateTextureBuffer({.width = textureInfo.width,
                                             .height = textureInfo.height,
                                             .num_channels = textureInfo.numChannels});

        auto size = textureInfo.width * textureInfo.height * textureInfo.numChannels;
        uint16_t curr_texture_offset = current_texture_offset;
        current_texture_offset += size;
        // vk::copyBuffer(m_pVkData, m_pCommandPools.back(), m_pStagingBuffer.buffer, buffer.image, size, 0);

        transitionImageLayout(m_pVkData, m_pCommandPools.back(), texture.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        copyBufferToImage(m_pVkData, m_pCommandPools.back(), m_pStagingTextureBuffer.buffer, texture.image, static_cast<uint32_t>(textureInfo.width), static_cast<uint32_t>(textureInfo.height));
        transitionImageLayout(m_pVkData, m_pCommandPools.back(), texture.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        vmaDestroyBuffer(m_pAllocator, m_pStagingTextureBuffer.buffer, m_pStagingTextureBuffer.allocation);
        //vmaFreeMemory(m_pAllocator, m_pStagingTextureBuffer.allocation);

        texture.format = VK_FORMAT_R8G8B8A8_SRGB;
        vk::createImageView(m_pVkData, texture, VK_IMAGE_ASPECT_COLOR_BIT);
        vk::createTextureSampler(m_pVkData, texture);

        m_pTextures.push_back(texture);

        m_pTextureData.push_back({.id = m_pTextures.size() - 1,
                                  .size = size,
                                  .offset = curr_texture_offset,
                                  .texture_bucket = 0});

        return std::prev(m_pTextureData.end());
    }

    Material VulkanRenderer::CreateMaterial(Ref<Material> material)
    {
        auto &material = m_pMaterials.emplace_back();

        for()
        material.textures.push_back(m_pTextures[texture->id]);
        pCreateDescriptorSets(material);
        // pUpdateMaterial(material);
        Material mat;
        mat.id = m_pMaterials.size() - 1;
        return mat;
    }

    void VulkanRenderer::Push(RenderObject object_id)
    {
        m_pCurrentCommandQueue.push_back(object_id);
    }

    void VulkanRenderer::PushCameraData(const ShaderData &camData)
    {
        m_pCameraData = camData;
    }

    void VulkanRenderer::Begin(vk::VulkanRenderPass &renderPass)
    {

        m_pImageIndex = pPrepareSyncObjects();

        if (m_pImageIndex == -1)
            return;

        if (m_pImageIndex >= vk::MAX_FRAMES_IN_FLIGHT)
            m_pImageIndex = vk::MAX_FRAMES_IN_FLIGHT - 1;

        m_pFrame.FrameIndex(m_pCurrentFrame);

        m_pFrame.CommandBuffer(&m_pVkData.m_pCommandBuffers.at(m_pCurrentFrame));
        //m_pFrame.UniformBuffer(&m_pUniformBuffers.at(m_pCurrentFrame));

        m_pFrame.SyncObjects(&m_pVkData.syncObjects.at(m_pCurrentFrame));

        // std::cout << "m_pImageIndex " << m_pImageIndex << std::endl;

        pUpdateUniforms();
        vkResetCommandBuffer(m_pFrame.CommandBuffer(), 0);

        vk::recordCommandBuffer(m_pFrame.CommandBuffer(), m_pImageIndex);
        vk::createRenderPassInfo(m_pImageIndex, m_pVkData, renderPass);
        vk::beginRenderPass(m_pFrame.CommandBuffer(), renderPass);
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

        if (should_recreate_swapchain)
        {
            // vk::recreateSwapChain<MeshPushConstant>(m_pVkData, m_pWindow->getWindow(), m_pRenderPasses.at(attachedRenderPass), m_pVertexInfo);
            pRecreateSwapChain();
            vkDestroyDescriptorPool(m_pVkData.logicalDevice, m_pDescriptorPool, nullptr);
            pCreateDescriptorPool();

            std::cout << "recreating descriptor sets\n";
            for (auto &material : m_pMaterials)
            {
                pCreateDescriptorSets(material);
                pUpdateMaterial(material);
            }
            std::cout << "swap chain recreated\n";
        }
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
        vk::bindPipeline(m_pRenderPasses.at(attachedRenderPass).renderPipelines.at(DefaultRenderPass), m_pFrame.CommandBuffer());
        vk::bindVertexBuffer(m_pVkData, m_pFrame.CommandBuffer(), m_pVertexBuffers[0].buffer);
        vkCmdBindIndexBuffer(m_pFrame.CommandBuffer(), m_pIndexBuffers[0].buffer, 0, VK_INDEX_TYPE_UINT32);

        int num_indices = 0;
        int curr_offset = 0;
        int curr_indices = 0;

        bool prev_group = false;
        for (auto &command : m_pCurrentCommandQueue)
        {
            vkCmdBindDescriptorSets(m_pFrame.CommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_pRenderPasses.at(0).renderPipelines.back().pipelineLayout, 0, 1, &m_pMaterials.at(command.materialId).descriptorSets[m_pCurrentFrame], 0, nullptr);
            vkCmdPushConstants(m_pFrame.CommandBuffer(), m_pRenderPasses.at(0).renderPipelines.back().pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MeshPushConstant), &command.objectId->push_constant);

            vk::drawIndexed(m_pFrame.CommandBuffer(), command.objectId->num_indices, 1, command.objectId->index_offset, 0, 0);
        }

        m_pCurrentCommandQueue.clear();
    }

    void VulkanRenderer::pRecreateSwapChain()
    {
        vkDeviceWaitIdle(m_pVkData.logicalDevice);

        for (auto &[index, pass] : m_pRenderPasses)
        {
            cleanupSwapChain(m_pVkData, pass);
        }

        vk::createSwapChain(m_pVkData, m_pWindow->getWindow());

        vk::createImageViews(m_pVkData);

        for (auto &[index, pass] : m_pRenderPasses)
        {
            vk::createRenderPass(m_pVkData, pass, m_pRenderPassInfo[index]);

            for (auto i = 0; i < pass.renderPipelines.size(); i++)
                vk::createGraphicsPipeline<MeshPushConstant>(m_pVkData, pass, pass.renderPipelines[i], m_pVertexInfo, m_pRenderPassInfo[index].shaderInfo);

            vk::createFramebuffers(m_pVkData, pass);
        }
    }
    int32_t VulkanRenderer::pPrepareSyncObjects()
    {

        vkWaitForFences(m_pVkData.logicalDevice, 1, &m_pVkData.syncObjects[m_pCurrentFrame].inFlightFences, VK_TRUE, UINT64_MAX);

        uint32_t imageIndex = 0;
        VkResult result = vkAcquireNextImageKHR(m_pVkData.logicalDevice, m_pVkData.swapChain, UINT64_MAX, m_pVkData.syncObjects[m_pCurrentFrame].imageAvailableSemaphores, VK_NULL_HANDLE, &imageIndex);
        // m_pFrame.CurrentImage(imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            pRecreateSwapChain();
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
    void VulkanRenderer::Cleanup()
    {
        vkDeviceWaitIdle(m_pVkData.logicalDevice);

        for (size_t i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++)
        {

            vmaDestroyBuffer(m_pAllocator, m_pUniformBuffers[i].buffer, m_pUniformBuffers[i].allocation);
        }

        vkDestroyDescriptorPool(m_pVkData.logicalDevice, m_pDescriptorPool, nullptr);

        vk::cleanupSyncObjects(m_pVkData);
        vk::cleanCommandPool(m_pVkData, m_pCommandPools[0]);

        for (auto &buffer : m_pVertexBuffers)
        {
            vmaDestroyBuffer(m_pAllocator, buffer.buffer, buffer.allocation);
        }

        for (auto &buffer : m_pIndexBuffers)
        {
            vmaDestroyBuffer(m_pAllocator, buffer.buffer, buffer.allocation);
        }

        for (auto &texture : m_pTextures)
        {

            vkDestroySampler(m_pVkData.logicalDevice, texture.sampler, nullptr);
            vkDestroyImageView(m_pVkData.logicalDevice, texture.imageView, nullptr);
            vk::destroyImage(m_pVkData, texture);
        }

        vmaDestroyAllocator(m_pAllocator);

        for (auto &pass : m_pRenderPasses)
        {
            vk::cleanupRenderPass(m_pVkData, pass.second.renderPass);
            for (auto &pipeline : pass.second.renderPipelines)
                vk::destroyGraphicsPipeline(m_pVkData, pipeline);
        }

        vk::cleanup(m_pVkData);
    }

    void VulkanRenderer::pUpdateUniforms()
    {
        void *data;
        vmaMapMemory(m_pAllocator, m_pFrame.UniformBuffer().allocation, &data);
        memcpy(data, &m_pCameraData, sizeof(m_pCameraData));
        vmaUnmapMemory(m_pAllocator, m_pFrame.UniformBuffer().allocation);
    }

    void VulkanRenderer::pUpdateMaterial(vk::VulkanMaterial &material)
    {
        for (int i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++)
        {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = m_pUniformBuffers[i].buffer;
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(ShaderData);

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            auto &texture = material.textures.at(0);
            // texture->format = VK_FORMAT_R8G8B8A8_SRGB;
            // vk::createImageView(m_pVkData, *texture, VK_IMAGE_ASPECT_COLOR_BIT);
            // vk::createTextureSampler(m_pVkData, *texture);

            imageInfo.imageView = texture.imageView;
            imageInfo.sampler = texture.sampler;

            std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = material.descriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;
            descriptorWrites[0].pImageInfo = nullptr;       // Optional
            descriptorWrites[0].pTexelBufferView = nullptr; // Optional

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = material.descriptorSets[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pImageInfo = &imageInfo;

            vkUpdateDescriptorSets(m_pVkData.logicalDevice, 2, descriptorWrites.data(), 0, nullptr);
            // vk::updateDescriptorSet(m_pVkData, material);
        }
    }

}