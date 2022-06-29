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

        pCreateStagingTextureBuffer(pixels, texInfo);

        auto texture = pCreateTextureBuffer({.width = texInfo.width,
                                             .height = texInfo.height,
                                             .num_channels = texInfo.numChannels});

        auto size = texInfo.width * texInfo.height * texInfo.numChannels;
        uint16_t curr_texture_offset = current_texture_offset;
        current_texture_offset += size;
        // vk::copyBuffer(m_pVkData, m_pCommandPools.back(), m_pStagingBuffer.buffer, buffer.image, size, 0);

        transitionImageLayout(m_pVkDataPtr, m_pCommandPools.back(), texture.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        copyBufferToImage(m_pVkDataPtr, m_pCommandPools.back(), m_pStagingTextureBuffer.buffer, texture.image, static_cast<uint32_t>(textureInfo.width), static_cast<uint32_t>(textureInfo.height));
        transitionImageLayout(m_pVkDataPtr, m_pCommandPools.back(), texture.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        vmaDestroyBuffer(m_pAllocator, m_pStagingTextureBuffer.buffer, m_pStagingTextureBuffer.allocation);
        // vmaFreeMemory(m_pAllocator, m_pStagingTextureBuffer.allocation);

        texture.format = VK_FORMAT_R8G8B8A8_SRGB;
        vk::createImageView(m_pVkDataPtr, texture, VK_IMAGE_ASPECT_COLOR_BIT);
        vk::createTextureSampler(m_pVkDataPtr, texture);

        m_pTextures.push_back(texture);

        m_pTextureData.push_back({.id = m_pTextures.size() - 1,
                                  .size = size,
                                  .offset = curr_texture_offset,
                                  .texture_bucket = 0});

        return std::prev(m_pTextureData.end());
    }

    Ref<Material> createMaterial(MaterialInfo material) {
        Ref<Material> materialRef;

        auto &materialdata = m_pMaterials.emplace_back();

        for(auto& texture: material.textures){
            materialdata.textures.push_back(m_pTextures[texture->id]);
        }

        pCreateDescriptorSets(material);
        // pUpdateMaterial(material);
        Material mat;
        mat.id = m_pMaterials.size() - 1;
        
        return materialRef;
    }
}