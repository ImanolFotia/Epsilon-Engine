#pragma once

#include "../object_pool.hpp"
#include "glm/glm.hpp"
#include <core/common/common.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace engine {
struct ID;
struct Texture;
struct Buffer;
struct Shader;
struct UniformBindings;
struct BindGroup;
struct RenderPass;
struct Mesh;
struct PushConstant;
struct ComputeShader;

enum renderer_type {
  vulkan = 0,
  dx12,
  dx11,
  opengl,
  metal,
  size // if we ever want to iterate over renderer types
};

enum class audio_type { openal = 0, size };

enum ShaderModuleStage { FRAGMENT = 0x1, VERTEX = 0x10, COMPUTE = 0x100, ALL = FRAGMENT | VERTEX | COMPUTE };

struct ShaderStageInfo {
  std::string entryPoint;
  std::vector<char> shaderCode;
  ShaderModuleStage stage;
  std::vector<std::size_t> renderPassIds;
  std::unordered_map<std::size_t /*renderpassid*/, std::vector<std::size_t> /*render pipelines*/> pipelines;
};

struct ShaderStageFactory {
  ShaderStageFactory entryPoint(const char *e) {
    info.entryPoint = e;
    return *this;
  }
  ShaderStageFactory shaderCode(std::vector<char> e) {
    info.shaderCode = e;
    return *this;
  }
  ShaderStageFactory stage(enum ShaderModuleStage e) {
    info.stage = e;
    return *this;
  }
  operator ShaderStageInfo() { return info; }

private:
  ShaderStageInfo info;
};

struct ShaderInfo {
  std::vector<ShaderStageInfo> stages;
  ShaderModuleStage usedStages;
  std::string name;
};

struct ShaderFactory {
  ShaderFactory stages(std::vector<ShaderStageInfo> e) {
    info.stages = e;
    return *this;
  }
  ShaderFactory usedStages(enum ShaderModuleStage e) {
    info.usedStages = e;
    return *this;
  }

  operator ShaderInfo() { return info; }

private:
  ShaderInfo info;
};

struct MemoryHeap {

  uint64_t total_memory = 0;
  uint64_t free_memory = 0;
  uint64_t used_memory = 0;
  std::string type;
};
struct ResourcesMemory_t {
  void reset() {
    for (auto &heap : heaps) {
      heap = MemoryHeap();
    }
  }
  uint32_t m_pDescriptorSetsAllocationSize = 0;
  uint32_t m_pTextureBufferAllocationSize = 0;
  uint32_t m_pVertexBufferAllocationSize = 0;
  uint32_t m_pIndexBufferAllocationSize = 0;
  uint32_t m_pUniformBufferAllocationSize = 0;
  uint32_t m_pStorageBufferAllocationSize = 0;

  std::vector<MemoryHeap> heaps{};
  std::unordered_map<std::string, double> m_StageTimings{};

  std::string GPUVendor{};
  std::string GPUName{};
  uint32_t numDrawCalls = 0;
  uint64_t numVertices = 0;
};

enum TextureFormat {
  // INTEGER IMAGE
  COLOR_R = 0,
  COLOR_RG,
  COLOR_RGB,
  COLOR_RGBA,
  NON_COLOR_R,
  NON_COLOR_RG,
  NON_COLOR_RGB,
  NON_COLOR_RGBA,

  COLOR_RGB_BC1,
  COLOR_RGB_BC2,
  COLOR_RGB_BC3,

  COLOR_RGBA_BC1,
  COLOR_RGBA_BC2,
  COLOR_RGBA_BC3,
  COLOR_RGBA_BC7,
  COLOR_RGBA_BC6HS,
  COLOR_RGBA_BC6HU,

  NON_COLOR_RGB_BC1,
  NON_COLOR_RGB_BC2,
  NON_COLOR_RGB_BC3,

  NON_COLOR_RGBA_BC1,
  NON_COLOR_RGBA_BC2,
  NON_COLOR_RGBA_BC3,
  NON_COLOR_RG_BC5U,
  NON_COLOR_RGBA_BC7,

  // FLOATING POINT IMAGE
  COLOR_R_16F,
  COLOR_R_32F,
  COLOR_RG_16F,
  COLOR_RG_32F,
  COLOR_RGB_16F,
  COLOR_RGB_32F,
  COLOR_RGBA_16F,
  COLOR_RGBA_32F,
  NON_COLOR_R_16F,
  NON_COLOR_R_32F,
  NON_COLOR_RG_16F,
  NON_COLOR_RG_32F,
  NON_COLOR_RGB_16F,
  NON_COLOR_RGB_32F,
  NON_COLOR_RGBA_16F,
  NON_COLOR_RGBA_32F,

