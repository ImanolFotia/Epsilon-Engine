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
#include <vector>

#include "node.hpp"

namespace engine
{

	struct SceneManager
	{
		using TypeContainer = std::list<std::shared_ptr<NodeBase>>;
		using TypeIterator = typename TypeContainer::iterator;
		using NodeTypes = std::unordered_map<std::type_index, std::list<TypeIterator>>;
		using NodeIndexType = std::unordered_map<int, typename std::list<TypeIterator>::iterator>;
		using RootType = std::shared_ptr<Node<Root>>;

		SceneManager() {
			root = std::make_shared<Node<Root>>();
			empty.resize(0);
		}

		~SceneManager()
		{
			if (node_types.size() > 0 || node_index.size() > 0 ||
				nodes.size() > 0)
			{
				std::cerr << "Warning: Not all nodes were erased before deleting "
					"the scene manager\n";
				erase(root);
			}

			children_node_index.erase(root->index);
		}

		template <typename T, typename P, class... Args>
		std::shared_ptr<Node<T>> emplace(std::shared_ptr<Node<P>> parent, Args &&...args)
		{
			if (parent == nullptr)
				return nullptr;

			auto iType = std::type_index(typeid(T));

			size_t index = -1;

			if (free_indices.size() > 0)
			{
				index = free_indices.front();
				free_indices.pop();
			}
			else
			{
				index = node_count;
				node_count++;
			}

			nodes.push_back(std::make_shared<Node<T>>(std::forward<Args>(args)...));

			node_types[iType].push_back(std::prev(nodes.end()));

			node_index[index] = std::prev(node_types[iType].end());

			parent->destroy_children.push_back([this, index]()
				{
					try {
						using child_type = T;
						if (node_index.contains(index)) {
							erase(std::static_pointer_cast<Node<child_type>>(
								**node_index.at(index)));

							children_node_index.erase(index);
						}

						free_indices.push(index);
					}
					catch (std::exception& e) {
						std::cout << "from " << __PRETTY_FUNCTION__
							<< " ::: " << e.what() << std::endl;
					} });

			auto new_node_ptr = std::static_pointer_cast<Node<T>>(nodes.back());

			children_node_index[parent->index][iType].push_back(new_node_ptr);

			new_node_ptr->parent = std::move(parent);
			new_node_ptr->index = index;

			return std::static_pointer_cast<Node<T>>(nodes.back());
		}

		template <typename T, typename P, class... Args>
		std::shared_ptr<Node<T>> insert(std::shared_ptr<Node<P>> parent, T obj)
		{
			if (parent == nullptr)
				return nullptr;

			auto iType = std::type_index(typeid(T));

			size_t index = -1;

			if (free_indices.size() > 0)
			{
				index = free_indices.front();
				free_indices.pop();
			}
			else
			{
				index = node_count;
				node_count++;
			}

			nodes.push_back(std::make_shared<Node<T>>(std::move(obj)));

			node_types[iType].push_back(std::prev(nodes.end()));

			node_index[index] = std::prev(node_types[iType].end());

			parent->destroy_children.push_back([this, index]()
				{
					try {
						using child_type = T;
						if (node_index.contains(index)) {
							erase(std::static_pointer_cast<Node<child_type>>(
								**node_index.at(index)));

							children_node_index.erase(index);
						}

						free_indices.push(index);
					}
					catch (std::exception& e) {
						std::cout << "from " << __PRETTY_FUNCTION__
							<< " ::: " << e.what() << std::endl;
					} });

			auto new_node_ptr = std::static_pointer_cast<Node<T>>(nodes.back());

			children_node_index[parent->index][iType].push_back(new_node_ptr);

			new_node_ptr->parent = std::move(parent);
			new_node_ptr->index = index;

			return std::static_pointer_cast<Node<T>>(nodes.back());
		}

		template <typename T>
		void erase(std::shared_ptr<Node<T>> node)
		{
			auto iType = std::type_index(typeid(T));
			if (!node_types.contains(iType) || !node_index.contains(node->index) ||
				node == nullptr)
			{
				return;
			}

			std::list<TypeIterator>& node_container = node_types.at(iType);

			nodes.erase(*node_index.at(node->index));
			node_container.erase(node_index.at(node->index));

			free_indices.push(node->index);

			node_index.erase(node->index);

			if (node_types.at(iType).size() == 0)
			{
				node_types.erase(iType);
			}

			for (auto& dst_func : node->destroy_children)
			{
				dst_func();
			}

			children_node_index.erase(node->index);

			node = nullptr;
		}

		template <typename T>
		[[nodiscard]] std::list<TypeIterator>& get()
		{
			auto iType = std::type_index(typeid(T));
			if (node_types.contains(iType))
			{
				return node_types.at(std::type_index(typeid(T)));
			}
			return empty;
		}

		template <typename T>
		std::list<std::shared_ptr<NodeBase>>& get(std::shared_ptr<NodeBase> node)
		{
			auto iType = std::type_index(typeid(T));

			return children_node_index.at(node->index).at(std::type_index(typeid(T)));
		}

		template <typename T>
		std::shared_ptr<NodeBase> getChild(std::shared_ptr<NodeBase> node)
		{
			auto iType = std::type_index(typeid(T));

			return children_node_index.at(node->index).at(std::type_index(typeid(T))).front();
		}

		std::shared_ptr<NodeBase> get(uint32_t index)
		{

			return (**node_index.at(index));//children_node_index.at(node->index).at(std::type_index(typeid(T)));
		}

		template <typename T>
		std::shared_ptr<Node<T>> to(std::shared_ptr<NodeBase> node)
		{
			return std::static_pointer_cast<Node<T>>(node);
		}

		TypeContainer nodes;
		NodeTypes node_types;
		NodeIndexType node_index;

		std::unordered_map<std::type_index, std::list<std::shared_ptr<NodeBase>>> node_child_index;

		std::unordered_map<size_t /*parent*/,
			std::unordered_map<std::type_index,
			std::list<std::shared_ptr<NodeBase>>>>
			children_node_index;

		std::queue<size_t> free_indices;

		size_t node_count = 1;
		RootType root;

		std::list<TypeIterator> empty;
	};
}
