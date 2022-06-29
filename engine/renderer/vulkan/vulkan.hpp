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

        Material CreateMaterial(Ref<Material>);

        void Push(RenderObject) override;

        void PushCameraData(const ShaderData& camData);

        void Begin(uint32_t) override;

        void End() override;

        void Flush() override;

        void Cleanup() override;



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

    };
}