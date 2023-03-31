#pragma once

#include "renderer.hpp"
#include "object_pool.hpp"

#include <memory>

namespace engine
{
    struct RenderPassBinding
    {
        // Ref<RenderPass> renderPass;
        std::string renderPass;
        uint32_t index;
        uint32_t bindingPoint;
    };

    struct RenderPassBindings
    {
        std::vector<RenderPassBinding> bindings;
    };

    struct MeshResource
    {
        Ref<Buffer> vertexBuffer;
        Ref<Buffer> indexBuffer;
        uint32_t vertexOffset{};
        uint32_t indexOffset{};
        uint32_t numVertices{};
        uint32_t numIndices{};
    };

    struct ResourceManager
    {

        virtual ResourceManager *get() = 0;

        virtual void Init() = 0;

        virtual Ref<Texture> createTexture(TextureCreationInfo) = 0;
        virtual Ref<UniformBindings> createUniformData(UniformBindingInfo) = 0;
        virtual Ref<Material> createMaterial(MaterialInfo, Ref<RenderPass>) = 0;
        virtual Ref<Material> createMaterial(MaterialInfo, Ref<RenderPass>, std::initializer_list<RenderPassBinding> bindings) = 0;
        virtual Ref<Mesh> createMesh(MeshInfo) = 0;
        virtual Ref<Buffer> createGPUBuffer(const std::string &, uint32_t, BufferStorageType) = 0;
        virtual Ref<RenderPass> createRenderPass(RenderPassInfo) = 0;
        virtual Ref<RenderPass> createDefaultRenderPass(RenderPassInfo) = 0;
        virtual Ref<PushConstant> createPushConstant(const std::string &, PushConstantData) = 0;

        virtual void destroyTexture(Ref<Texture>) = 0;
        virtual void destroyBuffer(Ref<Buffer>) = 0;
        virtual void destroyUniformData(Ref<UniformBindings>) = 0;
        virtual void destroyMaterial(Ref<Material>) = 0;
        virtual void destroyMesh(Ref<Mesh>) = 0;
        virtual void destroyRenderPass(Ref<RenderPass>) = 0;
        virtual Ref<Buffer> destroyBuffer(BufferInfo) = 0;

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