#pragma once

#include "common.hpp"

namespace engine::parsers {

	static std::unordered_map<std::string, VertexLayout> parse_vertex_descriptors() {

		std::ifstream f("./engine/vertex_descriptors.json");
		nlohmann::json data = nlohmann::json::parse(f);

		std::unordered_map<std::string, VertexLayout> layouts;

		for (auto& descriptor : data["VertexDescriptors"]) {
			auto name = descriptor["name"];
			auto size = descriptor["size"];
			layouts[name].size = size;

			for (auto& layout : descriptor["layout"]) {
				VertexDescriptorInfo info;
				info.format = resolveVertexFormat(layout["type"]);
				info.offset = layout["offset"];
				layouts[name].descriptors.push_back(info);
			}
		}

		return layouts;
	}
}