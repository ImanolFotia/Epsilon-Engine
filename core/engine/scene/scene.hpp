#pragma once

#include <span>

#include "asset_manager.hpp"
#include "nodes/node_manager.hpp"
#include <core/engine/context.hpp>

#include "parsers/renderpass.hpp"
#include "trees/octree.hpp"

#include "../audio/audio_manager.hpp"

#include "structs/frustum.hpp"
#include "trees/octree.hpp"

#ifdef _WIN32
#include <Windows.h>
#endif

namespace engine {
using RenderModelNodePtr = std::shared_ptr<Node<RenderModel>>;
struct EntityBehaviour {
  std::function<void(void)> callback;
  bool once = false;
  bool done = false;
};

struct OctreeSceneItem {
  OctreeSceneItem() = default;
  std::shared_ptr<Node<RenderModel>> renderModel;
  Box boundingBox{};
  int index;
  int instance_index;
  bool visible;
};

struct EntityData {
  alignas(32) glm::mat4 modelMatrix{};
  glm::ivec4 texture_indices;
};

struct Transform {
  unsigned int id = 0;
  glm::vec3 position = glm::vec3(0.0f);
  glm::vec3 prev_position = glm::vec3(0.0f);
  glm::vec3 scale = glm::vec3(1.0f);
  glm::vec3 prev_scale = glm::vec3(1.0f);
  glm::vec3 target_scale = glm::vec3(1.0f);
  float angle = 0;
  glm::quat orientation{};
  glm::mat4 model_matrix = glm::mat4(1.0f);
  float lifetime = 0;
  bool grows = false;
};

struct Entity {
  Entity() : loaded(false) {}
  int id = 0;
  bool loaded = false;

  Transform transform;
};

struct RenderArgs {
  std::shared_ptr<Node<RenderModel>> renderModel;
  glm::mat4 transform = glm::mat4(1.0f);
  engine::parsers::RenderLayout layout{};
  int submeshCount =
      -1; //-1 to render all submeshes, >1 to render selected submeshes
  std::vector<int> subMeshes{};
  std::string material{};
  unsigned int count = 1;
  std::span<glm::mat4> transforms;
};

class Scene {
public:
  using OctreeRenderType = OctreeSceneItem;
  using OctreeRenderItem =
      typename std::list<OctreeItem<OctreeRenderType>>::iterator;
  using OctreeNodeType = std::shared_ptr<NodeBase>;

private:
  std::shared_ptr<AssetManager> m_pAssetManager;
  SceneManager m_pSceneManager;

  Ref<RenderPass> m_pCurrentRenderPass;

  std::shared_ptr<OctreeContainer<OctreeRenderType>> m_pRenderOctree = nullptr;
  std::shared_ptr<OctreeContainer<OctreeNodeType>> m_pNodeOctree = nullptr;

  Frustum m_pFrustum{};
  Frustum m_pShadowFrustum{};

  std::shared_ptr<Context> m_pContext = nullptr;

  uint32_t m_pMeshCount = 0;

  std::size_t getHash(const std::string &s) {
    return std::hash<std::string>{}(s);
  }

public:
  struct SceneEntity {
    glm::mat4 transform;
  };

  Scene() = default;

  Scene(std::shared_ptr<Context> context) : m_pContext(context) {
    // m_pAssetManager.m_pContext = m_pContext;

    float octree_size = 256;
    float octree_half_size = octree_size * 0.5;
    m_pNodeOctree = std::make_shared<OctreeContainer<OctreeNodeType>>(
        Box{glm::vec3(-octree_half_size, -25.0, -octree_half_size),
            glm::vec3(octree_size, 100, octree_size)},
        0);
    m_pRenderOctree = std::make_shared<OctreeContainer<OctreeRenderType>>(
        Box{glm::vec3(-octree_half_size, -25.0, -octree_half_size),
            glm::vec3(octree_size, 100, octree_size)},
        0);

    // m_pAssetManager.Init();
  }

  std::shared_ptr<Context> getContext() { return m_pContext; }

  void setAssetManager(std::shared_ptr<engine::AssetManager> assetManager) {
    m_pAssetManager = assetManager;
  }

