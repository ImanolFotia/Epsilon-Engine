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

#include "../structs/frustum.hpp"

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
        T data;
        OctreeItemLocation<typename std::list<OctreeItem<T>>::iterator> data_position;
    };

    template <typename T>
    class Octree
    {
        using Octree_ptr = std::shared_ptr<Octree<T>>;

        Box m_Box;

        bool m_IsBuilt = false;
        bool m_IsLeaf = false;

        bool m_IsEmpty = true;

        int m_Depth = 0;

        std::array<Octree_ptr, 8> m_Children;
        std::array<Box, 8> m_bChildren;

        std::array<std::thread, 8> m_Workers;

        std::list<std::pair<Box, T>> m_Data;

        std::mutex m_ItemsMutex;

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
                    std::lock_guard<std::mutex> guard(m_ItemsMutex);
                    items.push_back(p.second);
                }
            }

            for (size_t i = 0; i < 8; i++)
            {
                if (m_Depth == 55)
                {
                    m_Workers[i] = std::thread([this, &i, &box, &items]
                                               { check_child(i, box, items); });
                }
                else
                {
                    check_child(i, box, items);
                }
            }

            for (auto &worker : m_Workers)
            {
                if (worker.joinable())
                {
                    std::cout << "Thread " << worker.get_id() << " has finished." << std::endl;
                    worker.join();
                }
            }
        }

        void check_child(int index, const Box &box, std::list<T> &items) const
        {
            if (m_Children[index])
            {
                if (box.canContain(m_bChildren[index]))
                    m_Children[index]->items(items);
                else if (box.overlaps(m_bChildren[index]))
                    m_Children[index]->search(box, items);
            }
        }

        auto search(const Frustum &frustum) -> std::list<T>
        {
            std::list<T *> items;
            search(frustum, &items);
            return items;
        }

        void search(const Frustum &frustum, std::list<T> &items) const
        {
            for (const auto &p : m_Data)
            {
                if (frustum.overlaps(p.first))
                    items.push_back(p.second);
            }

            for (size_t i = 0; i < 8; i++)
            {
                if (m_Children[i])
                {
                    if (frustum.contains(m_bChildren[i]))
                        m_Children[i].items(items);
                    else if (frustum.overlaps(m_bChildren[i]))
                        m_Children[i].search(frustum, items);
                }
            }
        }

        std::list<T *> &items() const
        {
            std::list<T *> listItems;
            items(listItems);
            return listItems;
        }

        void items(std::list<T> items) const
        {
            for (auto &i : m_Data)
                items.push_back(i.second);

            for (auto &c : m_Children)
            {
                if (c)
                    c->items(items);
            }
        }

        OctreeItemLocation<T> insert(const Box &box, const T &data)
        {
            m_IsEmpty = false;
            if (m_IsBuilt)
            {
                for (int i = 0; i < 8; i++)
                {
                    if (m_bChildren[i].canContain(box))
                    {
                        if (m_Depth < MAX_DEPTH)
                        {
                            return m_Children[i]->insert(box, data);
                        }
                    }
                }
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

        void insert(Box pos, const T &data)
        {
            OctreeItem<T> item;
            item.data = data;

            m_Data.push_back(item);
            // std::cout << "Inserting data: " << data
            //           << " with address: " << &m_Data.back() << std::endl;
            m_Data.back().data_position = m_Root.insert(pos, std::prev(m_Data.end()));
        }

        void search(const Frustum &frustum) {}

        std::list<typename OctreeData::iterator> search(const Box &box)
        {
            return m_Root.search(box);
        }

        void erase(typename OctreeData::iterator &item)
        {
            item->data_position.container->erase(item->data_position.iterator);

            m_Data.erase(item);
        }

        void relocate(typename OctreeData::iterator &item, const Box &box)
        {
            item->data_position.c->erase(item->data_position.iterator);

            item->data_position = m_Root.insert(box, m_Data.end());
        }

        OctreeData &items()
        {
            return m_Data;
        }

        void erase() {}

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