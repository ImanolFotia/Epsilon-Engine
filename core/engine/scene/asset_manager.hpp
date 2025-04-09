#pragma once

#include <core/engine/renderer/resource_manager.hpp>
#include <core/framework/loaders/gltf.hpp>
#include <core/framework/loaders/model.h>

#include <core/framework/loaders/DDS.h>

#include "audio/audio_object.hpp"

#include "core/engine/audio/audio_manager.hpp"
#include "core/framework/clock.hpp"
#include "core/framework/random.hpp"
#include "structs/box.hpp"
#include <core/engine/context.hpp>
#include <core/framework/utils/image.hpp>

#include <core/framework/vfs/filesystem.hpp>

#include "transform.hpp"
#include <core/engine/renderer/vulkan/imgui/imgui_setup.hpp>

namespace engine {

enum AssetType { TEXTURE = 0, MODEL, SIZE };

struct AssetInfo {
  std::string AssetPath{};
  AssetType type;
  std::size_t bindGroup = 0;
  std::size_t shadowBindGroup = 0;
};

struct ShaderData {
  alignas(4) float iTime = 0.0f;
  alignas(8) glm::vec2 iResolution{};
  alignas(8) glm::vec3 lightPosition{};
  alignas(8) glm::vec3 viewPosition{};
  alignas(16) glm::mat4 view{};
  alignas(16) glm::mat4 proj{};
  alignas(16) glm::mat4 lightMatrix{};
  alignas(4) uint32_t iFrame{};
  alignas(8) glm::vec2 iMouse{};
};
struct ShaderObjectData {
  uint32_t object_id{};
  uint32_t transform_index{};
  uint32_t material_index[4] = {};
  uint32_t numMaterials = 1;
  uint32_t animationIndex = 0;
};

struct CursorInfo {
  glm::vec4 position{};
  alignas(16) glm::vec3 normal{};
  alignas(16) glm::vec3 selected_item_position;
  alignas(4) int id{};
  int selected{};
  int mask{};
  float circle_radius = 5.0;
};

enum class TextureSlot : unsigned short {
  Albedo = 0,
  Normal,
  Metallic,
  Roughness
};

enum class MeshMask : uint32_t {
  NONE = 1 << 0,
  PROP = 1 << 1,
  TERRAIN = 1 << 2,
  ANIMATED = 1 << 3,
  ALL = 0xffffffff
};

struct PBRMaterial {
  int32_t albedo_texture_index = -1;
  int32_t normal_texture_index = -1;
  int32_t metallic_texture_index = -1;
  int32_t roughness_texture_index = -1;

  alignas(16) glm::vec4 albedo_color = glm::vec4(0.75f, 0.75f, 0.75f, 1.0f);
  float metallic_color = 1.0;
  float roughness_color = 0.038;
  MeshMask mask;

  float specular = 1.0f;
};

struct PBRMaterialIndex {
  PBRMaterial material;
  uint32_t index{};
  std::size_t Slot[4] = {0};
  std::string name;
};

struct RenderMesh {
  unsigned int id = 0;
  std::string name{};
  Ref<Mesh> mesh{};
  Ref<BindGroup> bind_group{};
  std::size_t material_keys[4];
  std::size_t numMaterials = 0;
};

struct RenderModel {
  std::array<std::vector<RenderMesh>, 4> renderMeshes;
  std::vector<framework::Animation> animations;
  std::vector<glm::mat4> transforms{};
  std::vector<Transform> instance_data;
  common::MIN_MAX_POINTS min_max_points;
  framework::Skeleton skeleton;
  std::string name{};
  std::string bindGroup = "DefaultLayout";
  std::string shadowBindGroup = "ShadowLayout";
  std::string prepassBindGroup = "prepassLayout";

  std::size_t bindGroupId = std::hash<std::string>{}("DefaultLayout");
  std::size_t shadowBindGroupId = std::hash<std::string>{}("ShadowLayout");
  std::size_t prepassBindGroupId = std::hash<std::string>{}("prepassLayout");
  uint32_t animationIndex = 0;
  bool hasAnimation = false;
  bool isInstanced = false;
  bool visible = false;
  bool loaded = false;

  Box getBox(glm::mat4 transform) {

    Box box;

    common::MIN_MAX_POINTS minmax(min_max_points.Min(), min_max_points.Max());
    minmax.transform(transform);

    box.size = minmax.getSize();
    box.position = minmax.Min();

    return box;
  }
};

enum class EntityType : uint32_t {
  OMNI = 0,
  SUN,
  SPOT,
  TUBE,
  SPHERE,
  DECAL,
  SIZE
};

struct ShaderEntity {
  glm::vec4 color = glm::vec4(1.0);
  glm::vec3 position{};
  float radius{};
  float innerRadius{};
  float outerRadius{};
  EntityType type = EntityType::SIZE;
  MeshMask flags = MeshMask::PROP;
};

struct ShaderAsset {
  std::string name;
  std::vector<std::string> filePaths;
  std::vector<std::string> spirvFilePaths;
};

struct GPUAnimationData {
  glm::mat4 transforms[300];
};

struct TextureInfo {
  TextureFormat format;
  WrapMode wrapMode;
  Filtering filtering;
};

struct Decal {
  glm::mat4 transform;
  uint32_t material_id;
};

class AssetManager {
  std::unordered_map<std::size_t, Ref<Texture>> m_pImages;
  std::unordered_map<std::string, RenderModel> m_pModels;
  std::unordered_map<std::string, AudioBuffer> m_pAudioBuffers;
  std::unordered_map<std::string, ShaderAsset> m_pShaders;
  std::unordered_map<std::size_t, PBRMaterialIndex> m_pMaterials;

