#pragma once 

#include "types.hpp"

namespace engine {
    
    struct Texture;
    struct Mesh;
    struct Shader;
    struct UniformBindings;

    struct ResourceManager {
        static ResourceManager& get() { return m_pSelf; }

        virtual Ref<Texture> createTexture(CreateTextureInfo) = 0; 
        virtual Ref<Mesh> createMesh(CreateMeshInfo) = 0; 
        virtual Ref<Shader> createShader(CreateShaderInfo) = 0; 
        virtual Ref<UniformBindings> createUniformData(CreateUniformDataInfo) = 0; 

        virtual void destroyTexture(Ref<Texture>) = 0; 
        virtual void destroyMesh(Ref<Mesh>) = 0; 
        virtual void destroyShader(Ref<Shader>) = 0; 
        virtual void destroyUniformData(Ref<UniformBindings>) = 0; 

        private:
            static ResourceManager m_pSelf;
    }; 
}