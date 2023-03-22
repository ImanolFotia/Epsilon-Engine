#pragma once
#include "../types.hpp"
#include "core/engine/renderer/resource_manager.hpp"
#include "vulkan.hpp"

namespace engine
{

    class VulkanResourceManager : public ResourceManager
    {
    public:
        ResourcesMemory_t ResourcesMemory{};

    public:
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

        Ref<Texture> createTexture(TextureCreationInfo) override;
        Ref<Buffer> destroyBuffer(BufferInfo) override;
        Ref<UniformBindings> createUniformData(UniformBindingInfo) override;

        Ref<Material> createMaterial(MaterialInfo, Ref<RenderPass>) override;
        Ref<Material> createMaterial(MaterialInfo, Ref<RenderPass>, std::initializer_list<RenderPassBinding> bindings) override;
        Ref<Mesh> createMesh(MeshInfo) override;
        Ref<Buffer> createGPUBuffer(const std::string &, uint32_t size, BufferStorageType type) override;
        Ref<RenderPass> createRenderPass(RenderPassInfo) override;
        Ref<RenderPass> createDefaultRenderPass(RenderPassInfo) override;
        Ref<PushConstant> createPushConstant(const std::string &, PushConstantData) override;

        void destroyTexture(Ref<Texture>) override;
        void destroyBuffer(Ref<Buffer>) override;
        void destroyUniformData(Ref<UniformBindings>) override;
        void destroyMaterial(Ref<Material>) override;
        void destroyMesh(Ref<Mesh>) override;
        void destroyRenderPass(Ref<RenderPass>) override;

        void *mapBuffer(Ref<Buffer> buffer);
        void unmapBuffer(Ref<Buffer> buffer);

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
        vk::VulkanUniformBuffer pCreateUniformBuffer(UniformBindingInfo);
        vk::VulkanTexture pCreateTextureBuffer(vk::VulkanTextureInfo);

        Ref<Buffer> pFetchVertexBuffer(uint32_t numVertices);

        Ref<Buffer> pFetchIndexBuffer(uint32_t numIndices, uint32_t maxOffset);

        void pRecreateSwapChain(framework::Window::windowType *window);

        void pCreateBuffer(vk::VulkanBuffer &, size_t, VkBufferUsageFlags, VmaAllocationCreateFlags, VmaMemoryUsage);

        vk::VulkanBuffer pCreateStagingBuffer(const std::vector<Vertex> &);
        vk::VulkanBuffer pCreateStagingIndexBuffer(const std::vector<IndexType> &);
        vk::VulkanBuffer pCreateStagingTextureBuffer(unsigned char *, TextureCreationInfo);

        void pCreateUniformBuffers();
        void pCreateDescriptorPool();
        void pCreateGlobalDescriptorPool();
        void pCreateGlobalDescriptorSets(VkDescriptorSetLayout layout, VkDescriptorPool &pool, VkDescriptorSet &descriptorSets, uint32_t count);
        void pCreateDescriptorSets(VkDescriptorSetLayout layout, VkDescriptorPool &pool, std::vector<VkDescriptorSet> &descriptorSets, uint32_t count);
        void pRecreateDescriptorSets();

        void pUpdateMaterial(vk::VulkanMaterial &);

        const uint32_t MAX_BINDLESS_RESOURCES = 16536;
        const uint32_t RENDERPASS_LAYOUT = 0;
        const uint32_t GLOBAL_LAYOUT = 1;
        const uint32_t MATERIAL_LAYOUT = 2;

        uint32_t texture_index = 0; //!!! ONLY FOR TESTING, SHOULD FIX THIS ASAP

