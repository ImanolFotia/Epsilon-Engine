#pragma once
// #undef VMA_DEBUG_LOG
// #undef VMA_DEBUG_LOG_FORMAT
#include <core/common/common.hpp>
#include <core/engine/renderer/frame.hpp>
#include <core/engine/renderer/renderer.hpp>
#include <core/engine/renderer/types.hpp>

#include <apis/vk/vk.hpp>

#include <core/framework/singleton.hpp>

#include <list>
#include <vector>
#include <unordered_map>
/*
#define VMA_DEBUG_LOG_FORMAT(format, ...)

#define VMA_DEBUG_LOG_FORMAT(format, ...) \
    do                                    \
    {                                     \
        printf((format), __VA_ARGS__);    \
        printf("\n");                     \
    } while (false)
*/

#include "vk_mem_alloc.h"

class ImGuiRenderer;

namespace engine {
class VulkanResourceManager;
class VulkanRenderer : public Renderer {
  using RenderPasses = std::unordered_map<uint32_t, vk::VulkanRenderPass>;
  using VertexBuffers = std::vector<vk::VulkanBuffer>;
  using IndexBuffers = std::vector<vk::VulkanBuffer>;
  using TextureBuffers = std::vector<vk::VulkanTextureBuffer>;
  using Textures = std::vector<vk::VulkanTexture>;
  using Materials = std::vector<vk::VulkanMaterial>;
  using MemoryAllocations = std::unordered_map<VkMemoryPropertyFlags, vk::VulkanAllocation>;
  using IndexType = uint32_t;

  using VertexContainer = std::vector<common::Vertex>;
  using IndexContainer = std::vector<IndexType>;

  using UniformBuffers = std::vector<vk::VulkanBuffer>;

  const uint32_t DefaultRenderPass = 0;

  struct VulkanRenderPass : public RenderPass {
    vk::VulkanRenderPass renderPass;
  };
  std::vector<IndirectBatch> batches;

public:
  VulkanRenderer();

  ~VulkanRenderer();

  void Init(const char *appName, framework::Window &) override;

  ObjectDataId RegisterMesh(const std::vector<common::Vertex> &, std::vector<IndexType> &indices, bool) override;

  TexturesDataId RegisterTexture(TextureCreationInfo) override;

  Material CreateMaterial(Ref<Material>) override;

  void Push(ObjectData) override;

  void Submit() override;

  void EndFrame() override;

  void Begin() override;

  void BeginFrame() override;

  void End(glm::vec3 &v) override;

  void BeginCompute() override;

  void EndCompute() override;

  void Flush(FlushCommand command) override;

  void Flush(engine::Ref<engine::RenderPass>, engine::DrawType) override;

  void Cleanup() override;

  void UpdateRenderPassUniforms(Ref<RenderPass>, BindingIndex, const void *) override;

  void pUpdateUniformBuffer(const vk::VulkanBuffer &, const void *);

  void pUpdateUniforms(const vk::VulkanBuffer &);

  void pUpdateMaterial(vk::VulkanMaterial &);

  void setResourceManagerRef(std::shared_ptr<VulkanResourceManager> ref) { m_pResourceManagerRef = ref; }

  void *perPassData = nullptr;

  std::shared_ptr<ImGuiRenderer> getDebugRenderer() override;

  void SetViewport(const Viewport &) override;

  void SetScissor(const Scissor &) override;

  void SetVSync(bool) override;

  void SetRenderPass(Ref<RenderPass>) override;

  void InitDebugRenderer() override;

  void ComputeDispatch(Ref<ComputeShader>, Ref<BindGroup>) override;
  void ComputeDispatchAsync(Ref<ComputeShader>, Ref<BindGroup>) override;

private:
  void pRecreateSwapChain();
  int32_t pPrepareSyncObjects();

  void FlushNonIndexed(vk::VulkanRenderPass *renderPass);
  void FlushIndexed(vk::VulkanRenderPass *renderPass);
  void FlushIndirect(vk::VulkanRenderPass *renderPass);
  void FlushIndexedIndirect(vk::VulkanRenderPass *renderPass);

private:
  framework::Window *m_pWindow = nullptr;

  int32_t m_pImageIndex = 0;

  vk::VulkanData m_pVkData;

  Frame m_pFrame;

  vk::VulkanVertexInfo m_pVertexInfo;
  bool imguiInit = false;
  uint32_t renderpass_id = 0;
  uint32_t attachedRenderPass = 0;
  uint32_t m_pNumDrawCalls = 0;
  uint64_t m_pNumVertices = 0;
  engine::Ref<engine::RenderPass> m_pActiveRenderPass;

  std::shared_ptr<VulkanResourceManager> m_pResourceManagerRef = nullptr;
  bool m_pRenderPassActive = false;
  bool m_pShouldRecreateSwapchain = false;

  std::shared_ptr<ImGuiRenderer> m_pImguiRenderer; //!! TODO: find a better place for this

  std::vector<VkCommandBuffer> m_pSecondaryCommandBuffers;
};
} // namespace engine