  // DEPTH IMAGES
  DEPTH_F32,
  DEPTH_F32_STENCIL_8,
  DEPTH_F16,
  DEPTH_F16_STENCIL_8,

  DEPTH_UNORM,
  DEPTH_UNORM_STENCIL_8,
};

enum VertexFormat { XY_UINT, XY_INT, XY_FLOAT, XYZ_UINT, XYZ_INT, XYZ_FLOAT, XYZW_UINT, XYZW_INT, XYZW_FLOAT, XYZW_UINT16, XYZW_INT16, XYZW_FLOAT16, VERTEX_FORMAT_SIZE };

enum CompareFunction {
  ALWAYS = 0,
  LESS,
  LESS_OR_EQUAL,
  EQUAL,
};

enum WrapMode { REPEAT = 0, CLAMP_TO_BORDER, CLAMP_TO_EDGE };

enum Filtering { POINT = 0, LINEAR, ANISOTROPIC };

enum WindingMode { CLOCKWISE = 0, COUNTER_CLOCK_WISE };

enum CullMode { BACK = 0, FRONT, NONE };

enum BindingIndex {
  RENDERPASS_SET = 0,
  MATERIAL_SET,
  OBJECT_SET // I don't plan to use these one as push constants might be faster, but just in case
};

enum class DrawType { NON_INDEXED = 0, INDIRECT, INDEXED, INDEXED_INDIRECT, INSTANCED };

enum class MemoryUsage { DEVICE_ONLY = 0, HOST_VISIBLE, TRANSFER_SRC, TRANSFER_DST };

struct ObjectDataConstant {
  glm::mat4 transform = glm::mat4(1.0f);
  uint32_t material_index = 0;
  uint32_t animation_offset = 0;
};

struct Viewport {
  float offset_x = 0.0f;
  float offset_y = 0.0f;

  float width = 0.0f;
  float height = 0.0f;

  float min_depth = 0.0f;
  float max_depth = 0.0f;
};

struct Scissor {

  int offset_x = 0.0f;
  int offset_y = 0.0f;

  float width = 0.0f;
  float height = 0.0f;
};

struct TextureCreationInfo {
  uint32_t width;
  uint32_t height;
  uint32_t numChannels;
  uint32_t numLayers = 1;
  TextureFormat format{};
  WrapMode wrapMode{};
  Filtering filtering{};
  std::string name;
  bool isCompressed = false;
  bool isKTX = false;
  bool generateMipMaps = true;
  bool isAttachment = false;
  uint32_t mipLevels = 1;
  std::vector<std::size_t> offsets;
  std::size_t size{};
  bool storage_image = false;
  int bindless_array_index = -1;
  unsigned char *pixels = nullptr;
};

struct TextureBuilder {
  TextureBuilder width(uint32_t w) {
    self.width = w;
    return *this;
  }
  TextureBuilder height(uint32_t h) {
    self.height = h;
    return *this;
  }
  TextureBuilder numChannels(uint32_t nc) {
    self.numChannels = nc;
    return *this;
  }
  TextureBuilder format(TextureFormat f) {
    self.format = f;
    return *this;
  }

  TextureBuilder pixels(unsigned char *p) {
    self.pixels = p;
    return *this;
  }

  TextureBuilder name(const std::string &n) {
    self.name = n;
    return *this;
  }
  operator TextureCreationInfo() { return self; }
  TextureCreationInfo self;
};

struct Material {
  std::size_t id;
  std::string name;
};

struct IndirectPack {
  Mesh *mesh;
  Material *material;
  uint32_t first;
  uint32_t count;
};

struct SubPassInfo {};

struct VertexDescriptorInfo {
  VertexFormat format;
  std::size_t offset;
};

struct VertexLayout {
  std::vector<VertexDescriptorInfo> descriptors;
  std::size_t size;
};

struct RenderPassAttachment {
  TextureFormat format;
  WrapMode wrapMode;
  Filtering filtering;
  CompareFunction compareFunc;
  bool depthCompare = false;
  bool blendEnable = false;
  float clearColor[4]{};
  uint32_t depthStencilValue[2]{};
  bool isSampler = false;
  bool isDepthAttachment = false;
  bool isSwapChainAttachment = false;
  bool clearAttachment = true;
  bool isExtern = false;
  bool storageImage = false;
  int texture_index = -1;
  int imageview_index = -1;
  std::string externRenderPass = "";
  std::string name;
};

struct PushConstantData {
  std::size_t size;
  void *data;
};

template <typename T> struct PushConstantData2 {
  using type = T;
  std::size_t size;
  T data;
};

struct BufferInfo {
  std::size_t size{};
  std::size_t offset{};
  MemoryUsage usage{};
};

struct BufferInfoFactory {

  BufferInfoFactory size(std::size_t s) {
    info.size = s;
    return *this;
  }

