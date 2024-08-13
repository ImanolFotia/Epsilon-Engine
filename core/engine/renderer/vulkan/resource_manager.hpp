#pragma once
#include "../types.hpp"
#include "apis/vk/vk.hpp"
#include "core/engine/renderer/resource_manager.hpp"
#include "vulkan.hpp"

#include "../resource_cache.hpp"

namespace engine {

class VulkanResourceManager : public ResourceManager {

public:
  friend class VulkanRenderer;
  friend class ::ImGuiRenderer;
  ResourcesMemory_t ResourcesMemory{};

public:
  using IndexType = uint32_t;

  using CommandPools = std::vector<VkCommandPool>;
  using CommandBuffers = std::vector<VkCommandBuffer>;

  VulkanResourceManager();
  ~VulkanResourceManager();

  ResourceManager *get() override {
    if (m_pSelf == nullptr)
      m_pSelf = new VulkanResourceManager();
    return m_pSelf;
  }

  void Init() override;

  Ref<BindGroup> createBindGroup(BindGroupInfo) override;
  void updateBindGroup(Ref<BindGroup>, std::initializer_list<UniformBindingInfo>) override;
  Ref<Texture> createTexture(TextureCreationInfo) override;
  Ref<Mesh> createMesh(MeshInfo) override;
  Ref<Mesh> createMesh(AnimatedMeshInfo) override;
  Ref<Mesh> insertMesh(const std::string &, MeshResource) override;
  Ref<Buffer> createGPUBuffer(const std::string &, uint32_t size, BufferStorageType type, int count, GPUBufferUsage usage) override;
  Ref<RenderPass> createRenderPass(RenderPassInfo) override;
  Ref<RenderPass> createDefaultRenderPass(RenderPassInfo) override;
  Ref<PushConstant> createPushConstant(const std::string &, PushConstantData) override;
  Ref<UniformBindings> createUniformData(UniformBindingInfo) override;
  Ref<ComputeShader> createComputeShader(ComputeShaderInfo) override;
  Ref<ComputeShader> UpdateComputeShader(Ref<ComputeShader>, ComputeShaderInfo computeInfo) override;
  Ref<Buffer> createMappedVertexBuffer(const std::string &, const BufferInfo &) override;
  void UpdateMappedBuffer(Ref<Buffer>, const UpdateVertexBufferInfo &) override;
  void UpdateMesh(Ref<Mesh>, UpdateMeshInfo) override;
  void PatchMesh(Ref<Mesh>, UpdateMeshInfo) override;

  void UpdateUniform(const std::string &name, void *newData) override;

  Ref<Texture> GetRenderTarget(Ref<RenderPass>, uint32_t) override;
  void CopyTexture(Ref<Texture> src, Ref<Texture> dst) override;

  void destroyTexture(Ref<Texture>) override;
  void destroyBuffer(Ref<Buffer>) override;
  void destroyUniformData(Ref<UniformBindings>) override;
  void destroyMaterial(Ref<BindGroup>) override;
  void destroyMesh(Ref<Mesh>) override;
  void destroyRenderPass(Ref<RenderPass>) override;
  void destroyComputeShader(Ref<ComputeShader>) override;
  Ref<Buffer> destroyBuffer(const BufferInfo &) override;

  void *mapBuffer(Ref<Buffer> buffer, uint32_t currentFrame) override;
  void unmapBuffer(Ref<Buffer> buffer, uint32_t currentFrame) override;
  void *getMappedBuffer(Ref<Buffer> bufferRef, uint32_t currentFrame) override;

  void clean() override;

  vk::VulkanTexture *getTexture(Ref<Texture> tex) { return texPool.get(tex); }
  vk::VulkanTexture *getTexture(uint32_t index) { return texPool.get(index); }
  vk::VulkanBuffer *getBuffer(Ref<Buffer>);
  ShaderStageInfo *getShader(Ref<Shader>);
  VkDescriptorSetLayoutBinding *getUniformData(Ref<UniformBindings>);
  vk::VulkanMaterial *getMaterial(Ref<BindGroup>);
  vk::VulkanRenderPass *getRenderPass(Ref<RenderPass>);

  void ResizeFramebuffer(Ref<RenderPass>, glm::ivec2) override;

private:
  VmaAllocator m_pAllocator;
  vk::VulkanData *m_pVkDataPtr = nullptr;

private:
  vk::VulkanBuffer pCreateVertexBuffer();
  vk::VulkanBuffer pCreateIndexBuffer();
  vk::VulkanUniformBuffer pCreateUniformBuffer(UniformBindingInfo);
  vk::VulkanTexture pCreateTextureBuffer(vk::VulkanTextureInfo);

  void ReloadShaders(const std::string &) override;

  Ref<Buffer> pFetchVertexBuffer(uint32_t numVertices, size_t vertexSize = sizeof(common::Vertex));

  Ref<Buffer> pFetchIndexBuffer(uint32_t numIndices, uint32_t maxOffset);

  void pRecreateSwapChain(framework::Window::windowType *window);

  void pCreateBuffer(vk::VulkanBuffer &, size_t, VkBufferUsageFlags, VmaAllocationCreateFlags, VmaMemoryUsage);