  void Init() {

    auto resourceManager = m_pContext->ResourceManager();
    /*
    #ifdef _WIN32
                            system("cd .\\assets\\shaders &&
    .\\build_shaders.sh"); #endif*/

    m_pContext->Renderer()->InitDebugRenderer();
  }

  void setCurrentRenderPass(engine::Ref<engine::RenderPass> renderPassRef) {
    auto renderer = m_pContext->Renderer();
    renderer->SetRenderPass(renderPassRef);
    m_pCurrentRenderPass = renderPassRef;
  }

  std::shared_ptr<AssetManager> &getAssetManager() { return m_pAssetManager; }

  void SetViewport(const Viewport &viewport) {

    auto renderer = m_pContext->Renderer();
    renderer->SetViewport(viewport);
  }

  void SetScissor(const Scissor &scissor) {

    auto renderer = m_pContext->Renderer();
    renderer->SetScissor(scissor);
  }

  void UpdateShadowFrustum(glm::mat4 proj, glm::mat4 view) {
    m_pShadowFrustum.CalculateFrustum(proj * view, glm::mat4(1.0));
  }

  void UpdateFrustum(glm::mat4 proj, glm::mat4 view) {
    m_pFrustum.CalculateFrustum(proj * view, glm::mat4(1.0));
  }

  Frustum &getFrustum() { return m_pFrustum; }

  auto &Cull(float cutout_distance = 0, int pass = 0) {
    return m_pRenderOctree->search(
        m_pFrustum, (OctreeContainer<OctreeRenderType>::CullPass)pass);
  }

  auto &Cull(Box &box, int pass = 0) {
    return m_pRenderOctree->search(
        box, (OctreeContainer<OctreeRenderType>::CullPass)pass);
  }

  auto &Cull(BoundingSphere &sphere, int pass = 0) {
    return m_pRenderOctree->search(
        sphere, (OctreeContainer<OctreeRenderType>::CullPass)pass);
  }

  auto &CullShadow(float cutout_distance = 0, int pass = 1) {
    return m_pRenderOctree->search(
        m_pShadowFrustum, (OctreeContainer<OctreeRenderType>::CullPass)pass);
  }

  auto &CullShadow(Box box, int pass = 1) {
    return m_pRenderOctree->search(
        box, (OctreeContainer<OctreeRenderType>::CullPass)pass);
  }

  void RelocateObject(Box boundingBox, int index) {

    std::shared_ptr<Node<RenderModel>> node =
        std::static_pointer_cast<Node<RenderModel>>(m_pSceneManager.get(index));
    if (getChildren(node->Parent())
            [typeid(typename std::list<OctreeItem<OctreeRenderType>>::iterator)]
                .size() > 0) {
      using render_node_type = engine::Node<
          typename std::list<OctreeItem<OctreeRenderType>>::iterator>;
      std::shared_ptr<render_node_type> octree_render_node =
          getChild<typename std::list<OctreeItem<OctreeRenderType>>::iterator>(
              node->Parent());
      if (octree_render_node != nullptr) {
        auto new_item =
            m_pRenderOctree->relocate(octree_render_node->data, boundingBox);
      }
    }
  }

  template <typename T> void removeFromScene(uint32_t index) {
    // std::cout << "removed index: " << index << std::endl;
    std::shared_ptr<Node<T>> node =
        std::static_pointer_cast<Node<T>>(m_pSceneManager.get(index));

    using render_node_type =
        engine::Node<std::list<OctreeItem<OctreeRenderType>>::iterator>;
    using octree_node_type =
        engine::Node<std::list<OctreeItem<OctreeNodeType>>::iterator>;
    auto &children = getChildren(node);
    auto &octree_render_node = children[typeid(
        typename std::list<OctreeItem<OctreeRenderType>>::iterator)];
    for (auto &render_node : octree_render_node) {

      if (render_node != nullptr) {
        auto r_node = std::static_pointer_cast<render_node_type>(render_node);
        m_pRenderOctree->erase(r_node->data);
      }
    }

    m_pSceneManager.erase<T>(node);
  }

  template <typename T> bool isOfType(std::shared_ptr<NodeBase> node) {
    return m_pSceneManager.isOfType<T>(node);
  }

