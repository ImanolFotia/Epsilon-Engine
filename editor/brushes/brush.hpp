#pragma once

namespace Editor {
	struct Brush {
		uint16_t numVerts;
		uint16_t numIndices;
		uint32_t vertexOffset;
		uint32_t indexOffset;
		uint16_t material_id;
	};
}