        const VkBufferUsageFlags UNIFORM_BUFFER_USAGE = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        const VkBufferUsageFlags STORAGE_BUFFER_USAGE = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        const VkBufferUsageFlags RAYTRACING_BUFFER_USAGE = VK_BUFFER_USAGE_RAY_TRACING_BIT_NV;
        const VkBufferUsageFlags VERTEX_BUFFER_USAGE = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        const VkBufferUsageFlags INDEX_BUFFER_USAGE = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        const VkBufferUsageFlags STAGING_BUFFER_USAGE = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        const VkBufferUsageFlags TEXTURE_BUFFER_USAGE = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        const VkBufferUsageFlags INDIRECT_BUFFER_USAGE = VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;

        const VmaAllocationCreateFlags STAGING_BUFFER_PROP = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

        const VmaAllocationCreateFlags VERTEX_BUFFER_PROP = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
        const VmaAllocationCreateFlags INDEX_BUFFER_PROP = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
        const VmaAllocationCreateFlags UNIFORM_BUFFER_PROP = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
        const VmaAllocationCreateFlags STORAGE_BUFFER_PROP = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
        const VmaAllocationCreateFlags TEXTURE_BUFFER_PROP = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
        const VmaAllocationCreateFlags INDIRECT_BUFFER_PROP = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

        const VmaMemoryUsage STAGING_BUFFER_MEM_USAGE = VMA_MEMORY_USAGE_CPU_ONLY;
        const VmaMemoryUsage VERTEX_BUFFER_MEM_USAGE = VMA_MEMORY_USAGE_GPU_ONLY;
        const VmaMemoryUsage INDEX_BUFFER_MEM_USAGE = VMA_MEMORY_USAGE_GPU_ONLY;
        const VmaMemoryUsage TEXTURE_BUFFER_MEM_USAGE = VMA_MEMORY_USAGE_AUTO;
        const VmaMemoryUsage UNIFORM_BUFFER_MEM_USAGE = VMA_MEMORY_USAGE_AUTO;
        const VmaMemoryUsage STORAGE_BUFFER_MEM_USAGE = VMA_MEMORY_USAGE_AUTO;
        const VmaMemoryUsage INDIRECT_BUFFER_MEM_USAGE = VMA_MEMORY_USAGE_AUTO;

    protected:
        Pool<Texture, vk::VulkanTexture> texPool;
        Pool<ID, uint32_t> resourceIdPool;
        Pool<Shader, ShaderStageInfo> shaderPool;
        Pool<Buffer, vk::VulkanBuffer> vertexBufferPool;
        Pool<Buffer, vk::VulkanBuffer> indexBufferPool;
        Pool<Buffer, vk::VulkanGPUMappedBuffer> gpuBufferPool;
        Pool<UniformBindings, vk::VulkanUniformBuffer> uniformBufferPool;
        // Pool<UniformBindings, VkDescriptorSetLayoutBinding> uniformBindingPool;
        Pool<Material, vk::VulkanMaterial> materialPool;
        Pool<RenderPass, vk::VulkanRenderPass> renderPassPool;
        Pool<Mesh, MeshResource> meshPool;
        Pool<PushConstant, PushConstantData> pushConstantPool;

        std::vector<Ref<Buffer>> indexBufferReferences;
        std::vector<Ref<Buffer>> vertexBufferReferences;

        VkDescriptorPool m_pGlobalDescriptorPool;
        VkDescriptorSet m_pGlobalDescriptorSets;
        VkDescriptorSetLayout m_pGlobalDescriptorSetLayout;

        VkDescriptorPool m_pDescriptorPool;
        std::vector<VkDescriptorSet> m_pDescriptorSets;

        std::vector<RenderPassInfo> m_pRenderPassInfo;

        RenderPassInfo m_pDefaultRenderPassInfo;
        uint32_t m_pRenderPassCount = 0;

        CommandPools m_pCommandPools;
        uint32_t m_pNumCommandPools = 1;
        CommandBuffers m_pCommandBuffers;
        Ref<RenderPass> m_pDefaultRenderPassRef;

        uint32_t vertexBufferCount = 0;
        uint32_t indexBufferCount = 0;
        vk::VulkanBuffer m_pIndirectBuffer;
        vk::VulkanBuffer m_pMaterialBuffer;
    };
}