  template <typename T> auto insertIntoScene(Box boundingBox, T object) {
    auto scene_node = m_pSceneManager.insert(m_pSceneManager.root, object);

    /*auto octree_node = m_pNodeOctree->insert(boundingBox, scene_node);

    m_pSceneManager.insert(scene_node, octree_node);
*/
    return scene_node;
  }

  template <typename T, class... Args>
  auto emplaceIntoScene(Box boundingBox, Args &&...args) {
    auto scene_node = m_pSceneManager.emplace<T>(m_pSceneManager.root,
                                                 std::forward<Args>(args)...);
    /*
    auto octree_node = m_pNodeOctree->insert(boundingBox, scene_node);

    m_pSceneManager.insert(scene_node, octree_node);
    * /

            return scene_node;
}

template <typename T>
auto emplaceIntoScene(Box boundingBox)
{
    std::shared_ptr<Node<T>> scene_node =
m_pSceneManager.emplace<T>(m_pSceneManager.root);
    /*auto octree_node = m_pNodeOctree->insert(boundingBox, scene_node);

    m_pSceneManager.insert(scene_node, octree_node);*/

    return scene_node;
  }

  template <typename P, typename T>
  auto insertIntoNode(std::shared_ptr<Node<P>> parent, T object) {
    auto node = m_pSceneManager.insert(parent, object);

    return node;
  }

  template <typename P, typename T>
  auto insertIntoNode(Box boundingBox, std::shared_ptr<Node<P>> parent,
                      T object) {
    auto node = m_pSceneManager.insert(parent, object);

    if (typeid(T) == typeid(RenderModel)) {
      OctreeSceneItem item;
      item.renderModel = node;
      item.index = node->Index();
      std::list<OctreeItem<OctreeRenderType>>::iterator octree_node =
          m_pRenderOctree->insert(boundingBox, item);

      insertIntoNode(parent, octree_node);
    }
    return node;
  }

  auto insertIntoOctree(Box boundingBox, OctreeSceneItem item) {
    std::list<OctreeItem<OctreeRenderType>>::iterator octree_node =
        m_pRenderOctree->insert(boundingBox, item);
    return octree_node;
  }

  template <typename T, typename P, class... Args>
  auto emplaceIntoNode(std::shared_ptr<Node<P>> parent, Args &&...args) {
    auto node = m_pSceneManager.emplace<T>(parent, std::forward<Args>(args)...);
    return node;
  }

  template <typename T, typename P, class... Args>
  auto emplaceIntoNode(Box boundingBox, std::shared_ptr<Node<P>> parent,
                       Args &&...args) {

    auto node = m_pSceneManager.emplace<T>(parent, std::forward<Args>(args)...);
    if (typeid(T) == typeid(RenderModel)) {

      OctreeSceneItem item;
      item.renderModel = node;
      item.index = node->Index();
      std::list<OctreeItem<OctreeRenderType>>::iterator octree_node =
          m_pRenderOctree->insert(boundingBox, item);
      insertIntoNode(parent, octree_node);
    }

    return node;
  }

  template <typename T, typename P>
  auto emplaceIntoNode(std::shared_ptr<Node<P>> parent) {
    auto node = m_pSceneManager.emplace<T>(parent);
    return node;
  }

  template <typename T, typename P>
  auto emplaceIntoNode(Box boundingBox, std::shared_ptr<Node<P>> parent) {
    auto node = m_pSceneManager.emplace<T>(parent);
    if (typeid(T) == typeid(RenderModel)) {
      OctreeSceneItem item;
      item.renderModel = node;
      item.index = node->Index();
      std::list<OctreeItem<OctreeRenderType>>::iterator octree_node =
          m_pRenderOctree->insert(boundingBox, item);
      insertIntoNode(parent, octree_node);
    }

    return node;
  }

  template <typename T> auto &getNodes() { return m_pSceneManager.get<T>(); }

  const auto &getNodes() { return m_pSceneManager.get<Node<Root>>(); }

  auto getNode(uint32_t index) { return m_pSceneManager.get(index); }

  template <typename T, typename P>
  std::shared_ptr<Node<T>> getChild(std::shared_ptr<Node<P>> parent) {
    return m_pSceneManager.to<T>(m_pSceneManager.getChild<T>(parent));
  }