  BufferInfoFactory offset(std::size_t o) {
    info.offset = o;
    return *this;
  }

  operator BufferInfo() { return info; }
  BufferInfo info;
};

enum class UniformBindingType {
  UNIFORM_BUFFER = 0,
  TEXTURE_SAMPLER,
  TEXTURE_IMAGE_COMBINED_SAMPLER,
  SHADER_STORAGE,
  BINDLESS_TEXTURE,
  ACCELERATION_STRUCTURE,
  STORAGE_IMAGE,
  SIZE
};

enum class MemoryBarrierHint : int {
  COMPUTE_WRITE_BUFFER_COMPUTE_READ_BUFFER = 0,
  COMPUTE_READ_BUFFER_COMPUTE_WRITE_BUFFER,
  COMPUTE_WRITE_STORAGE_IMAGE_COMPUTE_READ_STORAGE_IMAGE,
  COMPUTE_READ_STORAGE_IMAGE_COMPUTE_WRITE_STORAGE_IMAGE,
  COMPUTE_WRITE_BUFFER_GRAPHICS_READ_BUFFER,
  COMPUTE_WRITE_BUFFER_GRAPHICS_READ_INDEX,
  COMPUTE_WRITE_BUFFER_GRAPHICS_READ_UNIFORM,
  COMPUTE_WRITE_BUFFER_GRAPHICS_READ_INDIRECT,
  COMPUTE_WRITE_STORAGE_IMAGE_GRAPHICS_SAMPLE_IMAGE,
  COMPUTE_WRITE_STORAGE_TEXEL_GRAPHICS_READ_INDIRECT,
  COMPUTE_WRITE_STORAGE_TEXEL_GRAPHICS_READ_UNIFORM,
  GRAPHICS_WRITE_COLOR_COMPUTE_SAMPLE_IMAGE,
  GRAPHICS_WRITE_DEPTH_COMPUTE_SAMPLE_IMAGE,
  NONE
};

enum class BufferStorageType { UNIFORM_BUFFER = 0, STORAGE_BUFFER, IMAGE_BUFFER };

enum class GPUBufferUsage { SHARED = 0, DEVICE_ONLY };

enum class ShaderStage : std::uint32_t {
  VERTEX = 1,
  FRAGMENT = 2,
  COMPUTE = 4,
  TESSELLATION_EVALUATION = 8,
  TESSELLATION_CONTROL = 16,
  GEOMETRY = 32,
  MESH = 64,
  TASK = 128,
  SIZE = 7
};

inline ShaderStage operator|(ShaderStage first, ShaderStage second) {
  return static_cast<ShaderStage>(static_cast<std::uint32_t>(first) | static_cast<std::uint32_t>(second));
}

inline ShaderStage operator&(ShaderStage first, ShaderStage second) {
  return static_cast<ShaderStage>(static_cast<std::uint32_t>(first) & static_cast<std::uint32_t>(second));
}

struct UniformBindingInfo {
  std::size_t size;
  std::size_t offset;
  uint32_t binding = 0;
  uint32_t set = 0;
  UniformBindingType type;
  TextureCreationInfo textureInfo;
  std::string texture;
  std::string buffer;
  bool bindless = false;
  uint32_t descriptorCount = 1;
  ShaderStage stage = ShaderStage::FRAGMENT | ShaderStage::VERTEX;
  std::string name;
};

struct UniformBindingFactory {
  operator UniformBindingInfo() { return info; }

  UniformBindingInfo size(std::size_t s) {
    info.size = s;
    return *this;
  }
  UniformBindingInfo offset(std::size_t o) {
    info.offset = o;
    return *this;
  }
  UniformBindingInfo binding(uint32_t b) {
    info.binding = b;
    return *this;
  }
  UniformBindingInfo type(UniformBindingType t) {
    info.type = t;
    return *this;
  }

  UniformBindingInfo info;
};

struct PipelineLayout {
  ShaderInfo shaderInfo{};
  VertexLayout vertexLayout{};
  CullMode cullMode;
  WindingMode windingMode;
  bool depthWriteEnable = true;
  bool depthTestEnable = true;
  std::string name;
};

struct ImageDimensions {
  int width = 0;
  int height = 0;
};

struct RenderPassInfo {
  uint32_t index = 0;
  uint32_t numDescriptors = 0;
  uint32_t numAttributes = 0;
  uint32_t numLayouts = 0;
  uint32_t numAttachments = 0;
  // std::size_t size;
  bool depthAttachment = false;
  bool isSwapChainAttachment = false;
  std::vector<SubPassInfo> subpasses;
  std::vector<RenderPassAttachment> attachments;
  std::vector<PipelineLayout> pipelineLayout;
  std::unordered_map<std::string, uint32_t> pipelineIndices;
  PushConstantData pushConstant;
  std::vector<UniformBindingInfo> bindingInfo;
  ImageDimensions dimensions;
  uint8_t numSamples = 1;
  bool resizeWithSwapChain = false;
  std::string name;
};

