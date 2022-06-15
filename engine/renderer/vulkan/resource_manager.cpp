#include "resource_manager.hpp"

#include <stdexcept>

namespace engine
{
    VulkanResourceManager::VulkanResourceManager()
    {
    }
    VulkanResourceManager::~VulkanResourceManager()
    {
    }

    Ref<Texture> VulkanResourceManager::createTexture(unsigned char *pixels, TextureInfo texInfo)
    {
        if (m_pRendererReference == nullptr)
        {
            throw std::runtime_error("Renderer reference has not been set");
        }
        auto texId = m_pRendererReference->RegisterTexture(pixels, texInfo);
        return texPool.getNewRef(texId);
    }
}