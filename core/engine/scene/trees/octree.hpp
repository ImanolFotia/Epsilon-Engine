#pragma once

#include <array>
#include <glm/glm.hpp>
#include <iostream>
#include <list>
#include <memory>
#include <random>
#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include <core/framework/containers/static_array.hpp>

#include "../structs/frustum.hpp"
#include "../structs/sphere.hpp"

namespace engine
{
	/**
	 * @brief Wrapper for the items that are stored in the container
	 * Along with their position in the container's main object list
	 *
	 * @tparam T the data type of objects the octree holds
	 */
	template <typename T>
	struct OctreeItem
	{
		T data{};
		OctreeItemLocation<typename std::list<OctreeItem<T>>::iterator> data_position;
	};

	template <typename T>
	class Octree
	{
		using Octree_ptr = std::shared_ptr<Octree<T>>;

		Box m_Box{};

		bool m_IsBuilt = false;
		bool m_IsLeaf = true;

		bool m_IsEmpty = true;

		int m_Depth = 0;

		framework::StaticArray<Octree_ptr, 8> m_Children{};
		framework::StaticArray<Box, 8> m_bChildren{};

		std::list<std::pair<Box, T>> m_Data{};

		enum COLLISION
		{
			OVERLAPS = 0,
			CONTAINS,
			IS_LEAF
		};

	public:
		/**
		 * @brief Construct a new Octree object
		 *
		 * @param box Bounding box of the octree, this is the size of the root node
		 * @param depth Depth of the current octree node within the root node
		 */
		Octree() = delete;
		Octree(Box box, int depth = 0) : m_Box(box), m_Depth(depth)
		{
			if (m_Depth < MAX_DEPTH)
				pBuild();
			else
				m_IsLeaf = true;
			m_IsBuilt = true;
		}

		/**
		 * @brief Traverses the entires octree and debug prints all it's content to stdout
		 *
		 */
		void traverse()
		{
			for (auto &d : m_Data)
				std::cout << "found data: " //<< *d.second.data
						  << " with address: " << &d.second
						  << "\nat depth: " << m_Depth << std::endl;

			if (m_IsBuilt && !m_IsLeaf)
			{
				for (size_t i = 0; i < 8; i++)
				{
					if (m_Children[i] != nullptr)
						if (!m_Children[i]->m_IsEmpty)
							m_Children[i]->traverse();
				}
			}
		}

		auto search(Box minmax) -> std::list<T>
		{
			std::list<T> items;
			search(minmax, items);
			return items;
		}

		void search(const Box &box, std::list<T> &items)
		{
			for (const auto &p : m_Data)
			{
				if (box.overlaps(p.first))
				{
					items.push_back(p.second);
				}
			}

			for (size_t i = 0; i < 8; i++)
			{
				check_child(i, box, items);
			}
			/*
			for (auto& worker : m_Workers)
			{
				if (worker.joinable())
				{
					std::cout << "Thread " << worker.get_id() << " has finished." << std::endl;
					worker.join();
				}
			}*/
		}

		void check_child(int index, const Box &box, std::list<T> &items)
		{
			if (m_Children[index])
			{
				if (box.contains(m_bChildren[index]))
					m_Children[index]->items(items);
				else if (box.overlaps(m_bChildren[index]))
					m_Children[index]->search(box, items);
			}
		}

		auto search(Frustum &frustum) -> std::list<T>
		{
			std::list<T> items{};
			search(frustum, items);
			return items;
		}

		void search(Frustum &frustum, std::list<T> &items)
		{
			for (auto &p : m_Data)
			{
				if (frustum.overlaps(p.first))
					items.push_back(p.second);
			}

			for (size_t i = 0; i < 8; i++)
			{
				if (!m_Children[i]->m_IsEmpty)
				{
					if (frustum.contains(m_bChildren[i]))
						m_Children[i]->items(items);
					else if (frustum.overlaps(m_bChildren[i]))
						m_Children[i]->search(frustum, items);
				}
			}
		}

		auto search(BoundingSphere &sphere) -> std::list<T>
		{
			std::list<T> items{};
			search(sphere, items);
			return items;
		}