struct RenderPassFactory {

  /*template<typename VertexLayout>
  RenderPassFactory vertexInfo() {
      info.size = sizeof(VertexLayout);
      return *this;
  }*/

  RenderPassFactory index(std::size_t index) {
    info.index = index;
    return *this;
  }

  RenderPassFactory depthAttachment(bool d) {
    info.depthAttachment = d;
    return *this;
  }

  RenderPassFactory isSwapChainAttachment(bool a) {

    info.isSwapChainAttachment = a;
    return *this;
  }

  RenderPassFactory subpasses(std::vector<SubPassInfo> sp) {
    info.subpasses = sp;
    return *this;
  }

  RenderPassFactory dimensions(ImageDimensions d) {
    info.dimensions = d;
    return *this;
  }

  RenderPassFactory inputs(std::initializer_list<UniformBindingInfo> bi) {
    info.bindingInfo = bi;
    return *this;
  }

  RenderPassFactory inputs(std::vector<UniformBindingInfo> bi) {
    info.bindingInfo = bi;
    return *this;
  }
  RenderPassFactory outputs(std::vector<RenderPassAttachment> a) {
    info.attachments = a;
    info.numAttachments = info.attachments.size();
    return *this;
  }

  RenderPassFactory pipelineLayout(PipelineLayout pl) {
    // assert(info.numDescriptors == pl.vertexLayout.size(), "All vertex layouts must have the same number of component in every pipeline layout");
    info.numDescriptors = pl.vertexLayout.descriptors.size();
    info.pipelineLayout.emplace_back(pl);
    info.numLayouts++;
    return *this;
  }

  RenderPassFactory pushConstant(std::size_t size) {
    info.pushConstant.size = size;
    return *this;
  }

  RenderPassFactory name(const std::string &n) {
    info.name = n;
    return *this;
  }

  operator RenderPassInfo() { return info; }

private:
  RenderPassInfo info;
};

struct RenderPassBinding {
  // Ref<RenderPass> renderPass;
  std::string renderPass;
  uint32_t index;
  uint32_t bindingPoint;
  UniformBindingType type;
};
struct BindGroupInfo {
  std::vector<UniformBindingInfo> bindingInfo{};
  std::vector<RenderPassBinding> inputs;
  std::string renderPass{};
  std::string computeShader{};
  std::string name{};
};

struct UpdateVertexBufferInfo {
  std::size_t size{};
  std::size_t offset{};
  void *vertices = nullptr;
};

struct UpdateMeshInfo {
  int32_t vertex_size = 0;
  int32_t vertex_offset = 0; // offset relative to current mesh
  int32_t index_size = 0;
  int32_t index_offset = 0; // offset relative to current mesh
  std::vector<common::Vertex> vertices{};
  std::vector<uint32_t> indices{};
  Ref<Buffer> vertexBuffer = Ref<Buffer>::makeEmpty();
};

struct MeshInfo {
  std::vector<common::Vertex> vertices{};
  std::vector<uint32_t> indices{};
  std::string name{};
};

struct AnimatedMeshInfo {
  std::vector<common::AnimatedVertex> vertices;
  std::vector<uint32_t> indices;
  std::string name;
};

struct MeshInfoFactory {
  MeshInfoFactory vertices(std::vector<common::Vertex> v) {
    info.vertices = v;
    return *this;
  }

  MeshInfoFactory indices(std::vector<uint32_t> i) {
    info.indices = i;
    return *this;
  }
  operator MeshInfo() { return info; }
  MeshInfo info;
};

struct AnimatedMeshInfoFactory {
  AnimatedMeshInfoFactory vertices(std::vector<common::AnimatedVertex> v) {
    info.vertices = v;
    return *this;
  }

  AnimatedMeshInfoFactory indices(std::vector<uint32_t> i) {
    info.indices = i;
    return *this;
  }
  operator AnimatedMeshInfo() { return info; }
  AnimatedMeshInfo info;
};

struct MemoryBarrier {
  MemoryBarrierHint memoryBarrierHint{};
  std::string image{};
  engine::Ref<Buffer> buffer = engine::Ref<Buffer>::makeEmpty();
};

struct ComputeShaderInfo {
  std::string name;
  PipelineLayout pipelineLayout;
  std::vector<UniformBindingInfo> bindingInfo;
  std::vector<MemoryBarrier> memoryBarrier{};

  uint16_t groupCountX = 1;
  uint16_t groupCountY = 1;
  uint16_t groupCountZ = 1;
};

} // namespace engine