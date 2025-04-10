#include "asset_manager.hpp"

namespace engine {
AssetManager::AssetManager() {}

const std::unordered_map<std::size_t, Ref<Texture>> &AssetManager::getImages() {
  return m_pImages;
}

void AssetManager::setContext(std::shared_ptr<Context> context) {
  m_pContext = context;
}

const std::string &AssetManager::getImageName(std::size_t hash) {
  return m_pImageName.at(hash);
}

const Ref<Texture> AssetManager::getImage(const std::string &name) {
  if (m_pImages.contains(std::hash<std::string>{}(name)))
    return m_pImages[std::hash<std::string>{}(name)];

  return Ref<Texture>::makeEmpty();
}

const Ref<Texture> AssetManager::getImage(std::size_t hash) {
  if (m_pImages.contains(hash))
    return m_pImages[hash];
  return Ref<Texture>::makeEmpty();
}

const std::unordered_map<std::string, RenderModel> &AssetManager::getModels() {
  return m_pModels;
}

const std::unordered_map<std::string, AudioBuffer> &AssetManager::getAudios() {
  return m_pAudioBuffers;
}

void AssetManager::Init() {

  for (int i = 0; i < 256; i++) {
    m_FreeEntities.emplace(i);
  }

  Filesystem::Mount("./assets/buttons.zip");

  auto resourceManager = m_pContext->ResourceManager();

  m_pGPUBuffers["material_buffer"] = resourceManager->createGPUBuffer(
      "material_buffer", sizeof(PBRMaterial) * MAX_MATERIALS,
      engine::BufferStorageType::STORAGE_BUFFER);

  m_pGPUBuffers["decal_buffer"] = resourceManager->createGPUBuffer(
      "decal_buffer", sizeof(Decal) * MAX_DECALS,
      engine::BufferStorageType::STORAGE_BUFFER);

  m_pGPUBuffers["info_buffer"] = resourceManager->createGPUBuffer(
      "info_buffer", 64, engine::BufferStorageType::STORAGE_BUFFER, 1);

  m_pGPUBuffers["object_buffer"] = resourceManager->createGPUBuffer(
      "object_buffer", sizeof(ShaderObjectData) * MAX_OBJECTS,
      engine::BufferStorageType::STORAGE_BUFFER);

  m_pGPUBuffers["transform_buffer"] = resourceManager->createGPUBuffer(
      "transform_buffer", sizeof(glm::mat4) * MAX_TRANSFORMS,
      engine::BufferStorageType::STORAGE_BUFFER);

  m_pGPUBuffers["animation_transform_buffer"] =
      resourceManager->createGPUBuffer(
          "animation_transform_buffer", sizeof(GPUAnimationData) * 256,
          engine::BufferStorageType::STORAGE_BUFFER);

  m_pGPUBuffers["entity_buffer"] = resourceManager->createGPUBuffer(
      "entity_buffer", sizeof(ShaderEntity) * 256,
      engine::BufferStorageType::STORAGE_BUFFER, 1);

  transformBuffer.resize(vk::MAX_FRAMES_IN_FLIGHT);
  objectBuffer.resize(vk::MAX_FRAMES_IN_FLIGHT);
  // infoBufferPtr.resize(vk::MAX_FRAMES_IN_FLIGHT);
  animationTransformBufferPtr.resize(vk::MAX_FRAMES_IN_FLIGHT);
  lightBufferPtr = reinterpret_cast<ShaderEntity *>(
      resourceManager->mapBuffer(m_pGPUBuffers["entity_buffer"], 0));

  for (int i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++) {
    transformBuffer[i] = reinterpret_cast<glm::mat4 *>(
        resourceManager->mapBuffer(m_pGPUBuffers["transform_buffer"], i));
    objectBuffer[i] = reinterpret_cast<ShaderObjectData *>(
        resourceManager->mapBuffer(m_pGPUBuffers["object_buffer"], i));
    animationTransformBufferPtr[i] =
        reinterpret_cast<GPUAnimationData *>(resourceManager->mapBuffer(
            m_pGPUBuffers["animation_transform_buffer"], i));
  }
  infoBufferPtr = reinterpret_cast<CursorInfo *>(
      resourceManager->mapBuffer(m_pGPUBuffers["info_buffer"], 0));
}

Ref<Buffer> AssetManager::CreateGPUBuffer(
    const std::string &name, std::size_t size,
    engine::BufferStorageType type,
    int count) {
  if (m_pGPUBuffers.contains(name)) {
    Log::Error("Trying to create a buffer that already exists");
    return Ref<Buffer>::makeEmpty();
  }

  if (name.empty()) {
    Log::Error("GPU buffer name can't be null");
    return Ref<Buffer>::makeEmpty();
  }

  if (size <= 0) {
    Log::Error("GPU buffer size can't be zero");
    return Ref<Buffer>::makeEmpty();
  }
  auto resourceManager = m_pContext->ResourceManager();
  m_pGPUBuffers[name] =
      resourceManager->createGPUBuffer(name, size, type, count);

  return m_pGPUBuffers[name];
}

ShaderEntity *AssetManager::getEntityPointer(int32_t index) {
  return lightBufferPtr;
}

int AssetManager::AddEntity(ShaderEntity entity, bool reserve) {
  int index;
  if (m_FreeEntities.size() > 0) {
    index = *m_FreeEntities.begin();
    int count = 1;
    while (m_ReservedEntities.contains(index) || count > 255) {
      index = *m_FreeEntities.begin() + count;
    }
    m_FreeEntities.erase(index);

    if (reserve)
      m_ReservedEntities.emplace(index);
  } else {
    std::cerr << "No free entities" << std::endl;
    return -1;
  }

  lightBufferPtr[index] = entity;

  return index;
}

void AssetManager::AddEntity(int index, ShaderEntity entity) {
  if (index >= 256)
    return;

  m_ReservedEntities.emplace(index);

  if (m_FreeEntities.contains(index))
    m_FreeEntities.erase(index);
  lightBufferPtr[index] = entity;
}

void AssetManager::RemoveEntity(int index) {

  if (index >= 256)
    return;

  if (m_ReservedEntities.contains(index)) {
    m_ReservedEntities.erase(index);
  }

  m_FreeEntities.emplace(index);
  lightBufferPtr[index] = {.type = EntityType::SIZE};
}

void *AssetManager::getMappedBuffer(const std::string &name) {
  auto resourceManager = m_pContext->ResourceManager();
  return resourceManager->mapBuffer(m_pGPUBuffers[name], 0);
}

void AssetManager::FreeGPUBuffer(const std::string &name) {
  auto resourceManager = m_pContext->ResourceManager();
  auto buffer = m_pGPUBuffers[name];
  resourceManager->destroyBuffer(buffer);
  m_pGPUBuffers.erase(name);
}

void AssetManager::RegisterShader(ShaderAsset shader) {
  m_pShaders[shader.name] = shader;
}

ShaderAsset &AssetManager::getShader(const std::string &name) {
  return m_pShaders[name];
}

glm::mat4 *AssetManager::getTransformBuffer() {
  uint32_t currFrame = m_pContext->Renderer()->CurrentFrameInFlight();
  return transformBuffer[currFrame];
}

ShaderObjectData *AssetManager::getObjectBuffer() {
  uint32_t currFrame = m_pContext->Renderer()->CurrentFrameInFlight();
  return objectBuffer[currFrame];
}

GPUAnimationData *AssetManager::getAnimationBuffer() {
  uint32_t currFrame = m_pContext->Renderer()->CurrentFrameInFlight();
  return animationTransformBufferPtr[currFrame];
}

CursorInfo *AssetManager::getBufferPointer() {
  return infoBufferPtr; // [currFrame] ;
}

CursorInfo *AssetManager::getBufferPointer(int frame) {

  return infoBufferPtr; // [frame] ;
}

void AssetManager::Destroy() {

  /*auto audioManager = m_pContext->AudioManager();
  for (auto &[name, audio] : m_pAudioBuffers) {
    for (auto &source : audio.sources) {
      audioManager->Stop(source);
      audioManager->deleteSource(source);
    }
  }

  for (auto &[name, audio] : m_pAudioBuffers) {
    if (audio.sources.size() > 0)
      audioManager->deleteBuffer(audio.buffer);
  }

  audioManager->CleanUp();*/
}

void AssetManager::deleteAudioSource(AudioObject object,
                                     Ref<audio::AudioSource> source) {
  auto audioManager = m_pContext->AudioManager();
  if (audioManager->getSourceState(source) == audio::AudioState::PLAYING)
    audioManager->Stop(source);
  auto id = audioManager->releaseSource(source);

  for (int i = 0; i < m_pAudioBuffers[object.name].sources.size(); i++) {
    if (id == audioManager->getId(source)) {
      m_pAudioBuffers[object.name].sources.erase(
          m_pAudioBuffers[object.name].sources.begin() + i);
      break;
    }
  }

  // audioManager->deleteSource(source);
}

void AssetManager::deleteAudioObject(AudioObject object) {
  auto audioManager = m_pContext->AudioManager();
  for (auto &source : object.source) {
    deleteAudioSource(object, source);
  }
  audioManager->deleteBuffer(object.Buffer().buffer);
  m_pAudioBuffers.extract(object.name);
}

void AssetManager::pLoadMaterials(std::string name, RenderMesh &subRenderC,
                                  std::vector<common::MeshMaterial> materials) {

  auto resourceManager = m_pContext->ResourceManager();
  unsigned int material_index = 0;
  for (auto &material : materials) {
    std::string material_name =
        name + "_material" + std::to_string(material_index);
    std::size_t material_name_hash = std::hash<std::string>{}(material_name);

    if (m_pMaterials.contains(material_name_hash)) {
      subRenderC.material_keys[material_index] = material_name_hash;
      material_index++;
      continue;
    }
    subRenderC.material_keys[material_index] = material_name_hash;

    PBRMaterial pbr_material;

    pbr_material.albedo_color = material.color;
    pbr_material.metallic_color = material.metallic;
    pbr_material.roughness_color = material.roughness;
    pbr_material.mask = (MeshMask)material.mask;

    if (!material.albedo_path.empty()) {
      auto albedo = addTexture(
          material.albedo_path,
          {.format = COLOR_RGBA, .wrapMode = REPEAT, .filtering = LINEAR});

      pbr_material.albedo_texture_index = albedo.Index();
    }
    if (!material.metallic_path.empty()) {
      auto metallic = addTexture(
          material.metallic_path,
          {.format = NON_COLOR_RGBA, .wrapMode = REPEAT, .filtering = LINEAR});

      pbr_material.metallic_texture_index = metallic.Index();
    }
    if (!material.roughness_path.empty()) {
      auto roughness = addTexture(
          material.roughness_path,
          {.format = NON_COLOR_RGBA, .wrapMode = REPEAT, .filtering = LINEAR});

      pbr_material.roughness_texture_index = roughness.Index();
    }
    if (!material.normal_path.empty()) {
      auto normal = addTexture(
          material.normal_path,
          {.format = NON_COLOR_RGBA, .wrapMode = REPEAT, .filtering = LINEAR});

      pbr_material.normal_texture_index = normal.Index();
    }

    uint32_t mat_index = 0;
    if (!m_pFreeMaterialIndexes.empty()) {
      mat_index = m_pFreeMaterialIndexes.front();
      m_pFreeMaterialIndexes.pop();
    } else {
      mat_index = m_pMaterialCurrentIndex;
      m_pMaterialCurrentIndex++;
    }

    for (int i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++) {
      PBRMaterial *materialBufferPtr = reinterpret_cast<PBRMaterial *>(
          resourceManager->mapBuffer(m_pGPUBuffers["material_buffer"], i));
      materialBufferPtr[mat_index] = pbr_material;
    }

    m_pMaterials[material_name_hash].index = mat_index;
    m_pMaterials[material_name_hash].material = pbr_material;
    m_pMaterials[material_name_hash].name = material_name;

    if (!material.albedo_path.empty()) {
      m_pMaterials[material_name_hash].Slot[(int)TextureSlot::Albedo] =
          std::hash<std::string>{}(material.albedo_path);
    }
    if (!material.metallic_path.empty()) {
      m_pMaterials[material_name_hash].Slot[(int)TextureSlot::Metallic] =
          std::hash<std::string>{}(material.metallic_path);
    }
    if (!material.roughness_path.empty()) {
      m_pMaterials[material_name_hash].Slot[(int)TextureSlot::Roughness] =
          std::hash<std::string>{}(material.roughness_path);
    }
    if (!material.normal_path.empty()) {
      m_pMaterials[material_name_hash].Slot[(int)TextureSlot::Normal] =
          std::hash<std::string>{}(material.normal_path);
    }

    material_index++;
  }
}

void AssetManager::ModifyModel(RenderModel) {}

void AssetManager::DeleteModel(RenderModel model) {

  auto resourceManager = m_pContext->ResourceManager();

  for (auto &mesh : model.renderMeshes[0]) {

    resourceManager->destroyMesh(mesh.mesh);
  }

  m_pModels.erase(model.name);
}

RenderModel AssetManager::createModelFromMesh(
    const std::string &name, const common::Mesh &mesh,
    std::initializer_list<common::MeshMaterial> materials) {

  auto resourceManager = m_pContext->ResourceManager();

  if (m_pModels.contains(name)) {
    for (auto &mesh : m_pModels.at(name).renderMeshes[0]) {
      mesh.id = mesh_counter;
      mesh_counter++;
    }
    return m_pModels.at(name);
  }

  RenderModel &model = m_pModels[name];
  model.name = name;
  RenderMesh subRenderC;
  subRenderC.numMaterials = materials.size();
  subRenderC.mesh = addMesh(name, mesh);
  subRenderC.id = mesh_counter;
  mesh_counter++;
  unsigned int material_index = 0;

  ///.... here materiasl

  pLoadMaterials(name, subRenderC, materials);

  if (subRenderC.numMaterials == 0) {
    subRenderC.numMaterials = 1;
    std::string material_name =
        name + "_material" + std::to_string(material_index);
    std::size_t material_name_hash = std::hash<std::string>{}(material_name);
    subRenderC.material_keys[material_index] = material_name_hash;

    if (!m_pMaterials.contains(material_name_hash)) {

      uint32_t mat_index = 0;
      if (!m_pFreeMaterialIndexes.empty()) {
        mat_index = m_pFreeMaterialIndexes.front();
        m_pFreeMaterialIndexes.pop();
      } else {
        mat_index = m_pMaterialCurrentIndex;
        m_pMaterialCurrentIndex++;
      }
      PBRMaterial pbr_material;
      m_pMaterials[material_name_hash].index = mat_index;
      m_pMaterials[material_name_hash].material = pbr_material;

      m_pMaterials[material_name_hash].name = material_name;
    }
  }

  model.renderMeshes[0].push_back(subRenderC);

  return model;
}

RenderModel
AssetManager::createModelFromMesh(const std::string &name,
                                  const common::Mesh &mesh,
                                  const common::MeshMaterial &material) {

  auto resourceManager = m_pContext->ResourceManager();

  if (m_pModels.contains(name)) {
    for (auto &mesh : m_pModels.at(name).renderMeshes[0]) {
      mesh.id = mesh_counter;
      mesh_counter++;
    }
    return m_pModels.at(name);
  }

  RenderModel &model = m_pModels[name];
  model.name = name;
  std::string material_name;
  std::size_t material_name_hash;

  if (material.name.empty())
    material_name = name + "_material";
  else {
    material_name = material.name;
  }

  material_name_hash = std::hash<std::string>{}(material_name);

  RenderMesh subRenderC;
  subRenderC.material_keys[0] = material_name_hash;
  subRenderC.numMaterials = 1;
  subRenderC.mesh = addMesh(name, mesh);
  subRenderC.id = mesh_counter;
  mesh_counter++;
  model.renderMeshes[0].push_back(subRenderC);

  if (m_pMaterials.contains(material_name_hash)) {
    return model;
  }
  PBRMaterial pbr_material;

  pbr_material.albedo_color = material.color;
  pbr_material.metallic_color = material.metallic;
  pbr_material.roughness_color = material.roughness;
  pbr_material.specular = material.specular;
  pbr_material.mask = (MeshMask)material.mask;

  if (!material.albedo_path.empty()) {
    auto albedo = addTexture(
        material.albedo_path,
        {.format = COLOR_RGBA, .wrapMode = REPEAT, .filtering = LINEAR});
    pbr_material.albedo_texture_index = albedo.Index();
  }
  if (!material.metallic_path.empty()) {
    auto metallic = addTexture(
        material.metallic_path,
        {.format = NON_COLOR_RGBA, .wrapMode = REPEAT, .filtering = LINEAR});
    pbr_material.metallic_texture_index = metallic.Index();
  }
  if (!material.roughness_path.empty()) {
    auto roughness = addTexture(
        material.roughness_path,
        {.format = NON_COLOR_RGBA, .wrapMode = REPEAT, .filtering = LINEAR});
    pbr_material.roughness_texture_index = roughness.Index();
  }
  if (!material.normal_path.empty()) {
    auto normal = addTexture(
        material.normal_path,
        {.format = NON_COLOR_RGBA, .wrapMode = REPEAT, .filtering = LINEAR});
    pbr_material.normal_texture_index = normal.Index();
  }

  uint32_t mat_index = 0;
  if (!m_pFreeMaterialIndexes.empty()) {
    mat_index = m_pFreeMaterialIndexes.front();
    m_pFreeMaterialIndexes.pop();
  } else {
    mat_index = m_pMaterialCurrentIndex;
    m_pMaterialCurrentIndex++;
  }

  for (int i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++) {
    PBRMaterial *materialBufferPtr = reinterpret_cast<PBRMaterial *>(
        resourceManager->mapBuffer(m_pGPUBuffers["material_buffer"], i));
    materialBufferPtr[mat_index] = pbr_material;
  }

  m_pMaterials[material_name_hash].index = mat_index;
  m_pMaterials[material_name_hash].material = pbr_material;

  m_pMaterials[material_name_hash].name = material_name;

  if (!material.albedo_path.empty()) {
    m_pMaterials[material_name_hash].Slot[(int)TextureSlot::Albedo] =
        std::hash<std::string>{}(material.albedo_path);
  }
  if (!material.metallic_path.empty()) {
    m_pMaterials[material_name_hash].Slot[(int)TextureSlot::Metallic] =
        std::hash<std::string>{}(material.metallic_path);
  }
  if (!material.roughness_path.empty()) {
    m_pMaterials[material_name_hash].Slot[(int)TextureSlot::Roughness] =
        std::hash<std::string>{}(material.roughness_path);
  }
  if (!material.normal_path.empty()) {
    m_pMaterials[material_name_hash].Slot[(int)TextureSlot::Normal] =
        std::hash<std::string>{}(material.normal_path);
  }

  return model;
}

RenderModel &AssetManager::getModel(const std::string &name) {
  return m_pModels.at(name);
}

uint32_t AssetManager::getAnimationIndex() {
  uint32_t anim_index = 0;
  if (m_pFreeAnimationIndices.size() > 0) {
    anim_index = m_pFreeAnimationIndices.top();
    m_pFreeAnimationIndices.pop();
  } else {
    anim_index = m_pCurrentAnimationIndex;
    m_pCurrentAnimationIndex++;
  }

  return anim_index;
}

void AssetManager::FreeAnimationIndex(uint32_t index) {
  m_pFreeAnimationIndices.push(index);
}
RenderModel &AssetManager::loadModel(
    const std::string &path, const std::string &name ,
    std::initializer_list<common::MeshMaterial> materials ) {
  std::string prefix = "./assets/";
  std::string model_name = name;

  if (path.find(":") != std::string::npos || path.find("/") == 0) {
    prefix = "";
  }

  std::string model_path = prefix + path;

  if (name == "")
    model_name = path;

  auto resourceManager = m_pContext->ResourceManager();
  if (m_pModels.contains(model_name)) {
    for (auto &mesh : m_pModels.at(model_name).renderMeshes[0]) {
      mesh.id = mesh_counter;
      mesh_counter++;
    }

    if (m_pModels.at(model_name).hasAnimation) {
      m_pModels.at(model_name).animationIndex = getAnimationIndex();
    }

    return m_pModels.at(model_name);
  }

  // if()
  const std::string ext =
      model_path.substr(model_path.find_last_of('.') + 1, model_path.length());

  std::unique_ptr<framework::gltfModel> inModel;

  /*if (ext == "eml")
  {
          inModel = std::make_unique<framework::Model>(model_path);
  }
  else
  {*/
  inModel = std::make_unique<framework::gltfModel>(model_path);
  //}

  int index = 0;
  RenderModel &model = m_pModels[model_name];

  model.hasAnimation = inModel->HasAnimation();

  if (model.hasAnimation) {
    model.animationIndex = getAnimationIndex();
  }

  model.name = model_name;
  model.animations = inModel->Animations();
  model.skeleton = inModel->getSkeleton();
  model.min_max_points = inModel->getMinMax();
  for (auto &mesh : inModel->Meshes()) {
    if (mesh.data().mesh.Indices.size() <= 0)
      continue;

    RenderMesh subRenderC;
    if (inModel->HasAnimation()) {
      subRenderC.mesh =
          addMesh(model_name + "_submesh_" + std::to_string(index),
                  inModel->AnimatedMeshes().at(index));
    } else {
      subRenderC.mesh = addMesh(
          model_name + "_submesh_" + std::to_string(index), mesh.data().mesh);
    }

    std::string material_name =
        model_name + "_submesh_" + std::to_string(index) + "_material";
    subRenderC.numMaterials = 1;
    subRenderC.material_keys[0] = std::hash<std::string>{}(material_name);
    subRenderC.id = mesh_counter;
    mesh_counter++;

    std::size_t material_name_hash = std::hash<std::string>{}(material_name);

    if (m_pMaterials.contains(material_name_hash)) {
      model.renderMeshes[0].push_back(subRenderC);
      continue;
    }

    if (materials.size() == 0) {
      auto mesh_material = mesh.Material();

      PBRMaterial material = loadModelMaterials(mesh_material);

      m_pMaterials[material_name_hash].material = material;
      m_pMaterials[material_name_hash].name = material_name;

      if (!mesh_material.albedo.empty()) {
        m_pMaterials[material_name_hash].Slot[(int)TextureSlot::Albedo] =
            std::hash<std::string>{}(mesh_material.albedo);
      }
      if (!mesh_material.metallic.empty()) {
        m_pMaterials[material_name_hash].Slot[(int)TextureSlot::Metallic] =
            std::hash<std::string>{}(mesh_material.metallic);
      }
      if (!mesh_material.roughness.empty()) {
        m_pMaterials[material_name_hash].Slot[(int)TextureSlot::Roughness] =
            std::hash<std::string>{}(mesh_material.roughness);
      }
      if (!mesh_material.normal.empty()) {
        m_pMaterials[material_name_hash].Slot[(int)TextureSlot::Normal] =
            std::hash<std::string>{}(mesh_material.normal);
      }

      subRenderC.material_keys[0] = material_name_hash;
      subRenderC.numMaterials = 1;

      uint32_t mat_index = 0;
      if (!m_pFreeMaterialIndexes.empty()) {
        mat_index = m_pFreeMaterialIndexes.front();
        m_pFreeMaterialIndexes.pop();
      } else {
        mat_index = m_pMaterialCurrentIndex;
        m_pMaterialCurrentIndex++;
      }

      m_pMaterials[material_name_hash].index = mat_index;

      for (int i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++) {
        PBRMaterial *materialBufferPtr = reinterpret_cast<PBRMaterial *>(
            resourceManager->mapBuffer(m_pGPUBuffers["material_buffer"], i));
        materialBufferPtr[mat_index] = material;
      }
    } else {

      subRenderC.numMaterials = materials.size();
      pLoadMaterials(model_name, subRenderC, materials);
    }

    model.renderMeshes[0].push_back(subRenderC);

    index++;
  }
  model.loaded = true;

  //
  return model;
}

PBRMaterial
AssetManager::loadModelMaterials(framework::Mesh::MeshMaterial &mesh_material) {
  PBRMaterial material;

  material.albedo_color = mesh_material.color;
  material.roughness_color = mesh_material.roughness_color;
  material.metallic_color = mesh_material.metallic_color;
  material.mask = MeshMask::PROP;
  // material.transmission = mesh_material.transmission;

  if (!mesh_material.albedo.empty()) {
    auto albedo = addTexture(
        mesh_material.albedo,
        {.format = COLOR_RGBA, .wrapMode = REPEAT, .filtering = LINEAR});

    if (albedo.empty())
      material.albedo_texture_index = -1;
    else
      material.albedo_texture_index = albedo.Index();
  } else {
    material.albedo_texture_index = -1;
  }
  if (!mesh_material.metallic.empty()) {
    auto metallic = addTexture(
        mesh_material.metallic,
        {.format = NON_COLOR_RGBA, .wrapMode = REPEAT, .filtering = LINEAR});

    if (metallic.empty())
      material.metallic_texture_index = -1;
    else
      material.metallic_texture_index = metallic.Index();
  } else {
    material.metallic_texture_index = -1;
  }
  if (!mesh_material.roughness.empty()) {
    auto roughness = addTexture(
        mesh_material.roughness,
        {.format = NON_COLOR_RGBA, .wrapMode = REPEAT, .filtering = LINEAR});

    if (roughness.empty())
      material.roughness_texture_index = -1;
    else
      material.roughness_texture_index = roughness.Index();
  } else {
    material.roughness_texture_index = -1;
  }
  if (!mesh_material.normal.empty()) {
    auto normal = addTexture(
        mesh_material.normal,
        {.format = NON_COLOR_RGBA, .wrapMode = REPEAT, .filtering = LINEAR});

    if (normal.empty())
      material.normal_texture_index = -1;
    else
      material.normal_texture_index = normal.Index();
  } else {
    material.normal_texture_index = -1;
  }

  return material;
}

AudioObject AssetManager::loadAudio(const std::string &path,
                                    int numSources ) {

  auto audioManager = m_pContext->AudioManager();

  if (m_pAudioBuffers.contains(path)) {
    AudioBuffer &buffer = m_pAudioBuffers.at(path);

    AudioObject object;
    object.buffer = buffer;

    for (int i = 0; i < numSources; i++) {
      std::string name = std::format(
          "{}_source_{}_{}_{}", path, i, buffer.sources.size(),
          framework::Clock::Now()); // path + "_source_" + std::to_string(i) +
                                    // "_" +
                                    // std::to_string(buffer.sources.size());
      buffer.sources.push_back(audioManager->createSource(
          name, {.buffer = buffer.buffer, .name = name}));
      object.source.push_back(buffer.sources.back());
    }

    return object;
  }
  framework::WAVfile audioFile;
  audioFile.Load(("./assets/" + path).c_str());

  auto buffer = audioManager->createBuffer(
      path, {
                .numChannels = audioFile.getNumberOfChannels(),
                .size = audioFile.getFileSize(),
                .bps = audioFile.getBPS(),
                .bitrate = audioFile.getSampleRate(),
                .data = audioFile.data(),
            });

  AudioBuffer &audioBuffer = m_pAudioBuffers[path];
  audioBuffer.buffer = buffer;

  for (int i = 0; i < numSources; i++) {
    std::string name =
        std::format("{}_source_{}_{}_{}", path, i, audioBuffer.sources.size(),
                    framework::Clock::Now()); //
    audioBuffer.sources.push_back(audioManager->createSource(
        name, {.buffer = audioBuffer.buffer, .name = name}));
  }

  AudioObject object;
  object.buffer = audioBuffer;
  object.source = audioBuffer.sources;
  object.name = path;

  delete[] audioFile.data();

  return object;
}

uint32_t AssetManager::getShaderDataIndex() {

  uint32_t obj_index = 0;
  if (!m_pFreeObjectIndexes.empty()) {
    obj_index = m_pFreeObjectIndexes.front();
    m_pFreeObjectIndexes.pop();
  } else {
    obj_index = m_pObjectCurrentIndex;
    m_pObjectCurrentIndex++;
  }

  return obj_index;
}

Ref<Texture> AssetManager::addTextureFromBytes(const std::string &name,
                                               unsigned char *bytes,
                                               std::size_t size, int w, int h,
                                               int c, const TextureInfo &info) {

  std::vector<std::size_t> offsets;
  unsigned int format = info.format;
  bool isKTX = false;
  int mipLevels = 0;
  Ref<Texture> ref = Ref<Texture>::makeEmpty();
  if (bytes != nullptr) {
    engine::TextureCreationInfo texInfo = {
        .width = (uint32_t)w,
        .height = (uint32_t)h,
        .numChannels = (uint32_t)c,
        .format = (engine::TextureFormat)format,
        .wrapMode = info.wrapMode,
        .filtering = info.filtering,
        .name = name,
        .isCompressed = false,
        .isKTX = isKTX,
        .mipLevels = (uint32_t)mipLevels,
        .offsets = offsets,
        .size = size,
        .pixels = bytes,
    };

    auto resourceManager = m_pContext->ResourceManager();
    ref = resourceManager->createTexture(texInfo);

    m_pContext->Renderer()->getDebugRenderer()->addTexture(ref);
  }

  m_pImages[std::hash<std::string>{}(name)] = ref;
  m_pImageName[std::hash<std::string>{}(name)] = name;

  return ref;
}

Ref<Texture> AssetManager::addTexture(const std::string &path,
                                      const TextureInfo &info) {

  auto resourceManager = m_pContext->ResourceManager();

  if (m_pImages.contains(std::hash<std::string>{}(path))) {
    return m_pImages.at(std::hash<std::string>{}(path));
  }

  int width, height, num_channels, mipLevels = 1;
  std::size_t size = 0;
  bool isCompressed = false;
  std::string texture_path = "./assets/" + path;

  const std::string ext =
      path.substr(path.find_last_of('.') + 1, path.length());
  unsigned char *pixels;
  // ktxTexture *ktxTex;
  // ktxTexture2 *ktxTex2;
  std::vector<std::size_t> offsets;
  unsigned int format = info.format;
  bool isKTX = false;
  bool isDDS = false;
  /*
  if (ext == "ktx")
  {
          ktxResult result = KTX_SUCCESS;

          result = ktxTexture_CreateFromNamedFile(texture_path.c_str(),
  KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktxTex);

          if (result != KTX_SUCCESS)
          {
                  throw std::runtime_error("Error loading texture: " +
  texture_path);
          }
          if (ktxTex->isCompressed)
          {
                  isCompressed = true;
          }
          pixels = ktxTexture_GetData(ktxTex);
          pixels = ktxTexture_GetData(ktxTex);
          ktx_std::size_t ktxTextureSize = ktxTexture_GetDataSize(ktxTex);

          width = ktxTex->baseWidth;
          height = ktxTex->baseHeight;
          mipLevels = ktxTex->numLevels;
          isKTX = true;
          size = ktxTextureSize;
          num_channels = 4;

          for (int i = 0; i < mipLevels; i++)
          {
                  ktx_std::size_t offset;
                  KTX_error_code result = ktxTexture_GetImageOffset(ktxTex, i,
  0, 0, &offset); offsets.push_back(offset);
          }
  }
  else if (ext == "ktx2")
  {
          ktxResult result = KTX_SUCCESS;

          result = ktxTexture2_CreateFromNamedFile(texture_path.c_str(),
  KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktxTex2);

          if (result != KTX_SUCCESS)
          {
                  throw std::runtime_error("Error loading texture: " +
  texture_path);
          }
          if (ktxTex2->isCompressed)
          {
                  isCompressed = true;
          }
          pixels = ktxTex2->pData;
          ktx_std::size_t ktxTextureSize = ktxTex2->dataSize;

          width = ktxTex2->baseWidth;
          height = ktxTex2->baseHeight;
          mipLevels = ktxTex2->numLevels;
          isKTX = true;
          size = ktxTextureSize;
          num_channels = 4;

          for (int i = 0; i < mipLevels; i++)
          {
                  ktx_std::size_t offset;

                  KTX_error_code result =
  ktxTexture_GetImageOffset(ktxTexture(ktxTex2), i, 0, 0, &offset);
                  offsets.push_back(offset);
          }
  }
  else */
  if (ext == "dds") {
    int baseLevel = 0;
    DDS ddsfile(texture_path, baseLevel);
    isDDS = true;
    width = ddsfile.width();
    height = ddsfile.height();

    baseLevel = std::min(ddsfile.mipLevels() - 1u, (uint32_t)baseLevel);
    mipLevels = std::max(ddsfile.mipLevels(), 1);
    size = ddsfile.size();
    num_channels = 4;
    isCompressed = true;
    unsigned int desired_format = format;
    if (desired_format == NON_COLOR_RGBA) {
      format = TextureFormat::NON_COLOR_RGBA_BC3;
    } else {
      format = ddsfile.format();
    }

    if (desired_format == NON_COLOR_RGBA &&
        ddsfile.format() == COLOR_RGBA_BC7) {
      format = NON_COLOR_RGBA_BC7;
    } else if (desired_format == COLOR_RGBA &&
               ddsfile.format() == NON_COLOR_RGBA_BC7) {
      format = COLOR_RGBA_BC7;
    } else if (desired_format == NON_COLOR_RGBA &&
               ddsfile.format() == NON_COLOR_RGBA_BC7) {
      format = NON_COLOR_RGBA_BC7;
    } else if (desired_format == NON_COLOR_RGBA &&
               ddsfile.format() == NON_COLOR_RG_BC5U) {
      format = NON_COLOR_RG_BC5U;
    }
    pixels = ddsfile.data();
    std::size_t offset = 0;

    int w = width, h = height;
    unsigned int s = 0;
    for (unsigned int level = 0; level < mipLevels; ++level) {
      s = ((w + 3) / 4) * (((h + 3) / 4) * ddsfile.blockSize());
      offsets.push_back(offset);
      offset += s;
      w /= 2;
      h /= 2;

      if (w < 1)
        w = 1;
      if (h < 1)
        h = 1;
    }
  } else {
    pixels = framework::load_image_from_file(texture_path.c_str(), &width,
                                             &height, &num_channels);
  }
  Ref<Texture> ref = Ref<Texture>::makeEmpty();
  if (pixels != nullptr) {
    engine::TextureCreationInfo texInfo = {
        .width = (uint32_t)width,
        .height = (uint32_t)height,
        .numChannels = (uint32_t)num_channels,
        .format = (engine::TextureFormat)format,
        .wrapMode = info.wrapMode,
        .filtering = info.filtering,
        .name = path,
        .isCompressed = isCompressed,
        .isKTX = isKTX,
        .mipLevels = (uint32_t)mipLevels,
        .offsets = offsets,
        .size = size,
        .pixels = pixels,
    };

    ref = resourceManager->createTexture(texInfo);

    m_pContext->Renderer()->getDebugRenderer()->addTexture(ref);

    if (!isCompressed && !isDDS) {
      framework::free_image_data(pixels);
    } /*
     else {
             ktxTexture_Destroy(ktxTex);
             ktxTexture_Destroy(ktxTexture(ktxTex2));
     }*/

    if (isDDS) {
      DDS::free_data(pixels);
    }

    m_pImages[std::hash<std::string>{}(path)] = ref;
    m_pImageName[std::hash<std::string>{}(path)] = path;
  }

  return ref;
}

Ref<Mesh> AssetManager::addMesh(const std::string &name, common::Mesh mesh) {

  auto resourceManager = m_pContext->ResourceManager();

  auto ref = resourceManager->createMesh(
      {.vertices = mesh.Vertices, .indices = mesh.Indices, .name = name});

  return ref;
}

Ref<Mesh> AssetManager::addMesh(const std::string &name,
                                common::AnimatedMesh mesh) {

  auto resourceManager = m_pContext->ResourceManager();

  auto ref = resourceManager->createMesh(
      {.vertices = mesh.Vertices, .indices = mesh.Indices, .name = name});

  return ref;
}

std::unordered_map<std::size_t, PBRMaterialIndex> &
AssetManager::getMaterials() {
  return m_pMaterials;
}

PBRMaterialIndex &AssetManager::getMaterial(std::size_t key) {
  return m_pMaterials.at(key);
}

void AssetManager::setMaterial(std::size_t key, PBRMaterialIndex material) {
  m_pMaterials.at(key) = material;
}

std::unordered_map<std::string, ShaderAsset> AssetManager::getShaders() {
  return m_pShaders;
}

std::size_t AssetManager::createMaterial(const std::string &name,
                                         PBRMaterial material) {

  auto resourceManager = m_pContext->ResourceManager();
  uint32_t mat_index = 0;
  if (!m_pFreeMaterialIndexes.empty()) {
    mat_index = m_pFreeMaterialIndexes.front();
    m_pFreeMaterialIndexes.pop();
  } else {
    mat_index = m_pMaterialCurrentIndex;
    m_pMaterialCurrentIndex++;
  }

  std::size_t material_name_hash = std::hash<std::string>{}(name);
  m_pMaterials[material_name_hash].index = mat_index;

  for (int i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++) {
    PBRMaterial *materialBufferPtr = reinterpret_cast<PBRMaterial *>(
        resourceManager->mapBuffer(m_pGPUBuffers["material_buffer"], i));
    materialBufferPtr[mat_index] = material;
    resourceManager->unmapBuffer(m_pGPUBuffers["material_buffer"], i);
  }

  return material_name_hash;
}

void AssetManager::setMaterial(PBRMaterialIndex material) {

  auto resourceManager = m_pContext->ResourceManager();

  for (int i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++) {
    PBRMaterial *materialBufferPtr = reinterpret_cast<PBRMaterial *>(
        resourceManager->mapBuffer(m_pGPUBuffers["material_buffer"], i));
    materialBufferPtr[material.index] = material.material;
    resourceManager->unmapBuffer(m_pGPUBuffers["material_buffer"], i);
  }
}
} // namespace engine