#pragma once 

#include "renderer.hpp"
#include "types.hpp"

#include <memory>

namespace engine {
    
    struct Texture;
    struct Buffer;
    struct Shader;
    struct UniformBindings;

    struct ResourceManager {

        virtual ResourceManager* get() = 0;

        virtual Ref<Texture> createTexture(unsigned char *pixels, TextureInfo) = 0; 
        //virtual Ref<Buffer> destroyBuffer(CreateMeshInfo) = 0; 
        //virtual Ref<Shader> createShader(ShaderInfo) = 0; 
        //virtual Ref<UniformBindings> createUniformData(CreateUniformDataInfo) = 0; 

        //virtual void destroyTexture(Ref<Texture>) = 0; 
        //virtual void destroyBuffer(Ref<Buffer>) = 0; 
        //virtual void destroyShader(Ref<Shader>) = 0; 
        //virtual void destroyUniformData(Ref<UniformBindings>) = 0; 

        void setRendererReference(std::shared_ptr<Renderer> r) {
            m_pRendererReference = r;
        }

        protected:
            ResourceManager* m_pSelf;
            std::shared_ptr<Renderer> m_pRendererReference;
    }; 
}