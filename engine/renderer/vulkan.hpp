#pragma once

#include "renderer.hpp"
#include "frame.hpp"

#include <vk/vk.hpp>

#include <vector>
#include <list>

namespace engine
{
    class VulkanRenderer : public Renderer
    {
        const VkBufferUsageFlags UNIFORM_BUFFER_USAGE = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        const VkBufferUsageFlags VERTEX_BUFFER_USAGE = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        const VkBufferUsageFlags INDEX_BUFFER_USAGE = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        const VkBufferUsageFlags STAGING_BUFFER_USAGE = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        const VkBufferUsageFlags TEXTURE_BUFFER_USAGE = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

        const VkMemoryPropertyFlags STAGING_BUFFER_PROP = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        const VkMemoryPropertyFlags VERTEX_BUFFER_PROP = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        const VkMemoryPropertyFlags INDEX_BUFFER_PROP = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        const VkMemoryPropertyFlags UNIFORM_BUFFER_PROP = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        const VkMemoryPropertyFlags TEXTURE_BUFFER_PROP = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        using RenderPasses = std::unordered_map<uint32_t, vk::VulkanRenderPass>;
        using CommandPools = std::vector<VkCommandPool>;
        using CommandBuffers = std::vector<VkCommandBuffer>;
        using VertexBuffers = std::vector<vk::VulkanBuffer>;
        using IndexBuffers = std::vector<vk::VulkanBuffer>;
        using TextureBuffers = std::vector<vk::VulkanTextureBuffer>;
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

        uint32_t addRenderpass(RenderPassInfo) override;

        ObjectDataId RegisterMesh(const std::vector<Vertex> &, std::vector<IndexType>& indices, const MaterialInfo &, bool) override;

        TexturesDataId RegisterTexture(unsigned char*, TextureInfo) override;

        void Push(ObjectDataId) override;

        void PushCameraData(const ShaderData& camData);

        void Begin() override;

        void End() override;

        void Flush() override;

        void Cleanup() override;

    private:
        void pCreateVertexBuffer();
        void pCreateIndexBuffer();
        void pCreateUniformBuffer(size_t);
        void pCreateTextureBuffer();

        void pCreateBuffer(vk::VulkanBuffer&, size_t, VkBufferUsageFlags, VkMemoryPropertyFlags);

        std::pair<vk::VulkanBuffer*, vk::VulkanBuffer*> pGetBuffers(const VertexContainer& vertices, const IndexContainer& indices);

        void pCreateStagingBuffer(const std::vector<Vertex> &);
        void pCreateStagingIndexBuffer(const std::vector<IndexType> &);
        void pCreateStagingTextureBuffer(unsigned char*, TextureInfo);

        void pCreateUniformBuffers();
        void pCreateDescriptorPool();
        void pCreateDescriptorSets();

        void pUpdateUniforms();

        vk::VulkanAllocation pGetOrCreateDeviceMemory(VkMemoryPropertyFlags, const vk::VulkanBuffer&);
        vk::VulkanAllocation getDeviceMemory(VkMemoryPropertyFlags properties);
    private:
        framework::Window *m_pWindow = nullptr;

        uint32_t m_pCurrentFrame = 0;
        uint32_t m_pImageIndex = 0;

        vk::VulkanData m_pVkData;

        RenderPasses m_pRenderPasses;

        CommandPools m_pCommandPools;
        CommandBuffers m_pCommandBuffers;

        VertexBuffers m_pVertexBuffers;
        IndexBuffers m_pIndexBuffers;
        TextureBuffers m_pTextureBuffers;

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

        uint32_t renderpass_id = 0;

        ObjectsData m_pObjectData;
        std::list<typename ObjectsData::iterator> m_pCurrentCommandQueue;

        ShaderData m_pCameraData{};
    };
}