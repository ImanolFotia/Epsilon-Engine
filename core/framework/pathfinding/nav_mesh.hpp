#pragma once

#include <core/common/common.hpp>
#include "graph.hpp"

#include <unordered_map>
#include <unordered_set>

namespace framework {
	class NavMesh {

		using EdgeKey = uint64_t;

		std::unordered_map<uint32_t, std::unordered_set<uint32_t>> m_pConnectedTriangles;

		common::Mesh& m_pMesh;

	public:
		NavMesh(common::Mesh& mesh) : m_pMesh(mesh) {
			CreateNavMesh(mesh);
		}

		void CreateNavMesh(const common::Mesh& mesh) {
			m_pConnectedTriangles.clear();

			std::unordered_map<EdgeKey, std::vector<uint32_t>> m_pSharedEdges;

			using common::Triangle;

			uint32_t numTriangles = mesh.numTriangles();

			//get all triangles that share one edge
			for (uint32_t i = 0; i < numTriangles; i++) {
				Triangle tr = mesh.getTriangle(i);


				EdgeKey key_a = generateKey(tr.vertices[0], tr.vertices[1]);
				EdgeKey key_b = generateKey(tr.vertices[0], tr.vertices[2]);
				EdgeKey key_c = generateKey(tr.vertices[1], tr.vertices[2]);


				m_pSharedEdges[key_a].push_back(tr.id);
				m_pSharedEdges[key_b].push_back(tr.id);
				m_pSharedEdges[key_c].push_back(tr.id);


			}

			for (uint32_t i = 0; i < numTriangles; i++) {
				Triangle tr = mesh.getTriangle(i);

				EdgeKey key_a = generateKey(tr.vertices[0], tr.vertices[1]);
				EdgeKey key_b = generateKey(tr.vertices[0], tr.vertices[2]);
				EdgeKey key_c = generateKey(tr.vertices[1], tr.vertices[2]);

				for (auto& k : m_pSharedEdges[key_a]) {
					if (k != tr.id)
						m_pConnectedTriangles[tr.id].insert(k);
				}

				for (auto& k : m_pSharedEdges[key_b]) {
					if (k != tr.id)
						m_pConnectedTriangles[tr.id].insert(k);
				}

				for (auto& k : m_pSharedEdges[key_c]) {
					if (k != tr.id)
						m_pConnectedTriangles[tr.id].insert(k);
				}

			}
		}
	private:
		uint64_t generateKey(uint32_t a, uint32_t b) {

			if (a > b) {
				uint32_t ta = a;
				a = b;
				b = ta;
			}

			uint64_t key = a;
			key = (key << 32) | b;
			return key;
		}



	};
}