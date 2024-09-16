#pragma once

#include <core/framework/threadpool.hpp>

#include "../object_pool.hpp"
#include "renderer.hpp"

#include <memory>

namespace engine {

struct RenderPassBindings {
  std::vector<RenderPassBinding> bindings;
};

struct ThreadData {
  VkCommandPool commandPool;
  std::vector<VkCommandBuffer> commandBuffer;
};

enum class CopyTextureType : uint8_t {
  IMAGE_TO_IMAGE = 0,
  IMAGE_TO_RENDER_TARGET,
  RENDER_TARGET_TO_IMAGE,
  SIZE
};

struct TextureCopyCommand {
  Ref<Texture> srcTexture;
  Ref<Texture> dstTexture;
  Ref<RenderPass> renderPass;
  CopyTextureType type;
  int8_t render_target_index = -1;
};

struct ResourceManager {

  virtual void Init() = 0;

  virtual Ref<Texture> createTexture(TextureCreationInfo) = 0;
  virtual Ref<UniformBindings> createUniformData(UniformBindingInfo) = 0;
  virtual Ref<BindGroup> createBindGroup(BindGroupInfo) = 0;
  virtual void updateBindGroup(Ref<BindGroup>, std::initializer_list<UniformBindingInfo>) = 0;
  virtual Ref<ComputeShader> createComputeShader(ComputeShaderInfo) = 0;
  virtual Ref<ComputeShader> UpdateComputeShader(Ref<ComputeShader>, ComputeShaderInfo computeInfo) = 0;
  virtual Ref<Mesh> createMesh(MeshInfo) = 0;
  virtual Ref<Mesh> createMesh(AnimatedMeshInfo) = 0;
  virtual Ref<Mesh> insertMesh(const std::string &, MeshResource) = 0;
  virtual Ref<Buffer> createGPUBuffer(const std::string &, uint32_t, BufferStorageType, int count = -1, GPUBufferUsage usage = GPUBufferUsage::SHARED) = 0;
  virtual Ref<RenderPass> createRenderPass(RenderPassInfo) = 0;
  virtual Ref<RenderPass> createDefaultRenderPass(RenderPassInfo) = 0;
  virtual Ref<PushConstant> createPushConstant(const std::string &, PushConstantData) = 0;
  virtual Ref<Buffer> createMappedVertexBuffer(const std::string &, const BufferInfo &) = 0;
  virtual void UpdateMappedBuffer(Ref<Buffer>, const UpdateVertexBufferInfo &) = 0;

  virtual void CopyTexture(TextureCopyCommand copyCommand) = 0;

  virtual void UpdateMesh(Ref<Mesh>, UpdateMeshInfo) = 0;
  virtual void PatchMesh(Ref<Mesh>, UpdateMeshInfo) = 0;
  virtual void UpdateUniform(const std::string &name, void *newData) = 0;

  virtual void destroyTexture(Ref<Texture>) = 0;
  virtual void destroyBuffer(Ref<Buffer>) = 0;
  virtual void destroyUniformData(Ref<UniformBindings>) = 0;
  virtual void destroyMaterial(Ref<BindGroup>) = 0;
  virtual void destroyMesh(Ref<Mesh>) = 0;
  virtual void destroyRenderPass(Ref<RenderPass>) = 0;
  virtual void destroyComputeShader(Ref<ComputeShader>) = 0;
  virtual Ref<Buffer> destroyBuffer(const BufferInfo &) = 0;

  virtual void ReloadShaders(const std::string &) = 0;

  virtual void *mapBuffer(Ref<Buffer> buffer, uint32_t currentFrame) = 0;

  virtual void unmapBuffer(Ref<Buffer> buffer, uint32_t currentFrame) = 0;

  virtual void *getMappedBuffer(Ref<Buffer> bufferRef, uint32_t currentFrame) = 0;

  virtual void clean() = 0;

  virtual void ResizeFramebuffer(Ref<RenderPass>, glm::ivec2) = 0;
  virtual void ResizeTexture(Ref<Texture>, glm::ivec2) = 0;

  virtual int FramesInFlight() = 0;

  template <typename T> std::shared_ptr<T> to() { return std::static_pointer_cast<T>(*this); }

protected:
  std::vector<ThreadData> m_pThreadData;
  framework::ThreadPool m_pThreadPool;

  ResourceManager *m_pSelf;
};
} // namespace engine