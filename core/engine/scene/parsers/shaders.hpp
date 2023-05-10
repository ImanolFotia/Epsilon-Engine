#pragma once 

#include "common.hpp"

#include <core/framework/utils/file.hpp>

#include <unordered_map>

namespace engine::parsers {

	static std::unordered_map<std::string, ShaderInfo> parse_shaders() {

		std::ifstream f("./engine/shaders.json");
		nlohmann::json data = nlohmann::json::parse(f);

		std::unordered_map<std::string, ShaderInfo> shaders;

		auto& shaders_json = data["Shaders"];

		for (auto& shader : shaders_json) {
			auto name = shader["name"].get<std::string>();
			std::vector<ShaderStageInfo> stages;

			for (auto& stage : shader["stages"]) {
				auto shader_code = utils::readFile(stage["shaderCode"]);
				stages.push_back(
					{.entryPoint = stage["entryPoint"], 
					.shaderCode = shader_code , 
					.stage = resolveStage(stage["stage"])
				});
			}

			uint32_t module_stages = 0;

			for (auto& x : shader["usedStages"]) {
				module_stages = module_stages | (uint32_t)resolveStage(x);
			}

			ShaderInfo shaderInfo = {
				.stages = stages,
				.usedStages = (ShaderModuleStage)module_stages };

			shaders[name] = shaderInfo;
		}

		return shaders;

	}
}