  template <typename T>
  std::shared_ptr<Node<T>> getChild(std::shared_ptr<NodeBase> parent) {
    return m_pSceneManager.to<T>(m_pSceneManager.getChild<T>(parent));
  }

  template <typename T> std::shared_ptr<Node<T>> getChild() {
    return m_pSceneManager.to<T>(
        m_pSceneManager.getChild<T>(m_pSceneManager.root));
  }

  SceneManager::ChildNodes &getChildren(std::shared_ptr<NodeBase> parent) {
    return m_pSceneManager.getChildren(parent);
  }

  void BeginScene() {
    if (m_pContext->Window().getSize().width > 0) {
      auto renderer = m_pContext->Renderer();
      renderer->BeginFrame();
      renderer->Begin();

      m_pMeshCount = 0;
    }

    m_pContext->AudioManager()->Update();
  }

  void Flush(engine::DrawType drawType = engine::DrawType::INDEXED) {

    if (m_pContext->Window().getSize().width > 0) {
      auto renderer = m_pContext->Renderer();

      renderer->Flush(m_pCurrentRenderPass, drawType);
    }
  }

  uint32_t Push(std::shared_ptr<Node<RenderModel>> renderModel,
                const glm::mat4 &transform,
                engine::parsers::RenderLayout layout) {
    uint32_t push_index = m_pMeshCount;
    if (m_pContext->Window().getSize().width > 0) {
      auto renderer = m_pContext->Renderer();

      Ref<PushConstant> push_constant;

      auto &renderLayout = layout;

      Ref<BindGroup> selectedBindGroup = renderLayout.bindGroup;

      auto transform_buffer = m_pAssetManager->getTransformBuffer();
      auto object_buffer = m_pAssetManager->getObjectBuffer();

      for (auto &mesh : renderModel->data.renderMeshes[0]) {
        if (renderer->numPushedCommands() >= engine::MAX_COMMAND_QUEUE_SIZE) {
          Flush();
        }

        uint32_t material_indices[4] = {0};

        for (int i = 0; i < mesh.numMaterials; i++) {
          uint32_t uniform_index =
              m_pAssetManager->m_pMaterials.at(mesh.material_keys[i]).index;

          material_indices[i] = uniform_index;
        }

        transform_buffer[m_pMeshCount] = transform;

        object_buffer[m_pMeshCount] = {
            .object_id = (unsigned int)renderModel->Parent()->Index(),
            .transform_index = m_pMeshCount,
            .material_index = {material_indices[0], material_indices[1],
                               material_indices[2], material_indices[3]},
            .numMaterials = (uint32_t)mesh.numMaterials,
            .animationIndex = renderModel->data.animationIndex};

        renderer->Push(
            {.mesh = mesh.mesh,
             .material = selectedBindGroup,
             .pushConstant = push_constant,
             .objectConstant = {.transform = transform,
                                .material_index = material_indices[0],
                                .animation_offset =
                                    renderModel->data.animationIndex},
             .layout_index = renderLayout.pipelineLayoutIndex,
             .uniformIndex = m_pMeshCount});
        m_pMeshCount++;
      }
    }

    return push_index;
  }

  uint32_t Push(const std::vector<glm::mat4> &transforms,
                engine::parsers::RenderLayout layout,
                const std::string &material, unsigned int count = 1) {

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

        for (int j = 0; j < 1; j++) {
          uint32_t uniform_index = m_pAssetManager->m_pMaterials
                                       .at(std::hash<std::string>{}(material))
                                       .index;
          material_indices[j] = uniform_index;
        }

        transform_buffer[m_pMeshCount] = transforms[i];

        object_buffer[m_pMeshCount] = {
            .object_id = (unsigned int)i,
            .transform_index = m_pMeshCount,
            .material_index = {material_indices[0], material_indices[1],
                               material_indices[2], material_indices[3]},
            .numMaterials = (uint32_t)1,
            .animationIndex = 0};

        m_pMeshCount++;
      }

      renderer->Push({.mesh = engine::Ref<Mesh>().makeEmpty(),
                      .material = selectedBindGroup,
                      .pushConstant = push_constant,
                      .objectConstant = {.transform = transforms[0],
                                         .material_index = material_indices[0]},
                      .layout_index = layout.pipelineLayoutIndex,
                      .uniformIndex = firstInstance,
                      .count = count});
    }

