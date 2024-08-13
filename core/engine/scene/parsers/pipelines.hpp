#pragma once

#include "common.hpp"
#include "shaders.hpp"
#include "vertex_layouts.hpp"

namespace engine::parsers {

	static std::unordered_map<std::string, PipelineLayout> parse_pipelines() {

		std::ifstream f("./engine/pipeline_layouts.json");
		nlohmann::json data = nlohmann::json::parse(f);

		auto shader_info = parse_shaders();
		auto vertex_descriptors = parse_vertex_descriptors();

		auto& layouts_json = data["PipelineLayouts"];

		std::unordered_map<std::string, PipelineLayout> layouts;

		for (auto& layout : layouts_json) {

			PipelineLayout tLayout = {
				.shaderInfo = shader_info[layout["shaderInfo"]],
				.vertexLayout = vertex_descriptors[layout["vertexLayout"]],
				.cullMode = resolveCullMode(layout["cullMode"]),
				.windingMode = resolveWindingMode(layout["windingMode"]),
				.depthWriteEnable = layout["depthWriteEnable"],
				.depthTestEnable = layout["depthTestEnable"],
				.name = layout["name"]
			};

			layouts[layout["name"]] = tLayout;
		}

		return layouts;

	}
}