  vk::VulkanBuffer pCreateStagingBuffer(const std::vector<common::Vertex> &);
  vk::VulkanBuffer pCreateStagingBuffer(const std::vector<common::AnimatedVertex> &);
  vk::VulkanBuffer pCreateStagingIndexBuffer(const std::vector<IndexType> &);
  vk::VulkanBuffer pCreateStagingTextureBuffer(unsigned char *, TextureCreationInfo);

  void pCreateUniformBuffers();
  void pCreateDescriptorPool();
  void pCreateGlobalDescriptorPool();
  void pCreateGlobalDescriptorSets(VkDescriptorSetLayout layout, VkDescriptorPool &pool, VkDescriptorSet &descriptorSets, uint32_t count);
  void pCreateDescriptorSets(VkDescriptorSetLayout layout, VkDescriptorPool &pool, std::vector<VkDescriptorSet> &descriptorSets, uint32_t count);
  void pRecreateDescriptorSets();

  void pRecreateFrameBuffers(VkExtent2D);

  void pUpdateMaterial(vk::VulkanMaterial &);

  void pGenerateMipMaps(TextureCreationInfo, vk::VulkanTexture &);
  void UploadMipmaps();

  int FramesInFlight() override { return vk::MAX_FRAMES_IN_FLIGHT; }

  const uint32_t MAX_BINDLESS_RESOURCES = 16536;
  const uint32_t RENDERPASS_LAYOUT = 0;
  const uint32_t GLOBAL_LAYOUT = 1;
  const uint32_t MATERIAL_LAYOUT = 2;

  uint32_t texture_index = 0; //!!! ONLY FOR TESTING, SHOULD FIX THIS ASAP
  uint32_t descriptor_sets = 0;

  const VkBufferUsageFlags UNIFORM_BUFFER_USAGE = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
  const VkBufferUsageFlags STORAGE_BUFFER_USAGE = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
  const VkBufferUsageFlags RAYTRACING_BUFFER_USAGE = VK_BUFFER_USAGE_RAY_TRACING_BIT_NV;
  const VkBufferUsageFlags VERTEX_BUFFER_USAGE = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  const VkBufferUsageFlags INDEX_BUFFER_USAGE = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
  const VkBufferUsageFlags STAGING_BUFFER_USAGE = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  const VkBufferUsageFlags TEXTURE_BUFFER_USAGE = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  const VkBufferUsageFlags INDIRECT_BUFFER_USAGE = VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
  const VkBufferUsageFlags STORAGE_IMAGE_USAGE = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;

  const VmaAllocationCreateFlags STAGING_BUFFER_PROP = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

  const VmaAllocationCreateFlags VERTEX_BUFFER_PROP = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
  const VmaAllocationCreateFlags INDEX_BUFFER_PROP = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
  const VmaAllocationCreateFlags UNIFORM_BUFFER_PROP = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
  const VmaAllocationCreateFlags STORAGE_BUFFER_PROP = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
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
  Pool<Shader, ShaderInfo> shaderPool;
  Pool<Buffer, vk::VulkanBuffer> vertexBufferPool;
  Pool<Buffer, vk::VulkanBuffer> indexBufferPool;
  Pool<Buffer, vk::VulkanGPUMappedBuffer> gpuBufferPool;
  Pool<ComputeShader, vk::VulkanComputeShader> computeShaderPool;
  Pool<UniformBindings, vk::VulkanUniformBuffer> uniformBufferPool;
  // Pool<UniformBindings, VkDescriptorSetLayoutBinding> uniformBindingPool;
  Pool<BindGroup, vk::VulkanMaterial> materialPool;
  Pool<RenderPass, vk::VulkanRenderPass> renderPassPool;
  Pool<Mesh, MeshResource> meshPool;
  Pool<PushConstant, PushConstantData> pushConstantPool;

  std::vector<Ref<Buffer>> indexBufferReferences;
  std::vector<Ref<Buffer>> vertexBufferReferences;

  std::vector<Ref<RenderPass>> resizableRenderPasses;

  VkDescriptorPool m_pGlobalDescriptorPool = VK_NULL_HANDLE;
  VkDescriptorSet m_pGlobalDescriptorSets = VK_NULL_HANDLE;
  VkDescriptorSetLayout m_pGlobalDescriptorSetLayout = VK_NULL_HANDLE;

  VkDescriptorPool m_pDescriptorPool = VK_NULL_HANDLE;
  std::vector<VkDescriptorSet> m_pDescriptorSets;

  std::unordered_map<uint32_t, RenderPassInfo> m_pRenderPassInfo;

  RenderPassInfo m_pDefaultRenderPassInfo;
  uint32_t m_pRenderPassCount = 0;

  CommandPools m_pCommandPools;

  VkCommandPool m_pTransferCommandPool;

  uint32_t m_pNumCommandPools = 1;
  CommandBuffers m_pCommandBuffers;
  Ref<RenderPass> m_pDefaultRenderPassRef;

  uint32_t vertexBufferCount = 0;
  uint32_t indexBufferCount = 0;
  std::vector<vk::VulkanBuffer> m_pIndirectBuffer;
  vk::VulkanBuffer m_pMaterialBuffer;

  ResourceCache m_pVertexCache;
  ResourceCache m_pIndexCache;
  ResourceCache m_pTextureCache;
};
} // namespace engine