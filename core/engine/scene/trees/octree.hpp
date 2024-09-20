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

	enum class OctreeItemMask : uint32_t {
		OBJECT 		= 1 << 0,
		LIGHT 		= 1 << 1,
		TRANSPARENT = 1 << 2,
		SHADOW 		= 1 << 3,
		OPAQUE 		= OBJECT | SHADOW,
		ANY 		= OBJECT | LIGHT | TRANSPARENT | SHADOW,
		SIZE		= 6
	};

	inline bool operator|(OctreeItemMask a, OctreeItemMask b) { return (uint32_t)a | (uint32_t)b; }
	inline bool operator&(OctreeItemMask a, OctreeItemMask b) { return (uint32_t)a & (uint32_t)b; }

	template <typename T>
	struct CullResult
	{
		size_t count = 0;
		size_t capacity = 0;
		std::vector<T> items{};

		void reset()
		{
			count = 0;
		}

		void push(const T &val)
		{
			if (count >= capacity)
			{
				items.push_back(val);
				capacity++;
			}
			else
			{
				items[count] = val;
			}
			count++;
		}
	};
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
		OctreeItemMask mask = OctreeItemMask::ANY;
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

		std::array<Octree_ptr, 8> m_Children{};
		std::array<Box, 8> m_bChildren{};

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

		void search(const Box &box, CullResult<T> &cullResult, OctreeItemMask mask)
		{
			for (const auto &p : m_Data)
			{
				if (box.overlaps(p.first) && p.second->mask & mask)
				{
					cullResult.push(p.second);
				}
			}

			for (size_t i = 0; i < 8; i++)
			{
				check_child(i, box, cullResult, mask);
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

		void check_child(int index, const Box &box, CullResult<T> &cullResult, OctreeItemMask mask)
		{
			if (m_Children[index])
			{
				if (box.contains(m_bChildren[index]))
					m_Children[index]->items(cullResult, mask);
				else if (box.overlaps(m_bChildren[index]))
					m_Children[index]->search(box, cullResult, mask);
			}
		}

		void search(Frustum &frustum, CullResult<T> &cullResult, OctreeItemMask mask)
		{
			for (auto &p : m_Data)
			{
				if (frustum.overlaps(p.first) && p.second->mask & mask)
					cullResult.push(p.second);
			}

			for (size_t i = 0; i < 8; i++)
			{
				if (!m_Children[i]->m_IsEmpty)
				{
					if (frustum.contains(m_bChildren[i]))
						m_Children[i]->items(cullResult, mask);
					else if (frustum.overlaps(m_bChildren[i]))
						m_Children[i]->search(frustum, cullResult, mask);
				}
			}
		}

		void search(const BoundingSphere &sphere, CullResult<T> &cullResult, OctreeItemMask mask)
		{
			for (auto &p : m_Data)
			{
				if (sphere.overlaps(p.first) && p.second->mask & mask)
					cullResult.push(p.second);
			}

			for (size_t i = 0; i < 8; i++)
			{
				if (!m_Children[i]->m_IsEmpty)
				{
					if (sphere.contains(m_bChildren[i]))
						m_Children[i]->items(cullResult, mask);
					else if (sphere.overlaps(m_bChildren[i]))
						m_Children[i]->search(sphere, cullResult, mask);
				}
			}
		}

		void items(CullResult<T> &cullResult, OctreeItemMask mask) const
		{
			for (auto &i : m_Data)
				if(i.second->mask & mask)
					cullResult.push(i.second);

			for (auto &c : m_Children)
			{
				if (!c->m_IsEmpty)
					c->items(cullResult, mask);
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
		enum CullPass
		{
			VISIBILITY = 0,
			SHADOW,
			PROBE,
			SIZE
		};

	private:
		std::array<CullResult<typename OctreeData::iterator>, CullPass::SIZE> m_pCullPasses;

	public:
		OctreeContainer(Box minmax, int depth = 0) : m_Root(minmax, depth)
		{
		}

		auto insert(Box pos, T data, OctreeItemMask mask = OctreeItemMask::ANY)
		{
			OctreeItem<T> item{};
			item.data = data;

			m_Data.push_back(item);
			// std::cout << "Inserting data: " << data
			//           << " with address: " << &m_Data.back() << std::endl;
			auto last = std::prev(m_Data.end());
			m_Data.back().data_position = m_Root.insert(pos, last);
			m_Data.back().mask = mask;

			return last;
		}

		CullResult<typename OctreeData::iterator> &search(Frustum &frustum, CullPass pass, OctreeItemMask mask)
		{
			m_pCullPasses[pass].reset();
			m_Root.search(frustum, m_pCullPasses[pass], mask);
			return m_pCullPasses[pass];
		}

		CullResult<typename OctreeData::iterator> &search(const Box &box, CullPass pass, OctreeItemMask mask)
		{
			m_pCullPasses[pass].reset();
			m_Root.search(box, m_pCullPasses[pass], mask);
			return m_pCullPasses[pass];
		}

		CullResult<typename OctreeData::iterator> &search(BoundingSphere &sphere, CullPass pass, OctreeItemMask mask)
		{
			m_pCullPasses[pass].reset();
			m_Root.search(sphere, m_pCullPasses[pass], mask);
			return m_pCullPasses[pass];
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