		void search(BoundingSphere &sphere, std::list<T> &items)
		{
			for (auto &p : m_Data)
			{
				if (sphere.overlaps(p.first))
					items.push_back(p.second);
			}

			for (size_t i = 0; i < 8; i++)
			{
				if (!m_Children[i]->m_IsEmpty)
				{
					if (sphere.contains(m_bChildren[i]))
						m_Children[i]->items(items);
					else if (sphere.overlaps(m_bChildren[i]))
						m_Children[i]->search(sphere, items);
				}
			}
		}

		void items(std::list<T> &items) const
		{
			for (auto &i : m_Data)
				items.push_back(i.second);

			for (auto &c : m_Children)
			{
				if (!c->m_IsEmpty)
					c->items(items);
			}
		}

		OctreeItemLocation<T> insert(const Box &box, T data)
		{
			if (m_IsBuilt)
			{
				for (int i = 0; i < 8; i++)
				{
					if (m_bChildren[i].contains(box))
					{
						if (m_Depth + 1 < MAX_DEPTH)
						{
							m_IsLeaf = false;
							m_IsEmpty = false;
							return m_Children[i]->insert(box, data);
						}
					}
				}
				m_IsEmpty = false;
				m_Data.push_back({box, data});
			}

			return {&m_Data, std::prev(m_Data.end())};
		}

	private:
		void pBuild()
		{
			glm::vec3 new_size = (m_Box.size / 2.0f);

			for (size_t i = 0; i < 8; i++)
			{
				glm::vec3 new_position;
				new_position.x = m_Box.position.x + (new_size.x) * CUADRANT[i].x;
				new_position.y = m_Box.position.y + (new_size.y) * CUADRANT[i].y;
				new_position.z = m_Box.position.z + (new_size.z) * CUADRANT[i].z;

				Box new_mmp(new_position, new_size);
				m_Children[i] = std::make_shared<Octree<T>>(new_mmp, m_Depth + 1);
				m_bChildren[i] = new_mmp;

				// std::cout << "Created new box on depth " << m_Depth << ": \n" <<
				// new_mmp;
			}

			m_IsBuilt = true;
		}
	};

	template <typename T>
	class OctreeContainer
	{

		using OctreeData = std::list<OctreeItem<T>>;

		OctreeData m_Data;
		Octree<typename OctreeData::iterator> m_Root;

	public:
		OctreeContainer(Box minmax, int depth = 0) : m_Root(minmax, depth)
		{
		}

		auto insert(Box pos, T data)
		{
			OctreeItem<T> item{};
			item.data = data;

			m_Data.push_back(item);
			// std::cout << "Inserting data: " << data
			//           << " with address: " << &m_Data.back() << std::endl;
			auto last = std::prev(m_Data.end());
			m_Data.back().data_position = m_Root.insert(pos, last);

			return last;
		}

		std::list<typename OctreeData::iterator> search(Frustum &frustum)
		{
			return m_Root.search(frustum);
		}

		std::list<typename OctreeData::iterator> search(const Box &box)
		{
			return m_Root.search(box);
		}

		std::list<typename OctreeData::iterator> search(BoundingSphere &sphere)
		{
			return m_Root.search(sphere);
		}

		void erase(typename OctreeData::iterator &item)
		{
			item->data_position.container->erase(item->data_position.iterator);

			m_Data.erase(item);
		}

		std::list<OctreeItem<T>>::iterator relocate(typename OctreeData::iterator &item, const Box &box)
		{
			item->data_position.container->erase(item->data_position.iterator);

			item->data_position = m_Root.insert(box, item);

			return std::prev(m_Data.end());
		}

		OctreeData &items()
		{
			return m_Data;
		}

		void clean() {}

		const std::list<T> &data() { return m_Data; }

		void traverse() { m_Root.traverse(); }

		/*useful functions to make ranged for loops*/

		// Convenience functions for ranged for loop
		typename OctreeData::iterator begin()
		{
			return m_Data.begin();
		}

		typename OctreeData::iterator end()
		{
			return m_Data.end();
		}

		typename OctreeData::const_iterator cbegin()
		{
			return m_Data.cbegin();
		}

		typename OctreeData::const_iterator cend()
		{
			return m_Data.cend();
		}
	};

}