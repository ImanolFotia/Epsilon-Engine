#pragma once

#include <core/engine/renderer/resource_manager.hpp>

#include "brush.hpp"

namespace Editor {
	class NodeManager {

		const uint32_t MAX_VERTICES = 10000000;
		const uint32_t MAX_INDICES = 20000000;

		std::vector<Brush> m_pBrushes;
		engine::Ref<Mesh> m_pMesh;

		std::shared_ptr<engine::Context> m_pContextReference;

	public:
		NodeManager(std::shared_ptr<engine::Context> context) : m_pContextReference{ context } {
			
			context->ResourceManager()->createBuffer();

			MeshResource meshResource = {
				.vertexBuffer = vertexBufferRef,
				.indexBuffer = indexBufferRef,
				.vertexOffset = vertexBuffer->allocatedVertices,
				.indexOffset = indexBuffer->allocatedVertices,
				.numVertices = (uint32_t)vertices->size(),
				.numIndices = (uint32_t)indices->size() };

				m_pMesh = context->ResourceManager()->insertMesh({ .vertices = mesh.Vertices,
													.indices = mesh.Indices,
													.name = name });
		}

		void addBrush(std::vector<common::Vertex> vertices, size_t material_index) {

		}
	};
}