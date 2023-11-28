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

#include "node.hpp"

namespace engine
{

    struct SceneManager {
        using TypeContainer = std::vector<std::shared_ptr<NodeBase>>;
        using TypeIterator = std::shared_ptr<NodeBase>;
        using NodeTypes =
            std::unordered_map<std::type_index, std::vector<TypeIterator>>;
        using NodeIndexType = std::unordered_map<int, std::shared_ptr<NodeBase>>;
        using RootType = std::shared_ptr<Node<Root>>;
        using ChildNodes =
            std::unordered_map<std::type_index,
            std::vector<std::shared_ptr<NodeBase>>>;

        std::mutex m_pMutex;

        SceneManager() {
            root = std::make_shared<Node<Root>>();
            empty.resize(0);
        }

        ~SceneManager() {
            if (node_types.size() > 0 || node_index.size() > 0
                /*nodes.size() > 0*/) {
                std::cerr << "Warning: Not all nodes were erased before deleting "
                    "the scene manager\n";
                //erase(root);
            }

            children_node_index.erase(root->index);
        }

        template <typename T, typename P, class... Args>
        std::shared_ptr<Node<T>> emplace(std::shared_ptr<Node<P>> parent,
            Args&&... args) {
            std::lock_guard<std::mutex> guard(m_pMutex);
            if (parent == nullptr) return nullptr;

            auto iType = std::type_index(typeid(T));

            size_t index = -1;

            if (free_indices.size() > 0) {
                index = free_indices.front();
                free_indices.pop();
            }
            else {
                index = node_count;
                node_count++;
            }

            //nodes.push_back(std::make_shared<Node<T>>(std::forward<Args>(args)...));

            node_types[iType].push_back(std::make_shared<Node<T>>(std::forward<Args>(args)...));
            node_types[iType].back()->container_index = node_types[iType].size() - 1;

            node_index[index] = node_types[iType].back();//std::prev(node_types[iType].end());

            parent->destroy_children.push_back([=, this]() {
                try {
                    using child_type = T;
                    if (node_index.contains(index)) {
                        erase(std::static_pointer_cast<Node<T>>(node_index.at(index)));

                        if (children_node_index.contains(index))
                            children_node_index.erase(index);
                    }

                }
                catch (std::exception& e) {
                    std::cout << "from " << __PRETTY_FUNCTION__
                        << " ::: " << e.what() << std::endl;
                }
                });

            auto new_node_ptr = std::static_pointer_cast<Node<T>>(node_types[iType].back());

            children_node_index[parent->index][iType].push_back(new_node_ptr);

            new_node_ptr->parent = std::move(parent);
            new_node_ptr->index = index;

            return std::static_pointer_cast<Node<T>>(node_types[iType].back());
        }


        template <typename T, typename P, class... Args>
        std::shared_ptr<Node<T>> insert(std::shared_ptr<Node<P>> parent, T obj) {

            std::lock_guard<std::mutex> guard(m_pMutex);
            if (parent == nullptr) return nullptr;

            auto iType = std::type_index(typeid(T));

            size_t index = -1;

            if (free_indices.size() > 0) {
                index = free_indices.front();
                free_indices.pop();
            }
            else {
                index = node_count;
                node_count++;
            }

            //nodes.push_back(std::make_shared<Node<T>>(std::move(obj)));

            node_types[iType].push_back(std::make_shared<Node<T>>(std::move(obj)));
            node_types[iType].back()->container_index = node_types[iType].size() - 1;

            node_index[index] = node_types[iType].back();

            parent->destroy_children.push_back([=, this]() {
                try {
                    using child_type = T;
                    if (node_index.contains(index)) {
                        erase(std::static_pointer_cast<Node<child_type>>(
                            node_index.at(index)));

                        if (children_node_index.contains(index))
                            children_node_index.erase(index);
                    }

                }
                catch (std::exception& e) {
                    std::cout << "from " << __PRETTY_FUNCTION__
                        << " ::: " << e.what() << std::endl;
                }
                });

            auto new_node_ptr = std::static_pointer_cast<Node<T>>(node_types[iType].back());

            children_node_index[parent->index][iType].push_back(new_node_ptr);

            new_node_ptr->parent = std::move(parent);
            new_node_ptr->index = index;

            return std::static_pointer_cast<Node<T>>(node_types[iType].back());
        }

        //void erase(int i) {}

        template <typename T>
        void erase(std::shared_ptr<Node<T>> node) {
            auto iType = std::type_index(typeid(T));
            if (!node_types.contains(iType) || !node_index.contains(node->index) ||
                node == nullptr) {
                return;
            }

            std::vector<TypeIterator>& node_container = node_types.at(iType);

            //nodes.erase(*node_index.at(node->index));
            size_t container_index = node_index.at(node->index)->container_index;
            node_container.erase(node_container.begin() + node_index.at(node->index)->container_index);
            for (int i = container_index; i < node_container.size(); i++) {
                node_container.at(i)->container_index--;
            }
            free_indices.push(node->index);

            node_index.erase(node->index);

            if (node_types.at(iType).size() == 0) {
                node_types.erase(iType);
            }

            for (auto& dst_func : node->destroy_children) {
                dst_func();
            }

            children_node_index.erase(node->index);

            node = nullptr;
        }

        template <typename T>
        [[nodiscard]] std::vector<TypeIterator>& get() {
            std::lock_guard<std::mutex> guard(m_pMutex);
            auto iType = std::type_index(typeid(T));
            if (node_types.contains(iType)) {
                return node_types.at(std::type_index(typeid(T)));
            }
            return empty;
        }

        template <typename T>
        std::vector<std::shared_ptr<NodeBase>>& get(
            std::shared_ptr<NodeBase> node) {
            auto iType = std::type_index(typeid(T));

            return children_node_index.at(node->index).at(iType);
        }

        ChildNodes& getChildren(std::shared_ptr<NodeBase> node) {
            return children_node_index[node->index];
        }

        template <typename T>
        bool isOfType(std::shared_ptr<NodeBase> node) {
            std::lock_guard<std::mutex> guard(m_pMutex);
            if (node == nullptr) return false;
            auto iType = std::type_index(typeid(T));
            if (node_types.contains(iType)) {
                for (auto& n : node_types.at(std::type_index(typeid(T)))) {
                    if (n->Index() == node->Index()) return true;
                }
            }
            return false;
        }

        template <typename T>
        std::shared_ptr<NodeBase> getChild(std::shared_ptr<NodeBase> node) {
            std::lock_guard<std::mutex> guard(m_pMutex);
            auto iType = std::type_index(typeid(T));
            if (node == nullptr) return nullptr;
            if(children_node_index.contains(node->index))
                if(children_node_index.at(node->index).contains(iType))
                    return children_node_index.at(node->index).at(iType).back();

            return nullptr;
        }

        std::shared_ptr<NodeBase> get(uint32_t index) {
            std::lock_guard<std::mutex> guard(m_pMutex);
            //if(node_index.contains(index))
                return (node_index.at(index));  // children_node_index.at(node->index).at(std::type_index(typeid(T)));
            return nullptr;
        }

        template <typename T>
        std::shared_ptr<Node<T>> to(std::shared_ptr<NodeBase> node) {
            return std::static_pointer_cast<Node<T>>(node);
        }

        //TypeContainer nodes;
        NodeTypes node_types;
        NodeIndexType node_index;

        std::unordered_map<size_t /*parent*/, ChildNodes> children_node_index;

        std::queue<size_t> free_indices;

        size_t node_count = 1;
        RootType root;

        std::vector<TypeIterator> empty;
    };
}
