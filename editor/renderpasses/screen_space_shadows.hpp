#pragma once

#include <core/engine/renderer/resource_manager.hpp>
#include <core/engine/scene/scene.hpp>

#include "shaders/forward.hpp"

#include <glm/glm.hpp>

namespace Editor::Renderpasses {

	struct SSSPassPushConstant {
		glm::mat4 transform;
		uint32_t material_index;
	};

	struct SSSUniformData {
		int step_count = 16;
		float depth_threshold = 0.02;
		float step_size = 0.1;
	};


	static engine::Ref<engine::RenderPass> createSSSRenderPass(std::shared_ptr<engine::Scene> scene) {

		using namespace engine;
		auto context = scene->getContext();
		auto assetManager = scene->getAssetManager();
		
		auto vertexCode = utils::readFile("./assets/shaders/editor/effect_vert.spv");
		auto fragmentCode = utils::readFile("./assets/shaders/editor/SSS.spv");

		ShaderInfo sssShaderInfo = {
			.stages = {
				{.entryPoint = "main", .shaderCode = vertexCode, .stage = VERTEX},
				{.entryPoint = "main", .shaderCode = fragmentCode, .stage = FRAGMENT}
			},
			.usedStages = ShaderModuleStage(VERTEX | FRAGMENT),
			.name = "SSSShader"
		};

		engine::ShaderAsset sssShaderAsset;
		sssShaderAsset.name = "SSSShader";
		sssShaderAsset.filePaths = { "./assets/shaders/editor/SSS.glsl", "./assets/shaders/editor/effect.vert.glsl" };
		sssShaderAsset.spirvFilePaths = { "./assets/shaders/editor/SSS.spv", "./assets/shaders/editor/effect_vert.spv" };


		VertexLayout vertexLayout = {
			.descriptors = {
				{XYZ_FLOAT, offsetof(common::Vertex, position)},
				{XY_FLOAT, offsetof(common::Vertex, texCoords)},
				{XYZ_FLOAT, offsetof(common::Vertex, normal)},
				{XYZW_FLOAT, offsetof(common::Vertex, color)},
				{XYZ_FLOAT, offsetof(common::Vertex, tangent)},
				{XYZ_FLOAT, offsetof(common::Vertex, bitangent)}
			},
			.size = sizeof(common::Vertex)
		};
		PipelineLayout sssLayout = {
			.shaderInfo = sssShaderInfo,
			.vertexLayout = vertexLayout,
			.cullMode = CullMode::NONE,
			.windingMode = WindingMode::COUNTER_CLOCK_WISE,
			.depthWriteEnable = true,
			.depthTestEnable = false
		};



		assetManager->RegisterShader(sssShaderAsset);

		RenderPassInfo renderPassInfo =
			RenderPassFactory()
			.name("SSSRenderPass")
			.depthAttachment(true)
			.isSwapChainAttachment(false)
			.subpasses({})
			.dimensions({ .width = 1280, .height = 720 })

			.inputs({ 
				{
					.size = 385,
					.offset = 0,
					.binding = 0,
					.set = 0,
					.type = engine::UniformBindingType::UNIFORM_BUFFER,
					.descriptorCount = 1,
					.name = "SSSUniformBuffer"
				},
				{
					.size = sizeof(SSSUniformData),
					.offset = 0,
					.binding = 1,
					.set = 0,
					.type = engine::UniformBindingType::UNIFORM_BUFFER,
					.descriptorCount = 1,
					.name = "SSSDataBuffer"
				},
				{

					.size = 0,
					.offset = 0,
					.binding = 2,
					.type = engine::UniformBindingType::TEXTURE_IMAGE_COMBINED_SAMPLER
				},
				})
			.outputs({
					{
						  .format = COLOR_RGBA_16F,
						  .wrapMode = engine::WrapMode::CLAMP_TO_EDGE,
						  .blendEnable = false,
						  .clearColor = {1.0f, 1.0f, 1.0f, 1.0f},
						  .isSampler = true,
						  .isDepthAttachment = false,
						  .isSwapChainAttachment = false,
						  .clearAttachment = true,
						  .name = "SSSTarget",
					},
					{
						  .format = DEPTH_F32_STENCIL_8,
						  .depthStencilValue = {1, 0},
						  .isDepthAttachment = true,
						  .name = "SSSDepth",
					}
				})
			.pipelineLayout(sssLayout)
			.pushConstant(72);


		auto pass = context->ResourceManager()->createRenderPass(renderPassInfo);

		return pass;
	}
}