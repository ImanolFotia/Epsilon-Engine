#pragma once

#include <span>

#include "asset_manager.hpp"
#include "core/common/common.hpp"
#include "nodes/node_manager.hpp"
#include <core/engine/context.hpp>

#include "parsers/renderpass.hpp"
#include "trees/octree.hpp"

#include "../audio/audio_manager.hpp"

#include "structs/frustum.hpp"
#include "transform.hpp"
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
  int instance_index = -1;
  bool visible;

  ~OctreeSceneItem() {}
};

struct ItemInfo {
  Box box;
  OctreeItemMask mask = OctreeItemMask::ANY;
};

struct EntityData {
  alignas(32) glm::mat4 modelMatrix{};
  glm::ivec4 texture_indices;
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
  int submeshCount = -1; //-1 to render all submeshes, >1 to render selected submeshes
  std::vector<int> subMeshes{};
  std::string material{};
  unsigned int count = 1;
  std::span<glm::mat4> transforms;
};

class Scene {
public:
  virtual ~Scene() = default;
  using OctreeRenderType = OctreeSceneItem;
  using OctreeRenderItem = typename std::list<OctreeItem<OctreeRenderType>>::iterator;
  using OctreeNodeType = std::shared_ptr<NodeBase>;


  using CullResultType = CullResult<typename OctreeContainer<OctreeRenderType>::OctreeData::iterator>;

protected:
  std::shared_ptr<AssetManager> m_pAssetManager;
private:
  SceneManager m_pSceneManager;

  Ref<RenderPass> m_pCurrentRenderPass;

  std::shared_ptr<OctreeContainer<OctreeRenderType>> m_pRenderOctree = nullptr;
  std::shared_ptr<OctreeContainer<OctreeNodeType>> m_pNodeOctree = nullptr;

  Frustum m_pFrustum{};
  Frustum m_pShadowFrustum{};

  std::shared_ptr<Context> m_pContext = nullptr;

  uint32_t m_pMeshCount = 0;

  std::size_t getHash(const std::string &s) { return std::hash<std::string>{}(s); }

public:
  struct SceneEntity {
    glm::mat4 transform;
  };

  Scene() = default;

  std::shared_ptr<OctreeContainer<OctreeRenderType>> getRenderOctree();

  Scene(std::shared_ptr<Context> context);

  std::shared_ptr<Context> getContext();

  void setAssetManager(std::shared_ptr<engine::AssetManager> assetManager);

  void Init();

  void setCurrentRenderPass(engine::Ref<engine::RenderPass> renderPassRef);

  std::shared_ptr<AssetManager> &getAssetManager();

  void SetViewport(const Viewport &viewport);

  void SetScissor(const Scissor &scissor);

  void UpdateShadowFrustum(glm::mat4 proj, glm::mat4 view);

  void UpdateFrustum(glm::mat4 proj, glm::mat4 view);

  Frustum &getFrustum();

  CullResultType &Cull(float cutout_distance = 0, int pass = 0, OctreeItemMask mask = OctreeItemMask::ANY);

  CullResultType &Cull(Box &box, int pass = 0, OctreeItemMask mask = OctreeItemMask::ANY);

  CullResultType &Cull(BoundingSphere &sphere, int pass = 0, OctreeItemMask mask = OctreeItemMask::ANY);

  CullResultType &CullShadow(float cutout_distance = 0, int pass = 1, OctreeItemMask mask = OctreeItemMask::SHADOW);

  CullResultType &CullShadow(Box box, int pass = 1, OctreeItemMask mask = OctreeItemMask::SHADOW);

  void RelocateObject(Box boundingBox, int index);

  template <typename T> void removeFromScene(uint32_t index) {
    // std::cout << "removed index: " << index << std::endl;
    std::shared_ptr<Node<T>> node = std::static_pointer_cast<Node<T>>(m_pSceneManager.get(index));

    using render_node_type = engine::Node<std::list<OctreeItem<OctreeRenderType>>::iterator>;
    auto &children = getChildren(node);
    auto &octree_render_node = children[typeid(typename std::list<OctreeItem<OctreeRenderType>>::iterator).hash_code()];
    for (auto render_node : octree_render_node) {

      if (render_node != nullptr) {
        auto r_node = std::static_pointer_cast<render_node_type>(render_node);
        m_pRenderOctree->erase(r_node->data);
      }
    }

    m_pSceneManager.erase<T>(node);
  }

  template <typename P, typename T> void removeFromNode(std::shared_ptr<Node<P>> parent, std::shared_ptr<Node<P>> node) { m_pSceneManager.erase(node); }

  template <typename T> void remove_selected(std::vector<T> &v, std::vector<std::size_t> selection) {

    int offset = v.size();
    for (size_t i = 0; i < selection.size(); i++) {
      offset -= 1;
      if (std::find(selection.begin(), selection.end(), offset) != selection.end()) {
        i--;
        continue;
      }
      v[selection[i]] = v[offset];
    }

    v.resize(v.size() - selection.size());
  }

  template <typename T> void removeFromOctree(std::list<OctreeItem<OctreeRenderType>>::iterator item) {
    auto node = std::static_pointer_cast<Node<T>>(m_pSceneManager.get((item->data.renderModel->Parent()->Index())));
    using render_node_type = engine::Node<std::list<OctreeItem<OctreeRenderType>>::iterator>;
    auto &children = getChildren(node);
    auto &octree_render_node = children[typeid(typename std::list<OctreeItem<OctreeRenderType>>::iterator)];
    int i = 0;

    for (auto render_node : octree_render_node) {

      if (render_node != nullptr) {
        auto r_node = std::static_pointer_cast<render_node_type>(render_node);
        if (item->data.instance_index == r_node->data->data.instance_index) {
          m_pSceneManager.erase(r_node);
          octree_render_node.erase(octree_render_node.begin() + i);
          break;
        }
      }
      i++;
    }
    /*
        for (auto index : deleting_indices) {

          octree_render_node.erase(octree_render_node.begin() + index);
        }*/

    m_pRenderOctree->erase(item);
  }

