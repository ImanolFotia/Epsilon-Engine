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
        // vk::copyBuffer(m_pVkData, m_pCommandPools.back(), m_pStagingBuffer.buffer, buffer.image, size, 0);

        transitionImageLayout(*m_pVkDataPtr, m_pCommandPools.back(), texture.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        copyBufferToImage(*m_pVkDataPtr, m_pCommandPools.back(), stagingBuffer.buffer, texture.image, static_cast<uint32_t>(textureInfo.width), static_cast<uint32_t>(textureInfo.height));
        transitionImageLayout(*m_pVkDataPtr, m_pCommandPools.back(), texture.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        vmaDestroyBuffer(m_pAllocator, stagingBuffer.buffer, stagingBuffer.allocation);
        // vmaFreeMemory(m_pAllocator, m_pStagingTextureBuffer.allocation);

        texture.format = VK_FORMAT_R8G8B8A8_SRGB;
        vk::createImageView(*m_pVkDataPtr, texture, VK_IMAGE_ASPECT_COLOR_BIT);
        vk::createTextureSampler(*m_pVkDataPtr, texture);

        auto ref = texPool.insert(texture);
        return ref;
    }

    Ref<Material> VulkanResourceManager::createMaterial(MaterialInfo material)
    {
        Ref<Material> materialRef;
        vk::VulkanMaterial vkMaterial;

        auto &materialdata = m_pMaterials.emplace_back();

        for (auto &texture : material.textures)
        {
            materialdata.textures.push_back(m_pTextures[texture->id]);
        }

        pCreateDescriptorSets(vkMaterial);

        return materialRef;
    }

    Ref<RenderPass> VulkanResourceManager::createRenderPass(RenderPassInfo renderPassInfo)
    {
        vk::VulkanRenderPass renderPass = {};

        renderPass.renderPipelines.emplace_back();

        //m_pRenderPassInfo[renderpass_id] = renderPassInfo;

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

        auto ref = renderPassPool.insert(renderPass);

        return ref;
    }
}