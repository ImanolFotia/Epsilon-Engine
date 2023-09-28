#pragma once

#include <list>
#include <memory>

#include <glm/gtx/hash.hpp>

namespace framework {

	template <typename T>
	class NodeItemLocation {

		using underlying_type = std::list<T>;
		typename underlying_type::iterator iterator;
	};

	template<typename T>
	struct NodeItem {
		T data;
		NodeItemLocation<typename std::list<NodeItem<T>>::iterator> data_position;
	};

	template<typename Key, typename T>
	class Node {

	public:


		Node() = default;

		std::list<Key>& Neighbors() {
			return m_pNeighbors;
		}

		void assign(std::initializer_list<Key> neighbors) {
			m_pNeighbors.assign(neighbors);
		}

		void push_back(Key t) {
			m_pNeighbors.push_back(t);
		}


		void data(typename std::list<T>::iterator t) {
			m_pData = t;
		}

		typename std::list<T>::iterator data() {
			return m_pData;
		}

	private:

		std::list<Key> m_pNeighbors;
		typename std::list<T>::iterator m_pData;
	};


	template<typename Key, typename DataType>
	class NodeGraph {

		using NodeData = std::list<DataType>;
		using Nodes = std::unordered_map < Key, Node<Key, DataType> >;

		NodeData m_pNodeData;
		Nodes m_pNodes;

		uint32_t m_pSizeX;
		uint32_t m_pSizeY;

		std::hash<Key> getHash(Key key) {
			return std::hash<Key>{}(key);
		}


		void pBuild() {


			auto inRange = [this](uint32_t x, uint32_t y) {
				return x >= 0 && x < m_pSizeX && y >= 0 && y < m_pSizeY;
			};
			/*
			for (int x = 0; x < m_pSizeX; x++) {
				for (int y = 0; y < m_pSizeY; y++) {
					DataType data = {x, y};
					m_pNodeData.push_back(data);

					//auto hash = getHash(key);
					//m_pNodes[x * m_pSizeX + y] = {};

					auto& current = m_pNodes.at(x * m_pSizeX + y);
					current.data(std::prev(m_pNodeData.end()));

					if (inRange(x - 1, y)) {
						auto key = (x - 1) * m_pSizeX + y;
						auto it = m_pNodes.find(key);
						if (it == m_pNodes.end()) {
							m_pNodes[key] = {};
							it = m_pNodes.find(key);
						}
						current.push_back(it);
					}
					if (inRange(x + 1, y)) {
						auto key = (x + 1) * m_pSizeX + y;
						auto it = m_pNodes.find(key);
						if (it == m_pNodes.end()) {
							m_pNodes[key] = {};
							it = m_pNodes.find(key);
						}
						current.push_back(it);
					}
					if (inRange(x, y - 1)) {
						auto key = x * m_pSizeX + y - 1;
						auto it = m_pNodes.find(key);
						if (it == m_pNodes.end()) {
							m_pNodes[key] = {};
							it = m_pNodes.find(key);
						}
						current.push_back(it);
					}
					if (inRange(x, y + 1)) {
						auto key = x * m_pSizeX + y + 1;
						auto it = m_pNodes.find(key);
						if (it == m_pNodes.end()) {
							m_pNodes[key] = {};
							it = m_pNodes.find(key);
						}
						current.push_back(it);
					}
				}
			}*/
		}
	public:

		NodeGraph(uint32_t sizex, uint32_t sizey) : m_pSizeX(sizex), m_pSizeY(sizey) {
			pBuild();
		}

		NodeGraph() = default;

		void clear() {
			m_pNodeData.clear();
			m_pNodes.clear();
		}

		Node<Key, DataType>& insert(Key key, const DataType& data) {
			NodeItem<DataType> item;
			item.data = data;

			m_pNodeData.push_back(data);
			m_pNodes[key].data(std::prev(m_pNodeData.end()));

			return m_pNodes[key];
		}

		void erase(Key location) {
			if (m_pNodes.contains(location)) {
				auto& neighbors = m_pNodes[location].Neighbors();
				for (auto& n : neighbors) {
					//if (n->second.find(location) != -1) {
					auto& nn = n->second.Neighbors();
					/*std::erase_if(nn, [location](auto x) {
						return x->first == location;
						});*/
						//break;
					//}
				}
				auto it = m_pNodes.find(location);
				//m_pNodes.erase(it);
			}
		}

		std::list<Key>& getNeightbors(Key nodeKey) {
			try {
				return m_pNodes.at(nodeKey).Neighbors();
			}
			catch (std::exception& e) {
				std::cerr << e.what() << std::endl;
			}
		}

		typename Nodes::iterator begin() {
			return m_pNodes.begin();
		}

		typename Nodes::iterator end() {
			return m_pNodes.end();
		}

		typename Nodes::const_iterator cbegin() {
			return m_pNodes.cbegin();
		}

		typename Nodes::const_iterator cend() {
			return m_pNodes.cend();
		}

		size_t size() {
			return m_pNodes.size();
		}

		Node<Key, DataType>& at(Key key) {
			return m_pNodes.at(key);
		}

		bool contains(Key key) {
			return m_pNodes.contains(key);
		}
	};

}