    return push_index;
  }
  uint32_t lastRenderModelId = -1;

  uint32_t Push(const RenderArgs &renderArgs) {

    uint32_t push_index = m_pMeshCount;

    if (m_pContext->Window().getSize().width <= 0)
      return push_index;

    auto renderer = m_pContext->Renderer();
    Ref<PushConstant> push_constant;
    auto &renderLayout = renderArgs.layout;
    Ref<BindGroup> selectedBindGroup = renderLayout.bindGroup;
    auto transform_buffer = m_pAssetManager->getTransformBuffer();
    auto object_buffer = m_pAssetManager->getObjectBuffer();
    int submeshCount =
        renderArgs.submeshCount >= 1
            ? renderArgs.submeshCount
            : renderArgs.renderModel->data.renderMeshes[0].size();
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
          uint32_t uniform_index =
              m_pAssetManager->m_pMaterials.at(mesh->material_keys[k]).index;
          material_indices[k] = uniform_index;
        }

        if (renderArgs.count > 1)
          transform_buffer[m_pMeshCount] = renderArgs.transforms[j];

        object_buffer[m_pMeshCount] = {
            .object_id =
                (unsigned int)renderArgs.renderModel->Parent()->Index() + j,
            .transform_index = m_pMeshCount,
            .material_index = {material_indices[0], material_indices[1],
                               material_indices[2], material_indices[3]},
            .numMaterials = (uint32_t)mesh->numMaterials,
            .animationIndex = renderArgs.renderModel->data.animationIndex};
        m_pMeshCount++;
      }
      renderer->Push(
          {.mesh = mesh->mesh,
           .material = selectedBindGroup,
           .pushConstant = push_constant,
           .objectConstant = {.transform = renderArgs.transform,
                              .material_index = material_indices[0],
                              .animation_offset =
                                  renderArgs.renderModel->data.animationIndex},
           .layout_index = renderLayout.pipelineLayoutIndex,
           .uniformIndex = firstInstance,
           .count = renderArgs.count});
    }

    return push_index;
  }

  uint32_t Push(std::shared_ptr<Node<RenderModel>> renderModel,
                const std::vector<glm::mat4> &transforms, unsigned int count,
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
            uint32_t uniform_index =
                m_pAssetManager->m_pMaterials.at(mesh.material_keys[j]).index;
            material_indices[j] = uniform_index;
          }

          transform_buffer[m_pMeshCount] = transforms[i];

          object_buffer[m_pMeshCount] = {
              .object_id = (unsigned int)renderModel->Parent()->Index() + i,
              .transform_index = m_pMeshCount,
              .material_index = {material_indices[0], material_indices[1],
                                 material_indices[2], material_indices[3]},
              .numMaterials = (uint32_t)mesh.numMaterials,
              .animationIndex = renderModel->data.animationIndex};

          m_pMeshCount++;
        }

        renderer->Push(
            {.mesh = mesh.mesh,
             .material = selectedBindGroup,
             .pushConstant = push_constant,
             .objectConstant = {.transform = transforms[0],
                                .material_index = material_indices[0],
                                .animation_offset =
                                    renderModel->data.animationIndex},
             .layout_index = layout.pipelineLayoutIndex,
             .uniformIndex = firstInstance,
             .count = count});
      }
    }
    return push_index;
  }

  void ComputeDispatch(engine::Ref<ComputeShader> computeShader,
                       Ref<BindGroup> bindGroup) {

    auto renderer = m_pContext->Renderer();
    renderer->ComputeDispatch(computeShader, bindGroup);
  }

  void ComputeDispatchAsync(engine::Ref<ComputeShader> computeShader,
                            Ref<BindGroup> bindGroup) {

    auto renderer = m_pContext->Renderer();
    renderer->ComputeDispatch(computeShader, bindGroup);
  }

  void EndScene() {
    if (m_pContext->Window().getSize().width > 0) {
      auto renderer = m_pContext->Renderer();
      glm::vec3 v;
      renderer->End(v);

      renderer->Submit();
      renderer->EndFrame();
    }
  }

  void Destroy() {}
};
} // namespace engine