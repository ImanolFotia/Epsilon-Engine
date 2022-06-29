#pragma once
#include "../types.hpp"
#include "../resource_manager.hpp"
#include "vulkan.hpp"

namespace engine
{
    struct VulkanResourceManager : ResourceManager
    {
        using IndexType = uint32_t;

        VulkanResourceManager();
        ~VulkanResourceManager();

        ResourceManager *get()
        {
            if (m_pSelf == nullptr)
                m_pSelf = new VulkanResourceManager();
            return m_pSelf;
        }

        Ref<Texture> createTexture(unsigned char *pixels, TextureInfo) override;
        Ref<Buffer> destroyBuffer(BufferInfo) override;
        Ref<Shader> createShader(ShaderInfo) override;
        Ref<UniformBindings> createUniformData(UniformBindingInfo) override;
        Ref<Material> createMaterial(MaterialInfo) override;

        void destroyTexture(Ref<Texture>) override;
        void destroyBuffer(Ref<Buffer>) override;
        void destroyShader(Ref<Shader>) override;
        void destroyUniformData(Ref<UniformBindings>) override;
        void destroyMaterial(Ref<Material>) override;

        vk::VulkanTexture *getTexture(Ref<Texture>);
        vk::VulkanBuffer *getBuffer(Ref<Buffer>);
        ShaderStageInfo *getShader(Ref<Shader>);
        VkDescriptorSetLayoutBinding *getUniformData(Ref<UniformBindings>);
        vk::VulkanMaterial *getMaterial(Ref<Material>);

    private:
        VmaAllocator m_pAllocator;
        const vk::VulkanData *m_pVkDataPtr = nullptr;

    private:
        void pCreateVertexBuffer();
        void pCreateIndexBuffer();
        void pCreateUniformBuffer(size_t);
        vk::VulkanTexture pCreateTextureBuffer(vk::VulkanTextureInfo);

        void pCreateBuffer(vk::VulkanBuffer &, size_t, VkBufferUsageFlags, VmaAllocationCreateFlags, VmaMemoryUsage);

        vk::VulkanBuffer pCreateStagingBuffer(const std::vector<Vertex> &);
        vk::VulkanBuffer pCreateStagingIndexBuffer(const std::vector<IndexType> &);
        void pCreateStagingTextureBuffer(unsigned char *, TextureInfo);

        void pCreateUniformBuffers();
        void pCreateDescriptorPool();
        void pCreateDescriptorSets(vk::VulkanMaterial &);

        const VkBufferUsageFlags UNIFORM_BUFFER_USAGE = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        const VkBufferUsageFlags VERTEX_BUFFER_USAGE = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        const VkBufferUsageFlags INDEX_BUFFER_USAGE = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        const VkBufferUsageFlags STAGING_BUFFER_USAGE = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        const VkBufferUsageFlags TEXTURE_BUFFER_USAGE = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

        const VmaAllocationCreateFlags STAGING_BUFFER_PROP = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

        const VmaAllocationCreateFlags VERTEX_BUFFER_PROP = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
        const VmaAllocationCreateFlags INDEX_BUFFER_PROP = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
        const VmaAllocationCreateFlags UNIFORM_BUFFER_PROP = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
        const VmaAllocationCreateFlags TEXTURE_BUFFER_PROP = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

        const VmaMemoryUsage STAGING_BUFFER_MEM_USAGE = VMA_MEMORY_USAGE_CPU_ONLY;
        const VmaMemoryUsage VERTEX_BUFFER_MEM_USAGE = VMA_MEMORY_USAGE_GPU_ONLY;
        const VmaMemoryUsage INDEX_BUFFER_MEM_USAGE = VMA_MEMORY_USAGE_GPU_ONLY;
        const VmaMemoryUsage TEXTURE_BUFFER_MEM_USAGE = VMA_MEMORY_USAGE_AUTO;
        const VmaMemoryUsage UNIFORM_BUFFER_MEM_USAGE = VMA_MEMORY_USAGE_AUTO;

    protected:
        Pool<Texture, vk::VulkanTexture> texPool;
        Pool<Shader, ShaderStageInfo> shaderPool;
        Pool<Buffer, vk::VulkanBuffer> bufferPool;
        Pool<UniformBindings, VkDescriptorSetLayoutBinding> uniformBindingPool;
        Pool<Material, vk::VulkanMaterial> materialPool;
    };
}