  template <typename T> bool isOfType(std::shared_ptr<NodeBase> node) { return m_pSceneManager.isOfType<T>(node); }

  template <typename T> auto insertIntoScene(Box boundingBox, T object) {
    auto scene_node = m_pSceneManager.insert(m_pSceneManager.root, object);

    /*auto octree_node = m_pNodeOctree->insert(boundingBox, scene_node);

    m_pSceneManager.insert(scene_node, octree_node);
*/
    return scene_node;
  }

  template <typename T, class... Args> auto emplaceIntoScene(Box boundingBox, Args &&...args) {
    auto scene_node = m_pSceneManager.emplace<T>(m_pSceneManager.root, std::forward<Args>(args)...);
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
    auto octree_node = m_pNodeOctree->insert(boundingBox, scene_node);

    m_pSceneManager.insert(scene_node, octree_node);*/

    return scene_node;
  }

  template <typename P, typename T> auto insertIntoNode(std::shared_ptr<Node<P>> parent, T object) {
    auto node = m_pSceneManager.insert(parent, object);

    return node;
  }

  template <typename P, typename T>
  auto insertIntoNode(Box boundingBox, std::shared_ptr<Node<P>> parent, const T &object, OctreeItemMask mask = OctreeItemMask::ANY) {
    auto node = m_pSceneManager.insert(parent, object);

    if (typeid(T) == typeid(RenderModel)) {
      OctreeSceneItem item;
      item.renderModel = node;
      item.index = node->Index();
      std::list<OctreeItem<OctreeRenderType>>::iterator octree_node = m_pRenderOctree->insert(boundingBox, item);
      octree_node->mask = mask;

      insertIntoNode(parent, octree_node);
    }

    return node;
  }

  Scene::OctreeRenderItem insertIntoOctree(Box boundingBox, OctreeSceneItem item, OctreeItemMask mask = OctreeItemMask::ANY);

  template <typename T, typename P, class... Args> auto emplaceIntoNode(std::shared_ptr<Node<P>> parent, Args &&...args) {
    auto node = m_pSceneManager.emplace<T>(parent, std::forward<Args>(args)...);
    return node;
  }

  template <typename T, typename P, class... Args> auto emplaceIntoNode(ItemInfo info, std::shared_ptr<Node<P>> parent, Args &&...args) {

    auto node = m_pSceneManager.emplace<T>(parent, std::forward<Args>(args)...);
    if (typeid(T) == typeid(RenderModel)) {

      OctreeSceneItem item;
      item.renderModel = node;
      item.index = node->Index();
      std::list<OctreeItem<OctreeRenderType>>::iterator octree_node = m_pRenderOctree->insert(info.box, item);
      octree_node->mask = info.mask;
      insertIntoNode(parent, octree_node);
    }

    return node;
  }

  template <typename T, typename P> auto emplaceIntoNode(std::shared_ptr<Node<P>> parent) {
    auto node = m_pSceneManager.emplace<T>(parent);
    return node;
  }

  template <typename T, typename P> auto emplaceIntoNode(Box boundingBox, std::shared_ptr<Node<P>> parent) {
    auto node = m_pSceneManager.emplace<T>(parent);
    if (typeid(T) == typeid(RenderModel)) {
      OctreeSceneItem item;
      item.renderModel = node;
      item.index = node->Index();
      std::list<OctreeItem<OctreeRenderType>>::iterator octree_node = m_pRenderOctree->insert(boundingBox, item);
      insertIntoNode(parent, octree_node);
    }

    return node;
  }

  template <typename T> auto &getNodes() { return m_pSceneManager.get<T>(); }

  const auto &getNodes();

  std::shared_ptr<NodeBase> getNode(uint32_t index);

  template <typename T, typename P> std::shared_ptr<Node<T>> getChild(std::shared_ptr<Node<P>> parent) {
    return m_pSceneManager.to<T>(m_pSceneManager.getChild<T>(parent));
  }

  template <typename T> std::shared_ptr<Node<T>> getChild(std::shared_ptr<NodeBase> parent) {
    return m_pSceneManager.to<T>(m_pSceneManager.getChild<T>(parent));
  }

  template <typename T> std::shared_ptr<Node<T>> getChild() { return m_pSceneManager.to<T>(m_pSceneManager.getChild<T>(m_pSceneManager.root)); }

  SceneManager::ChildNodes &getChildren(std::shared_ptr<NodeBase> parent);

  void BeginScene();

  void Flush(engine::DrawType drawType = engine::DrawType::INDEXED, bool clean_attachments = true);

  uint32_t Push(std::shared_ptr<Node<RenderModel>> renderModel, const glm::mat4 &transform, engine::parsers::RenderLayout layout);

  uint32_t Push(const std::vector<glm::mat4> &transforms, engine::parsers::RenderLayout layout, const std::string &material, unsigned int count = 1);
  uint32_t lastRenderModelId = -1;

  uint32_t Push(const RenderArgs &renderArgs);

  uint32_t Push(std::shared_ptr<Node<RenderModel>> renderModel, const std::vector<glm::mat4> &transforms, unsigned int count,
                engine::parsers::RenderLayout layout);

  void ComputeDispatch(engine::Ref<ComputeShader> computeShader, Ref<BindGroup> bindGroup);

  void ComputeDispatchAsync(engine::Ref<ComputeShader> computeShader, Ref<BindGroup> bindGroup);

  void EndScene();
};
} // namespace engine