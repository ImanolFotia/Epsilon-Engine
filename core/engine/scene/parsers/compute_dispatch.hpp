#pragma once

#include "common.hpp"
#include "pipelines.hpp"

#include <core/engine/renderer/types.hpp>

namespace engine::parsers {

	static std::unordered_map<std::string, engine::ComputeShaderInfo> parse_compute_shaders() {

		auto shader_info = parse_shaders();
		auto pipelines = parse_pipelines();

		std::unordered_map<std::string, engine::ComputeShaderInfo> computePipelines;

		for (auto& [name, layout] : pipelines) {
			if (layout["type"] == "ComputePipeline") {
				engine::ComputeShaderInfo shaderInfo;
				shaderInfo.bindingInfo.push_back
				computePipelines[layout["name"]] = shaderInfo;
			}
		}

		return computePipelines;
	}
}