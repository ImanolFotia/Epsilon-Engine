#pragma once

#include "../renderer.hpp"
#include "../frame.hpp"
#include "../types.hpp"

#include <vk/vk.hpp>

#include <vector>
#include <list>

#include <vk_mem_alloc.h>

namespace engine
{
    class VulkanRenderer : public Renderer
    {
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
        
        using RenderPasses = std::unordered_map<uint32_t, vk::VulkanRenderPass>;
        using CommandPools = std::vector<VkCommandPool>;
        using CommandBuffers = std::vector<VkCommandBuffer>;
        using VertexBuffers = std::vector<vk::VulkanBuffer>;
        using IndexBuffers = std::vector<vk::VulkanBuffer>;
        using TextureBuffers = std::vector<vk::VulkanTextureBuffer>;
        using Textures = std::vector<vk::VulkanTexture>;
        using Materials = std::vector<vk::VulkanMaterial>;
        using MemoryAllocations = std::unordered_map<VkMemoryPropertyFlags, vk::VulkanAllocation>;
        using IndexType = uint32_t;

        using VertexContainer = std::vector<Vertex>;
        using IndexContainer = std::vector<IndexType>;

        using UniformBuffers = std::vector<vk::VulkanBuffer>;

        const uint32_t DefaultRenderPass = 0;


        struct VulkanRenderPass : public RenderPass {
            vk::VulkanRenderPass renderPass;
        };

    public:
        VulkanRenderer();

        ~VulkanRenderer();

        void Init(const char *appName, framework::Window &) override;

        uint32_t addRenderpass(engine::RenderPassInfo) override;

        ObjectDataId RegisterMesh(const std::vector<Vertex> &, std::vector<IndexType>& indices, bool) override;

        TexturesDataId RegisterTexture(unsigned char*, TextureInfo) override;

        Material CreateMaterial(Renderer::TexturesDataId texture);

        void Push(RenderObject) override;

        void PushCameraData(const ShaderData& camData);

        void Begin(uint32_t) override;

        void End() override;

        void Flush() override;

        void Cleanup() override;

    private:
        void pCreateVertexBuffer();
        void pCreateIndexBuffer();
        void pCreateUniformBuffer(size_t);
        vk::VulkanTexture pCreateTextureBuffer(vk::VulkanTextureInfo);

        void pCreateBuffer(vk::VulkanBuffer&, size_t, VkBufferUsageFlags, VmaAllocationCreateFlags, VmaMemoryUsage);

        std::pair<vk::VulkanBuffer*, vk::VulkanBuffer*> pGetBuffers(const VertexContainer& vertices, const IndexContainer& indices);

        void pCreateStagingBuffer(const std::vector<Vertex> &);
        void pCreateStagingIndexBuffer(const std::vector<IndexType> &);
        void pCreateStagingTextureBuffer(unsigned char*, TextureInfo);

        void pCreateUniformBuffers();
        void pCreateDescriptorPool();
        void pCreateDescriptorSets(vk::VulkanMaterial&);

        void pUpdateUniforms();
        void pUpdateMaterial(vk::VulkanMaterial&);

        void pRecreateSwapChain();
        int32_t pPrepareSyncObjects();
        
    private:
        framework::Window *m_pWindow = nullptr;

        uint32_t m_pCurrentFrame = 0;
        uint32_t m_pImageIndex = 0;

        vk::VulkanData m_pVkData;

        RenderPasses m_pRenderPasses;
        std::unordered_map<uint32_t, engine::RenderPassInfo> m_pRenderPassInfo;

        CommandPools m_pCommandPools;
        CommandBuffers m_pCommandBuffers;

        VertexBuffers m_pVertexBuffers;
        IndexBuffers m_pIndexBuffers;
        TextureBuffers m_pTextureBuffers;
        Textures m_pTextures;
        Materials m_pMaterials;

        UniformBuffers m_pUniformBuffers;
        VkDescriptorPool m_pDescriptorPool;
        std::vector<VkDescriptorSet> m_pDescriptorSets;

        Frame m_pFrame;

        vk::VulkanBuffer m_pStagingBuffer;
        vk::VulkanBuffer m_pStagingIndexBuffer;
        vk::VulkanBuffer m_pStagingTextureBuffer;

        vk::VulkanVertexInfo<6> m_pVertexInfo;

        uint32_t current_vertex_count = 0;
        uint32_t current_index_count = 0;
        uint32_t allocations_count = 0;


        uint32_t current_texture_offset = 0;

        uint32_t renderpass_id = 0;
        uint32_t attachedRenderPass = 0;

        ObjectsData m_pObjectData;
        TexturesData m_pTextureData;
        std::list<RenderObject> m_pCurrentCommandQueue;

        ShaderData m_pCameraData{};

        VmaAllocator m_pAllocator;
    };
}