#pragma once
#include "../types.hpp"
#include "../resource_manager.hpp"
#include "vulkan.hpp"

namespace engine
{

    struct MeshResource {
        Ref<Buffer> vertexBuffer;
        Ref<Buffer> indexBuffer;
        uint32_t vertexOffset;
        uint32_t indexOffset;
        uint32_t numVertices;
        uint32_t numIndices;
    };

    struct VulkanResourceManager : ResourceManager
    {
        friend class VulkanRenderer;
        
        using IndexType = uint32_t;

        using CommandPools = std::vector<VkCommandPool>;
        using CommandBuffers = std::vector<VkCommandBuffer>;

        VulkanResourceManager();
        ~VulkanResourceManager();

        ResourceManager *get() override
        {
            if (m_pSelf == nullptr)
                m_pSelf = new VulkanResourceManager();
            return m_pSelf;
        }

        void Init() override;

        Ref<Texture> createTexture(unsigned char *pixels, TextureInfo) override;
        Ref<Buffer> destroyBuffer(BufferInfo) override;
        Ref<Shader> createShader(ShaderInfo) override;
        Ref<UniformBindings> createUniformData(UniformBindingInfo) override;
        Ref<Material> createMaterial(MaterialInfo, Ref<Buffer>, Ref<RenderPass> ) override;
        Ref<Mesh> createMesh(MeshInfo) override;
        Ref<RenderPass> createRenderPass(RenderPassInfo) override;

        void destroyTexture(Ref<Texture>) override;
        void destroyBuffer(Ref<Buffer>) override;
        void destroyShader(Ref<Shader>) override;
        void destroyUniformData(Ref<UniformBindings>) override;
        void destroyMaterial(Ref<Material>) override;
        void destroyMesh(Ref<Mesh>) override;
        void destroyRenderPass(Ref<RenderPass>) override;

        void clean() override;

        vk::VulkanTexture *getTexture(Ref<Texture>);
        vk::VulkanBuffer *getBuffer(Ref<Buffer>);
        ShaderStageInfo *getShader(Ref<Shader>);
        VkDescriptorSetLayoutBinding *getUniformData(Ref<UniformBindings>);
        vk::VulkanMaterial *getMaterial(Ref<Material>);
        vk::VulkanRenderPass *getRenderPass(Ref<RenderPass>);

    private:
        VmaAllocator m_pAllocator;
        vk::VulkanData *m_pVkDataPtr = nullptr;

    private:
        vk::VulkanBuffer pCreateVertexBuffer();
        vk::VulkanBuffer pCreateIndexBuffer();
        vk::VulkanBuffer pCreateUniformBuffer(UniformBindingInfo);
        vk::VulkanTexture pCreateTextureBuffer(vk::VulkanTextureInfo);

        Ref<Buffer> pFetchVertexBuffer(uint32_t numVertices);

        Ref<Buffer> pFetchIndexBuffer(uint32_t numIndices);

        void pRecreateSwapChain(GLFWwindow* window);

        void pCreateBuffer(vk::VulkanBuffer &, size_t, VkBufferUsageFlags, VmaAllocationCreateFlags, VmaMemoryUsage);

        vk::VulkanBuffer pCreateStagingBuffer(const std::vector<Vertex> &);
        vk::VulkanBuffer pCreateStagingIndexBuffer(const std::vector<IndexType> &);
        vk::VulkanBuffer pCreateStagingTextureBuffer(unsigned char *, TextureInfo);

        void pCreateUniformBuffers();
        void pCreateDescriptorPool();
        void pCreateDescriptorSets(vk::VulkanMaterial &);
        void pRecreateDescriptorSets();

        void pUpdateMaterial(vk::VulkanMaterial&);

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
        Pool<Buffer, vk::VulkanBuffer> vertexBufferPool;
        Pool<Buffer, vk::VulkanBuffer> indexBufferPool;
        Pool<Buffer, vk::VulkanBuffer> uniformBufferPool;
        Pool<UniformBindings, VkDescriptorSetLayoutBinding> uniformBindingPool;
        Pool<Material, vk::VulkanMaterial> materialPool;
        Pool<RenderPass, vk::VulkanRenderPass> renderPassPool;
        Pool<Mesh, MeshResource> meshPool;

        std::vector<Ref<Buffer>> indexBufferReferences;
        std::vector<Ref<Buffer>> vertexBufferReferences;

        VkDescriptorPool m_pDescriptorPool;
        std::vector<VkDescriptorSet> m_pDescriptorSets;

        std::vector<RenderPassInfo> m_pRenderPassInfo;
        uint32_t m_pRenderPassCount = 0;

        CommandPools m_pCommandPools;
        CommandBuffers m_pCommandBuffers;
    };
}