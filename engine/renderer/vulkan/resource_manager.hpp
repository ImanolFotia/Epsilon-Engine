#pragma once 
#include "../types.hpp"
#include "../resource_manager.hpp"
#include "vulkan.hpp"

namespace engine {
    struct VulkanResourceManager : ResourceManager {

        VulkanResourceManager();
        ~VulkanResourceManager();

        ResourceManager* get() { 
            if(m_pSelf == nullptr) m_pSelf = new VulkanResourceManager();
            return m_pSelf; 
        }

        Ref<Texture> createTexture(unsigned char *pixels, TextureInfo) final override; 
        //Ref<Buffer> createBuffer(CreateMeshInfo) final override; 
        //Ref<Shader> createShader(ShaderInfo) final override; 
        //Ref<UniformData> createUniformData(CreateUniformDataInfo) final override; 

        //void destroyTexture(Ref<Texture>) final override; 
        //void destroyBuffer(Ref<Buffer>) final override; 
        //void destroyShader(Ref<Shader>) final override; 
        //void destroyUniformData(Ref<UniformData>) final override; 

        Pool<Texture, Renderer::TexturesDataId> texPool;


    };
}