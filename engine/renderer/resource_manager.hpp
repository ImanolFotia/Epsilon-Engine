#pragma once

#include "renderer.hpp"
#include "object_pool.hpp"

#include <memory>

namespace engine
{

    struct ResourceManager
    {

        virtual ResourceManager *get() = 0;


        virtual void Init() = 0;

        virtual Ref<Texture> createTexture(unsigned char *pixels, TextureInfo) = 0;
        virtual Ref<Buffer> destroyBuffer(BufferInfo) = 0;
        virtual Ref<Shader> createShader(ShaderInfo) = 0;
        virtual Ref<UniformBindings> createUniformData(UniformBindingInfo) = 0;
        virtual Ref<Material> createMaterial(MaterialInfo, Ref<RenderPass>) = 0;
        virtual Ref<Mesh> createMesh(MeshInfo) = 0;
        virtual Ref<RenderPass> createRenderPass(RenderPassInfo) = 0;
        virtual Ref<RenderPass> createDefaultRenderPass(RenderPassInfo) = 0;
        virtual Ref<PushConstant> createPushConstant(PushConstantData) = 0;

        virtual void destroyTexture(Ref<Texture>) = 0;
        virtual void destroyBuffer(Ref<Buffer>) = 0;
        virtual void destroyShader(Ref<Shader>) = 0;
        virtual void destroyUniformData(Ref<UniformBindings>) = 0;
        virtual void destroyMaterial(Ref<Material>) = 0;
        virtual void destroyMesh(Ref<Mesh>) = 0;
        virtual void destroyRenderPass(Ref<RenderPass>) = 0;

        virtual void clean() = 0;

        template <typename T>
        std::shared_ptr<T> to()
        {
            return std::static_pointer_cast<T>(*this);
        }

    protected:
        ResourceManager *m_pSelf;
    };
}