#pragma once

#include <core/engine/renderer/resource_manager.hpp>
#include <core/common/common.hpp>

#include "brush.hpp"
#include <cstdint>

namespace Editor
{
	class BrushManager
	{

		const uint32_t MAX_VERTICES = 1000000;

		std::vector<Brush> m_pBrushes;
		common::Vertex *m_pVerticesPtr;
		engine::Ref<engine::Mesh> m_pMesh;

		std::shared_ptr<engine::Context> m_pContextReference;
		engine::Ref<engine::Buffer> m_pVertexData;

		size_t m_pCurrentOffset = 0;

	public:
		BrushManager(std::shared_ptr<engine::Context> context) : m_pContextReference{context}
		{
			m_pVertexData = context->ResourceManager()->createGPUBuffer("vertex_data_buffer", sizeof(common::Vertex) * MAX_VERTICES, engine::BufferStorageType::STORAGE_BUFFER, 1);
			// context->ResourceManager()->createBuffer();
			m_pVerticesPtr = (common::Vertex *)context->ResourceManager()->mapBuffer(m_pVertexData, 0);
			engine::Ref<engine::Buffer> vertexBuffer = engine::Ref<engine::Buffer>::makeEmpty();
			engine::Ref<engine::Buffer> indexBufferRef = engine::Ref<engine::Buffer>::makeEmpty();

			engine::MeshResource meshResource = {
				.vertexBuffer = vertexBuffer,
				.indexBuffer = indexBufferRef,
				.vertexOffset = 0,
				.indexOffset = 0,
				.numVertices = (int32_t)MAX_VERTICES,
				.numIndices = 0};

			m_pMesh = context->ResourceManager()->insertMesh("brush_mesh", meshResource);
		}

		void addBrush(std::vector<common::Vertex> vertices, size_t material_index)
		{
			Brush brush;
			brush.numVerts = vertices.size();
			brush.vertexOffset = m_pCurrentOffset;
			m_pCurrentOffset += brush.numVerts;
			m_pBrushes.push_back(brush);

			for (int i = 0; i < brush.numVerts; i++)
				m_pVerticesPtr[brush.vertexOffset] = vertices[i];
		}
	};
}