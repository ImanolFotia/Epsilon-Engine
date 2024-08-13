#pragma once

#include <vector>
#include <core/common/common.hpp>

namespace Editor {
	std::vector<common::Vertex> toTriangleStrip(std::vector<common::Vertex> vertices, std::vector<uint32_t> indices) {
		std::vector<common::Vertex> out_verts;
		for (int i = 0; i < indices.size(); i++) {
			out_verts.push_back(vertices[indices[i]]);
		}

		return out_verts;
	}
}