  std::unordered_map<std::size_t, std::string> m_pImageName;

  std::unordered_map<std::string, Ref<Buffer>> m_pGPUBuffers;

  std::queue<uint32_t> m_pFreeMaterialIndexes;
  uint32_t m_pMaterialCurrentIndex = 0;

  std::queue<uint32_t> m_pFreeObjectIndexes;
  uint32_t m_pObjectCurrentIndex = 0;

  std::shared_ptr<Context> m_pContext;

  friend class Scene;

  uint32_t mesh_counter = 0;

  ShaderEntity *lightBufferPtr;

public:
  static const std::size_t MAX_MATERIALS = 10000;
  static const std::size_t MAX_OBJECTS = 1100000;
  static const std::size_t MAX_TRANSFORMS = 1100000;
  static const std::size_t MAX_DECALS = 1000;

  AssetManager();

  const std::unordered_map<std::size_t, Ref<Texture>> &getImages();

  void setContext(std::shared_ptr<Context> context);

  const std::string &getImageName(std::size_t hash);

  const Ref<Texture> getImage(const std::string &name);
  const Ref<Texture> getImage(std::size_t hash);
  const std::unordered_map<std::string, RenderModel> &getModels();

  const std::unordered_map<std::string, AudioBuffer> &getAudios();

  std::set<uint16_t> m_FreeEntities{};
  std::set<uint16_t> m_ReservedEntities{};

  std::vector<glm::mat4 *> transformBuffer;
  std::vector<ShaderObjectData *> objectBuffer;
  CursorInfo *infoBufferPtr;
  std::vector<GPUAnimationData *> animationTransformBufferPtr;

  void Init();
  Ref<Buffer> CreateGPUBuffer(const std::string &name, std::size_t size,
                              engine::BufferStorageType type =
                                  engine::BufferStorageType::STORAGE_BUFFER,
                              int count = -1);

  ShaderEntity *getEntityPointer(int32_t index = 0);

  int AddEntity(ShaderEntity entity, bool reserve = false);
  void AddEntity(int index, ShaderEntity entity);
  void RemoveEntity(int index);
  void *getMappedBuffer(const std::string &name);
  void FreeGPUBuffer(const std::string &name);

  void RegisterShader(ShaderAsset shader);

  ShaderAsset &getShader(const std::string &name);

  glm::mat4 *getTransformBuffer();
  ShaderObjectData *getObjectBuffer();

  GPUAnimationData *getAnimationBuffer();
  CursorInfo *getBufferPointer();
  CursorInfo *getBufferPointer(int frame);
  void Destroy();
  void deleteAudioSource(AudioObject object, Ref<audio::AudioSource> source);

  void deleteAudioObject(AudioObject object);

  void pLoadMaterials(std::string name, RenderMesh &subRenderC,
                      std::vector<common::MeshMaterial> materials);

  void ModifyModel(RenderModel);

  void DeleteModel(RenderModel model);
  RenderModel
  createModelFromMesh(const std::string &name, const common::Mesh &mesh,
                      std::initializer_list<common::MeshMaterial> materials);
  RenderModel createModelFromMesh(const std::string &name,
                                  const common::Mesh &mesh,
                                  const common::MeshMaterial &material);
  RenderModel &getModel(const std::string &name);

  std::stack<int> m_pFreeAnimationIndices;
  uint32_t m_pCurrentAnimationIndex = 0;

  uint32_t getAnimationIndex();
  void FreeAnimationIndex(uint32_t index);
  RenderModel &
  loadModel(const std::string &path, const std::string &name = "",
            std::initializer_list<common::MeshMaterial> materials = {});
  PBRMaterial loadModelMaterials(framework::Mesh::MeshMaterial &mesh_material);
  AudioObject loadAudio(const std::string &path, int numSources = 1);
  uint32_t getShaderDataIndex();
  Ref<Texture> addTextureFromBytes(const std::string &name,
                                   unsigned char *bytes, std::size_t size,
                                   int w, int h, int c,
                                   const TextureInfo &info);

  Ref<Texture> addTexture(const std::string &path, const TextureInfo &info);
  Ref<Mesh> addMesh(const std::string &name, common::Mesh mesh);

  Ref<Mesh> addMesh(const std::string &name, common::AnimatedMesh mesh);

  std::unordered_map<std::size_t, PBRMaterialIndex> &getMaterials();

  PBRMaterialIndex &getMaterial(std::size_t key);

  void setMaterial(std::size_t key, PBRMaterialIndex material);

  std::unordered_map<std::string, ShaderAsset> getShaders();

  std::size_t createMaterial(const std::string &name, PBRMaterial material);
  void setMaterial(PBRMaterialIndex material);

private:
};
} // namespace engine