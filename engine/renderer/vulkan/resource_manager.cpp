#include "resource_manager.hpp"

#include <stdexcept>

namespace engine
{
    VulkanResourceManager::VulkanResourceManager()
    {
        VmaAllocatorCreateInfo allocatorInfo = {};
        allocatorInfo.physicalDevice = m_pVkDataPtr->physicalDevice;
        allocatorInfo.device = m_pVkDataPtr->logicalDevice;
        allocatorInfo.instance = m_pVkDataPtr->instance;
        vmaCreateAllocator(&allocatorInfo, &m_pAllocator);
    }

    VulkanResourceManager::~VulkanResourceManager()
    {
    }

    Ref<Texture> VulkanResourceManager::createTexture(unsigned char *pixels, TextureInfo texInfo)
    {
        Ref<Texture> refTexture;

        auto stagingBuffer = pCreateStagingTextureBuffer(pixels, texInfo);

        auto texture = pCreateTextureBuffer({.width = texInfo.width,
                                             .height = texInfo.height,
                                             .num_channels = texInfo.numChannels});

        auto size = texInfo.width * texInfo.height * texInfo.numChannels;

        transitionImageLayout(*m_pVkDataPtr, m_pVkDataPtr->m_pCommandPools.back(), texture.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        copyBufferToImage(*m_pVkDataPtr, m_pVkDataPtr->m_pCommandPools.back(), stagingBuffer.buffer, texture.image, static_cast<uint32_t>(texInfo.width), static_cast<uint32_t>(texInfo.height));
        transitionImageLayout(*m_pVkDataPtr, m_pVkDataPtr->m_pCommandPools.back(), texture.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        vmaDestroyBuffer(m_pAllocator, stagingBuffer.buffer, stagingBuffer.allocation);
        // vmaFreeMemory(m_pAllocator, m_pStagingTextureBuffer.allocation);

        texture.format = VK_FORMAT_R8G8B8A8_SRGB;
        vk::createImageView(*m_pVkDataPtr, texture, VK_IMAGE_ASPECT_COLOR_BIT);
        vk::createTextureSampler(*m_pVkDataPtr, texture);

        auto ref = texPool.insert(texture);
        return ref;
    }

    Ref<Material> VulkanResourceManager::createMaterial(MaterialInfo material)
    { /*
         Ref<Material> materialRef;
         vk::VulkanMaterial vkMaterial;

         auto &materialdata = m_pMaterials.emplace_back();

         for (auto &texture : material.textures)
         {
             materialdata.textures.push_back(m_pTextures[texture->id]);
         }

         pCreateDescriptorSets(vkMaterial);

         return materialRef;*/
    }

    void VulkanResourceManager::pRecreateSwapChain(GLFWwindow* window)
    {
        vkDeviceWaitIdle(m_pVkDataPtr->logicalDevice);

        for (auto &pass : renderPassPool)
        {
            cleanupSwapChain(*m_pVkDataPtr, pass);
        }

        vk::createSwapChain(*m_pVkDataPtr, window);

        vk::createImageViews(*m_pVkDataPtr);

        for (auto &pass : renderPassPool)
        {
            vk::createRenderPass(*m_pVkDataPtr, pass, m_pRenderPassInfo[pass.id]);

            for (auto i = 0; i < pass.renderPipelines.size(); i++)
                vk::createGraphicsPipeline<MeshPushConstant>(*m_pVkDataPtr, pass, pass.renderPipelines[i], pass.vertexInfo, m_pRenderPassInfo[pass.id].shaderInfo);

            vk::createFramebuffers(*m_pVkDataPtr, pass);
        }
    }

    Ref<RenderPass> VulkanResourceManager::createRenderPass(RenderPassInfo renderPassInfo)
    {
        vk::VulkanRenderPass renderPass = {};

        renderPass.renderPipelines.emplace_back();
        renderPass.id = m_pRenderPassCount;
        m_pRenderPassInfo[m_pRenderPassCount] = renderPassInfo;

        vk::createRenderPass(*m_pVkDataPtr, renderPass, renderPassInfo);

        vk::VulkanVertexInfo vertexInfo;

        vertexInfo.attributeDescriptions =
            vk::getAttributeDescriptions(0, renderPassInfo.vertexLayout);

        vertexInfo.bindingDescription = vk::getBindingDescription(renderPassInfo.size);

        vk::createDescriptorSetLayout(*m_pVkDataPtr, renderPass.renderPipelines.back().descriptorSetLayout);

        vk::createGraphicsPipeline<MeshPushConstant>(*m_pVkDataPtr,
                                                     renderPass,
                                                     renderPass.renderPipelines.back(),
                                                     vertexInfo,
                                                     renderPassInfo.shaderInfo);

        vk::createFramebuffers(*m_pVkDataPtr, renderPass);

        renderPass.vertexInfo = vertexInfo;
        auto ref = renderPassPool.insert(renderPass);

        m_pRenderPassCount++;
        return ref;
    }

    void VulkanResourceManager::clean()
    {
        for (auto &buffer : bufferPool)
        {
            vmaDestroyBuffer(m_pAllocator, buffer.buffer, buffer.allocation);
        }

        vkDestroyDescriptorPool(m_pVkDataPtr->logicalDevice, m_pDescriptorPool, nullptr);

        vk::cleanupSyncObjects(*m_pVkDataPtr);
        vk::cleanCommandPool(*m_pVkDataPtr, m_pVkDataPtr->m_pCommandPools[0]);

        for (auto &texture : texPool)
        {

            vkDestroySampler(m_pVkDataPtr->logicalDevice, texture.sampler, nullptr);
            vkDestroyImageView(m_pVkDataPtr->logicalDevice, texture.imageView, nullptr);
            vk::destroyImage(*m_pVkDataPtr, texture);
        }

        vmaDestroyAllocator(m_pAllocator);

        for (auto &pass : renderPassPool)
        {
            vk::cleanupRenderPass(*m_pVkDataPtr, pass.renderPass);
            for (auto &pipeline : pass.renderPipelines)
                vk::destroyGraphicsPipeline(*m_pVkDataPtr, pipeline);
        }

        vk::cleanup(*m_pVkDataPtr);
    }
}