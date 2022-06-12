#pragma once 
#include "../types.hpp"
#include "../resource_manager.hpp"

namespace engine::vulkan {
    struct VulkanResourceManager : ResourceManager {

        Ref<Texture> createTexture(CreateTextureInfo) final override; 
        Ref<Mesh> createMesh(CreateMeshInfo) final override; 
        Ref<Shader> createShader(CreateShaderInfo) final override; 
        Ref<UniformData> createUniformData(CreateUniformDataInfo) final override; 

        void destroyTexture(Ref<Texture>) final override; 
        void destroyMesh(Ref<Mesh>) final override; 
        void destroyShader(Ref<Shader>) final override; 
        void destroyUniformData(Ref<UniformData>) final override; 

    };
}