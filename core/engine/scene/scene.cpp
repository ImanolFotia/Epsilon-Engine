#include "scene.hpp"

namespace engine {

std::shared_ptr<OctreeContainer<Scene::OctreeRenderType>> Scene::getRenderOctree() { return m_pRenderOctree; }

Scene::Scene(std::shared_ptr<Context> context) {
  // m_pAssetManager.m_pContext = m_pContext;
  m_pContext = context;

  float octree_size = 256;
  float octree_half_size = octree_size * 0.5;
  m_pNodeOctree = std::make_shared<OctreeContainer<OctreeNodeType>>(
      Box{glm::vec3(-octree_half_size, -25.0, -octree_half_size), glm::vec3(octree_size, 100, octree_size)}, 0);
  m_pRenderOctree = std::make_shared<OctreeContainer<OctreeRenderType>>(
      Box{glm::vec3(-octree_half_size, -25.0, -octree_half_size), glm::vec3(octree_size, 100, octree_size)}, 0);

  // m_pAssetManager.Init();
}

std::shared_ptr<Context> Scene::getContext() { return m_pContext; }

void Scene::setAssetManager(std::shared_ptr<engine::AssetManager> assetManager) { m_pAssetManager = assetManager; }

void Scene::Init() { auto resourceManager = m_pContext->ResourceManager(); }

void Scene::setCurrentRenderPass(engine::Ref<engine::RenderPass> renderPassRef) {
  auto renderer = m_pContext->Renderer();
  //renderer->SetRenderPass(renderPassRef);
  m_pCurrentRenderPass = renderPassRef;
}

std::shared_ptr<AssetManager> &Scene::getAssetManager() { return m_pAssetManager; }

void Scene::SetViewport(const Viewport &viewport) {

  auto renderer = m_pContext->Renderer();
  renderer->SetViewport(viewport);
}

void Scene::SetScissor(const Scissor &scissor) {

  auto renderer = m_pContext->Renderer();
  renderer->SetScissor(scissor);
}

void Scene::UpdateShadowFrustum(glm::mat4 proj, glm::mat4 view) { m_pShadowFrustum.CalculateFrustum(proj * view, glm::mat4(1.0)); }

void Scene::UpdateFrustum(glm::mat4 proj, glm::mat4 view) { m_pFrustum.CalculateFrustum(proj * view, glm::mat4(1.0)); }

Frustum &Scene::getFrustum() { return m_pFrustum; }

Scene::CullResultType &Scene::Cull(float cutout_distance, int pass, OctreeItemMask mask) {
  return m_pRenderOctree->search(m_pFrustum, (OctreeContainer<OctreeRenderType>::CullPass)pass, mask);
}

Scene::CullResultType &Scene::Cull(Box &box, int pass, OctreeItemMask mask) {
  return m_pRenderOctree->search(box, (OctreeContainer<OctreeRenderType>::CullPass)pass, mask);
}

Scene::CullResultType &Scene::Cull(BoundingSphere &sphere, int pass, OctreeItemMask mask) {
  return m_pRenderOctree->search(sphere, (OctreeContainer<OctreeRenderType>::CullPass)pass, mask);
}

Scene::CullResultType &Scene::CullShadow(float cutout_distance, int pass, OctreeItemMask mask) {
  return m_pRenderOctree->search(m_pShadowFrustum, (OctreeContainer<OctreeRenderType>::CullPass)pass, mask);
}

Scene::CullResultType &Scene::CullShadow(Box box, int pass, OctreeItemMask mask) {
  return m_pRenderOctree->search(box, (OctreeContainer<OctreeRenderType>::CullPass)pass, mask);
}

void Scene::RelocateObject(Box boundingBox, int index) {

  std::shared_ptr<Node<RenderModel>> node = std::static_pointer_cast<Node<RenderModel>>(m_pSceneManager.get(index));
  if (getChildren(node->Parent())[typeid(typename std::list<OctreeItem<OctreeRenderType>>::iterator).hash_code()].size() > 0) {
    using render_node_type = engine::Node<typename std::list<OctreeItem<OctreeRenderType>>::iterator>;
    std::shared_ptr<render_node_type> octree_render_node = getChild<typename std::list<OctreeItem<OctreeRenderType>>::iterator>(node->Parent());
    if (octree_render_node != nullptr) {
      m_pRenderOctree->relocate(octree_render_node->data, boundingBox);
    }
  }
}

Scene::OctreeRenderItem Scene::insertIntoOctree(Box boundingBox, OctreeSceneItem item, OctreeItemMask mask) {
  std::list<OctreeItem<OctreeRenderType>>::iterator octree_node = m_pRenderOctree->insert(boundingBox, item);
  octree_node->mask = mask;
  return octree_node;
}

const auto &Scene::getNodes() { return m_pSceneManager.get<Node<Root>>(); }

std::shared_ptr<NodeBase> Scene::getNode(uint32_t index) { return m_pSceneManager.get(index); }

SceneManager::ChildNodes &Scene::getChildren(std::shared_ptr<NodeBase> parent) { return m_pSceneManager.getChildren(parent); }

void Scene::BeginScene() {
  if (m_pContext->Window().getSize().width > 0) {
    auto renderer = m_pContext->Renderer();
    renderer->BeginFrame();
    renderer->Begin();

    m_pMeshCount = 0;
  }
}

void Scene::Flush(engine::DrawType drawType, bool clean_attachments) {

  if (m_pContext->Window().getSize().width > 0) {
    auto renderer = m_pContext->Renderer();

    renderer->Flush({.renderPassRef = m_pCurrentRenderPass, .type = drawType, .frameBufferIndex = 0, .clean_attachments = clean_attachments});
  }
}

uint32_t Scene::Push(std::shared_ptr<Node<RenderModel>> renderModel, const glm::mat4 &transform, engine::parsers::RenderLayout layout) {
  uint32_t push_index = m_pMeshCount;
  if (m_pContext->Window().getSize().width > 0) {
    auto renderer = m_pContext->Renderer();

    Ref<PushConstant> push_constant = engine::Ref<engine::PushConstant>::makeEmpty();

    auto &renderLayout = layout;

    Ref<BindGroup> selectedBindGroup = renderLayout.bindGroup;

    auto transform_buffer = m_pAssetManager->getTransformBuffer();
    auto object_buffer = m_pAssetManager->getObjectBuffer();

    for (auto &mesh : renderModel->data.renderMeshes[0]) {
      if (renderer->numPushedCommands() >= engine::MAX_COMMAND_QUEUE_SIZE) {
        Flush();
      }

      uint32_t material_indices[4] = {};

      for (int i = 0; i < mesh.numMaterials; i++) {
        uint32_t uniform_index = m_pAssetManager->m_pMaterials.at(mesh.material_keys[i]).index;

        material_indices[i] = uniform_index;
      }

      transform_buffer[m_pMeshCount] = transform;

      object_buffer[m_pMeshCount] = {.object_id = (unsigned int)renderModel->Parent()->Index(),
                                     .transform_index = m_pMeshCount,
                                     .material_index = {material_indices[0], material_indices[1], material_indices[2], material_indices[3]},
                                     .numMaterials = (uint32_t)mesh.numMaterials,
                                     .animationIndex = renderModel->data.animationIndex};

      renderer->Push({.mesh = mesh.mesh,
                      .material = selectedBindGroup,
                      .pushConstant = push_constant,
                      .objectConstant = {.transform = transform, .material_index = material_indices[0], .animation_offset = renderModel->data.animationIndex},
                      .layout_index = renderLayout.pipelineLayoutIndex,
                      .uniformIndex = m_pMeshCount});
      m_pMeshCount++;
    }
  }

  return push_index;
}

uint32_t Scene::Push(const std::vector<glm::mat4> &transforms, engine::parsers::RenderLayout layout, const std::string &material, unsigned int count) {

  uint32_t push_index = m_pMeshCount;
  if (m_pContext->Window().getSize().width > 0) {
    auto renderer = m_pContext->Renderer();

    Ref<PushConstant> push_constant;

    Ref<BindGroup> selectedBindGroup = layout.bindGroup;
    auto transform_buffer = m_pAssetManager->getTransformBuffer();
    auto object_buffer = m_pAssetManager->getObjectBuffer();

    uint32_t material_indices[4] = {0};
    uint32_t firstInstance = m_pMeshCount;
    for (int i = 0; i < count; i++) {
      if (renderer->numPushedCommands() >= engine::MAX_COMMAND_QUEUE_SIZE) {
        Flush();
      }

      for (int j = 0; j < 4; j++) {
        uint32_t uniform_index = m_pAssetManager->m_pMaterials.at(std::hash<std::string>{}(material)).index;
        material_indices[j] = uniform_index;
      }

      transform_buffer[m_pMeshCount] = transforms[i];

      object_buffer[m_pMeshCount] = {.object_id = (unsigned int)i,
                                     .transform_index = m_pMeshCount,
                                     .material_index = {material_indices[0], material_indices[1], material_indices[2], material_indices[3]},
                                     .numMaterials = (uint32_t)1,
                                     .animationIndex = 0};

      m_pMeshCount++;
    }

    renderer->Push({.mesh = engine::Ref<Mesh>().makeEmpty(),
                    .material = selectedBindGroup,
                    .pushConstant = push_constant,
                    .objectConstant = {.transform = transforms[0], .material_index = material_indices[0]},
                    .layout_index = layout.pipelineLayoutIndex,
                    .uniformIndex = firstInstance,
                    .count = count});
  }

  return push_index;
}

uint32_t Scene::Push(const RenderArgs &renderArgs) {

  uint32_t push_index = m_pMeshCount;

  if (m_pContext->Window().getSize().width <= 0)
    return push_index;

  auto renderer = m_pContext->Renderer();
  Ref<PushConstant> push_constant;
  auto &renderLayout = renderArgs.layout;
  Ref<BindGroup> selectedBindGroup = renderLayout.bindGroup;
  auto transform_buffer = m_pAssetManager->getTransformBuffer();
  auto object_buffer = m_pAssetManager->getObjectBuffer();
  int submeshCount = renderArgs.submeshCount >= 1 ? renderArgs.submeshCount : renderArgs.renderModel->data.renderMeshes[0].size();
  auto &meshCollection = renderArgs.renderModel->data.renderMeshes[0];

  for (int i = 0; i < submeshCount; i++) {
    engine::RenderMesh *mesh = nullptr;
    mesh = &meshCollection[i];
    if (renderArgs.submeshCount >= 1)
      mesh = &meshCollection[renderArgs.subMeshes[i]];

    uint32_t material_indices[4] = {0};
    uint32_t firstInstance = m_pMeshCount;
    transform_buffer[m_pMeshCount] = renderArgs.transform;
    for (int j = 0; j < renderArgs.count; j++) {

      if (renderer->numPushedCommands() >= engine::MAX_COMMAND_QUEUE_SIZE)
        Flush();

      uint32_t material_indices[4] = {0};
      for (int k = 0; k < mesh->numMaterials; k++) {
        uint32_t uniform_index = m_pAssetManager->m_pMaterials.at(mesh->material_keys[k]).index;
        material_indices[k] = uniform_index;
      }

      if (renderArgs.count > 1)
        transform_buffer[m_pMeshCount] = renderArgs.transforms[j];

      object_buffer[m_pMeshCount] = {.object_id = (unsigned int)renderArgs.renderModel->Parent()->Index() + j,
                                     .transform_index = m_pMeshCount,
                                     .material_index = {material_indices[0], material_indices[1], material_indices[2], material_indices[3]},
                                     .numMaterials = (uint32_t)mesh->numMaterials,
                                     .animationIndex = renderArgs.renderModel->data.animationIndex};
      m_pMeshCount++;
    }
    renderer->Push({.mesh = mesh->mesh,
                    .material = selectedBindGroup,
                    .pushConstant = push_constant,
                    .objectConstant = {.transform = renderArgs.transform,
                                       .material_index = material_indices[0],
                                       .animation_offset = renderArgs.renderModel->data.animationIndex},
                    .layout_index = renderLayout.pipelineLayoutIndex,
                    .uniformIndex = firstInstance,
                    .count = renderArgs.count});
  }

  return push_index;
}

uint32_t Scene::Push(std::shared_ptr<Node<RenderModel>> renderModel, const std::vector<glm::mat4> &transforms, unsigned int count,
                     engine::parsers::RenderLayout layout) {
  uint32_t push_index = m_pMeshCount;
  if (m_pContext->Window().getSize().width > 0) {
    auto renderer = m_pContext->Renderer();

    Ref<PushConstant> push_constant;

    Ref<BindGroup> selectedBindGroup = layout.bindGroup;

    if (count > 1) {
    }

    auto transform_buffer = m_pAssetManager->getTransformBuffer();
    auto object_buffer = m_pAssetManager->getObjectBuffer();

    for (auto &mesh : renderModel->data.renderMeshes[0]) {
      uint32_t material_indices[4] = {0};
      uint32_t firstInstance = m_pMeshCount;
      for (int i = 0; i < count; i++) {
        if (renderer->numPushedCommands() >= engine::MAX_COMMAND_QUEUE_SIZE) {
          Flush();
        }

        for (int j = 0; j < mesh.numMaterials; j++) {
          uint32_t uniform_index = m_pAssetManager->m_pMaterials.at(mesh.material_keys[j]).index;
          material_indices[j] = uniform_index;
        }

        transform_buffer[m_pMeshCount] = transforms[i];

        object_buffer[m_pMeshCount] = {.object_id = (unsigned int)renderModel->Parent()->Index() + i,
                                       .transform_index = m_pMeshCount,
                                       .material_index = {material_indices[0], material_indices[1], material_indices[2], material_indices[3]},
                                       .numMaterials = (uint32_t)mesh.numMaterials,
                                       .animationIndex = renderModel->data.animationIndex};

        m_pMeshCount++;
      }

      renderer->Push(
          {.mesh = mesh.mesh,
           .material = selectedBindGroup,
           .pushConstant = push_constant,
           .objectConstant = {.transform = transforms[0], .material_index = material_indices[0], .animation_offset = renderModel->data.animationIndex},
           .layout_index = layout.pipelineLayoutIndex,
           .uniformIndex = firstInstance,
           .count = count});
    }
  }
  return push_index;
}

void Scene::ComputeDispatch(engine::Ref<ComputeShader> computeShader, Ref<BindGroup> bindGroup) {

  auto renderer = m_pContext->Renderer();
  renderer->ComputeDispatch(computeShader, bindGroup);
}

void Scene::ComputeDispatchAsync(engine::Ref<ComputeShader> computeShader, Ref<BindGroup> bindGroup) {

  auto renderer = m_pContext->Renderer();
  renderer->ComputeDispatch(computeShader, bindGroup);
}

void Scene::EndScene() {
  if (m_pContext->Window().getSize().width > 0) {
    auto renderer = m_pContext->Renderer();
    glm::vec3 v{};
    renderer->End(v);

    renderer->Submit();
    renderer->EndFrame();
  }

  m_pContext->AudioManager()->Update();
}

} // namespace engine