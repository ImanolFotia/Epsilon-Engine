#pragma once 

#include "renderer.hpp"
#include "object_pool.hpp"

#include <memory>

namespace engine {
    
    struct Texture;
    struct Buffer;
    struct Shader;
    struct UniformBindings;
    struct Material;
    struct RenderPass;

    struct ResourceManager {

        virtual ResourceManager* get() = 0;

        virtual Ref<Texture> createTexture(unsigned char *pixels, TextureInfo) = 0; 
        virtual Ref<Buffer> destroyBuffer(BufferInfo) = 0; 
        virtual Ref<Shader> createShader(ShaderInfo) = 0; 
        virtual Ref<UniformBindings> createUniformData(UniformBindingInfo) = 0; 
        virtual Ref<Material> createMaterial(MaterialInfo) = 0;
        virtual Ref<RenderPass> createRenderPass(RenderPassInfo) = 0;

        virtual void destroyTexture(Ref<Texture>) = 0; 
        virtual void destroyBuffer(Ref<Buffer>) = 0; 
        virtual void destroyShader(Ref<Shader>) = 0; 
        virtual void destroyUniformData(Ref<UniformBindings>) = 0; 
        virtual void destroyMaterial(Ref<Material>) = 0;
        virtual void destroyRenderPass(Ref<RenderPass>) = 0;

        protected:
            ResourceManager* m_pSelf;
    }; 
}