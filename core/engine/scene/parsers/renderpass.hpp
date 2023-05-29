#pragma once
#include "common.hpp"

#include "pipelines.hpp"

namespace engine::parsers {


	static std::unordered_map<std::string, RenderPassInfo> parse_renderpasses() {


		std::ifstream f("./engine/render_passes.json");
		nlohmann::json data = nlohmann::json::parse(f);

		auto pipelines = parse_pipelines();

		auto& renderpasses_json = data["RenderPasses"];
		std::unordered_map<std::string, RenderPassInfo> renderpasses;

		for (auto& renderpass : renderpasses_json) {
			auto name = renderpass["name"];

			std::vector<UniformBindingInfo> inputs;

			for (auto& input_json : renderpass["inputs"]) {
				auto type = resolveType(input_json["type"]);
					//if (type == UniformBindingType::TEXTURE_SAMPLER) {
					//	RenderPassBinding binding;
					//	binding.bindingPoint = input_json["binding"];
					//	binding.index = input_json["index"];
					//	binding.renderPass = input_json["renderpass"];
					//}
					//else {

						UniformBindingInfo info;
						info.type = resolveType(input_json["type"]);
						info.name = input_json["name"];
						info.binding = input_json["binding"];
						info.size = input_json["size"];
						info.offset = input_json["offset"];
						info.set = input_json["set"];
						info.descriptorCount = input_json["descriptorCount"];
						inputs.push_back(info);
					//}
			}

			std::vector<RenderPassAttachment> outputs;
			for (auto& output_json : renderpass["outputs"]) {
				RenderPassAttachment info;
				info.name = output_json["name"];

				if (output_json.contains("format")) {
					info.format = resolveFormat(output_json["format"]);
				}

				if (output_json.contains("wrapMode")) {
					info.wrapMode = resolveWrapMode(output_json["wrapMode"]);
				}

				if (output_json.contains("filtering")) {
					info.filtering = resolveFiltering(output_json["filtering"]);
				}

				if (output_json.contains("compareFunc")) {
					info.compareFunc = resolveCompareFunction(output_json["compareFunc"]);
				}

				if (output_json.contains("clearColor")) {
					for (int i = 0; i < 4; i++) {
						info.clearColor[i] = output_json["clearColor"][i];
					}
				}

				if (output_json.contains("depthStencilValue")) {
					for (int i = 0; i < 2; i++) {
						info.depthStencilValue[i] = output_json["depthStencilValue"][i];
					}
				}

				if (output_json.contains("isSampler")) {
					info.isSampler = output_json["isSampler"];
				}

				if (output_json.contains("isDepthAttachment")) {
					info.isDepthAttachment = output_json["isDepthAttachment"];
				}
				if (output_json.contains("isSwapChainAttachment")) {
					info.isSwapChainAttachment = output_json["isSwapChainAttachment"];
				}

				if (output_json.contains("blendEnable")) {
					info.blendEnable = output_json["blendEnable"];
				}


				outputs.push_back(info);

			}

			RenderPassInfo renderPassInfo =
				RenderPassFactory()
				.name(name)
				.depthAttachment(renderpass["depthAttachment"])
				.isSwapChainAttachment(renderpass["isSwapChainAttachment"])
				.subpasses({})
				.dimensions({ .width = renderpass["dimensions"]["width"], .height = renderpass["dimensions"]["height"] })
				.inputs(inputs)
				.outputs(outputs)
				.pushConstant(renderpass["pushConstant"]);

			for (auto& pLayout : renderpass["pipelineLayouts"]) {
				renderPassInfo.pipelineIndices[pLayout] = renderPassInfo.pipelineLayout.size();
				renderPassInfo.pipelineLayout.push_back(pipelines[pLayout]);
				renderPassInfo.numLayouts++;
				renderPassInfo.numDescriptors = pipelines[pLayout].vertexLayout.descriptors.size();
			}

			renderpasses[name] = renderPassInfo;

		}
		return renderpasses;
	}
}