#pragma once

#include <chrono>
#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <queue>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>

#include <core/framework/log.hpp>

#include "node.hpp"

namespace engine {

struct SceneManager {
  using TypeContainer = std::vector<std::shared_ptr<NodeBase>>;
  using TypeIterator = std::shared_ptr<NodeBase>;
  using NodeTypes = std::unordered_map<std::size_t, std::vector<std::shared_ptr<NodeBase>>>;
  using NodeIndexType = std::unordered_map<uint64_t, NodePosition>;
  using RootType = std::shared_ptr<Node<Root>>;
  using ChildNodes = std::unordered_map<std::size_t, std::vector<std::shared_ptr<NodeBase>>>;
  RootType root;
  SceneManager() {
    root = std::make_shared<Node<Root>>();

    auto type = GetType<Root>().hash_code();
    m_Nodes[type].push_back(std::make_shared<Node<Root>>());
    root = std::static_pointer_cast<Node<Root>>(m_Nodes[type].back());
    root->parent = nullptr;
    root->index = 0;
    m_Positions[0].type_id = type;
    m_Positions[0].container_position = 0;
  }

  ~SceneManager() { root->clear(); }

  NodeTypes m_Nodes;
  NodeIndexType m_Positions;
  uint64_t node_count = 1;

  template <typename T> std::type_index GetType() { return std::type_index(typeid(T)); }

  template <typename T, typename P, class... Args> std::shared_ptr<Node<T>> emplace(std::shared_ptr<Node<P>> parent, Args &&...args) {
    auto type = GetType<T>().hash_code();
    m_Nodes[type].push_back(std::make_shared<Node<T>>(std::forward<Args>(args)...));
    auto ptr = m_Nodes[type].back();
    ptr->parent = parent;
    ptr->parent->children[type].push_back(ptr);
    m_Positions[node_count].type_id = type;
    m_Positions[node_count].container_position = m_Nodes[type].size() - 1;
    ptr->index = node_count;

    node_count++;

    return std::static_pointer_cast<Node<T>>(ptr);
  }

  template <typename T, typename P, class... Args> std::shared_ptr<Node<T>> insert(std::shared_ptr<Node<P>> parent, T obj) {
    auto type = GetType<T>().hash_code();
    m_Nodes[type].push_back(std::make_shared<Node<T>>(obj));
    auto ptr = m_Nodes[type].back();
    ptr->parent = parent;
    ptr->parent->children[type].push_back(ptr);
    m_Positions[node_count] = {type, m_Nodes[type].size() - 1};
    ptr->index = node_count;
    node_count++;
    return std::static_pointer_cast<Node<T>>(ptr);
  }

  template <typename T> void erase(std::shared_ptr<Node<T>> node) {
    if(!m_Positions.contains(node->index)) return;
    auto position = m_Positions[node->index];
    auto &container = m_Nodes[position.type_id];

    // erase all data and references from this node's children
    // both from the internal array and the node container
    for (auto &[index, ptrs] : node->children) {
      for (int i = 0; i < ptrs.size(); i++) {
        auto pos = m_Positions[ptrs[i]->index];

        auto &child_container = m_Nodes[pos.type_id];

        auto child = child_container[pos.container_position];
        child->parent.reset();
        child->clear();

        auto last = child_container.back();
        auto &last_position = m_Positions[last->index];

        child_container[pos.container_position] = last;
        last_position.container_position = pos.container_position;

        child_container.resize(child_container.size() - 1);
        m_Positions.erase(ptrs[i]->index);
      }
      ptrs.clear();
    }

    node->children.clear();

    // erase all data and references from this node's parent
    for (auto &[index, ptrs] : node->parent->children) {
      bool found = false;
      for (int i = 0; i < ptrs.size(); i++) {
        if (ptrs[i].get() == node.get()) {
          found = true;
          ptrs[i]->clear();
          ptrs.erase(ptrs.begin() + i);
          break;
        }
      }
      if (found)
        break;
    }

    // remove this node from de node container
    auto last = container.back();
    auto &last_position = m_Positions[last->index];
    container[position.container_position].reset();

    container[position.container_position] = last;
    last_position.container_position = position.container_position;

    container.resize(container.size() - 1);
    m_Positions.erase(node->index);

    if (container.size() <= 0)
      m_Nodes[position.type_id].clear();

    node.reset();
  }

  template <typename T> [[nodiscard]] std::vector<TypeIterator> &get() {
    if (!m_Nodes.contains(GetType<T>().hash_code()))
      return empty;
    return m_Nodes[GetType<T>().hash_code()];
  }

  template <typename T> std::vector<std::shared_ptr<NodeBase>> &get(std::shared_ptr<NodeBase> node) {
    if (!m_Nodes.contains(GetType<T>().hash_code()))
      return empty;
    return m_Nodes[GetType<T>().hash_code()];
  }

  ChildNodes null_return_child_node{};
  ChildNodes &getChildren(std::shared_ptr<NodeBase> node) {
    auto position = m_Positions[node->index];
    if (!m_Nodes.contains(position.type_id))
      return null_return_child_node;
    return m_Nodes[position.type_id][position.container_position]->children;
  }

  template <typename T> bool isOfType(std::shared_ptr<NodeBase> node) {
    auto position = m_Positions[node->index];

    return GetType<T>().hash_code() == position.type_id;
  }

  template <typename T> std::shared_ptr<NodeBase> getChild(std::shared_ptr<NodeBase> node) {
    auto position = m_Positions[node->index];
    if (!m_Nodes.contains(position.type_id))
      return nullptr;
    return m_Nodes[position.type_id][position.container_position]->children[GetType<T>().hash_code()].front();
  }

  std::shared_ptr<NodeBase> get(uint64_t index) {
    auto position = m_Positions[index];
    if (m_Nodes.contains(position.type_id))
      if (m_Nodes[position.type_id].size() > 0 && m_Nodes[position.type_id].size() > position.container_position)
        return m_Nodes[position.type_id][position.container_position];

    return nullptr;
  }

  template <typename T> std::shared_ptr<Node<T>> to(std::shared_ptr<NodeBase> node) { return std::static_pointer_cast<Node<T>>(node); }

  std::vector<TypeIterator> empty;
};

} // namespace engine
