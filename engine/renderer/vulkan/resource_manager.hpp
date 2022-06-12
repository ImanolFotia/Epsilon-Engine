#pragma once 
#include "../types.hpp"
#include "../resource_manager.hpp"

namespace engine::vulkan {
    struct VulkanResourceManager : ResourceManager {

        virtual Ref<Texture> createTexture(CreateTextureInfo) = 0; 
        virtual Ref<Mesh> createMesh(CreateMeshInfo) = 0; 
        virtual Ref<Shader> createShader(CreateShaderInfo) = 0; 
        virtual Ref<UniformData> createUniformData(CreateUniformDataInfo) = 0; 

        virtual void destroyTexture(Ref<Texture>) = 0; 
        virtual void destroyMesh(Ref<Mesh>) = 0; 
        virtual void destroyShader(Ref<Shader>) = 0; 
        virtual void destroyUniformData(Ref<UniformData>) = 0; 

    };
}