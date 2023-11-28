#pragma once

#include <core/engine/renderer/resource_manager.hpp>
#include <core/engine/scene/scene.hpp>

#include "shaders/forward.hpp"

#include <glm/glm.hpp>

namespace Editor::Renderpasses {

	struct TAAPassPushConstant {
		glm::mat4 transform;
		uint32_t material_index;
	};

	struct TAAUniformData {
		float lerpAmount = 1.0;
		int clampingKernelSize = 1;
		int TotalFrames = 0;
		float _FeedbackMin = 0.1;
		float _FeedbackMax = 0.125;
	};

	struct TAARenderPasses {
		engine::Ref<engine::RenderPass> renderpass[2];
	};

	static TAARenderPasses createTAARenderPass(std::shared_ptr<engine::Scene> scene) {

		using namespace engine;
		auto context = scene->getContext();
		auto& assetManager = scene->getAssetManager();
		/*
		std::vector<char> vertexCode;

		for (int i = 0; i < (sizeof(Shaders::forward_vertex) / sizeof(uint32_t)); i++) {
			for (int j = 0; j < 4; j++) {
				vertexCode.push_back((char)(((char*)&Shaders::forward_vertex[i])[j]));
			}
		}

		std::vector<char> fragmentCode;

		for (int i = 0; i < (sizeof(Shaders::forward_vertex) / sizeof(uint32_t)); i++) {
			for (int j = 0; j < 4; j++) {
				fragmentCode.push_back((char)(((char*)&Shaders::forward_fragment[i])[j]));
			}
		}
		*/
		auto vertexCode = utils::readFile("./assets/shaders/editor/effect_vert.spv");
		auto fragmentCode = utils::readFile("./assets/shaders/editor/TAA.spv");

		ShaderInfo taaShaderInfo = {
			.stages = {
				{.entryPoint = "main", .shaderCode = vertexCode, .stage = VERTEX},
				{.entryPoint = "main", .shaderCode = fragmentCode, .stage = FRAGMENT}
			},
			.usedStages = ShaderModuleStage(VERTEX | FRAGMENT),
			.name = "TAAShader"
		};

		engine::ShaderAsset taaShaderAsset;
		taaShaderAsset.name = "TAAShader";
		taaShaderAsset.filePaths = { "./assets/shaders/editor/TAA.glsl", "./assets/shaders/editor/effect.vert.glsl" };
		taaShaderAsset.spirvFilePaths = { "./assets/shaders/editor/TAA.spv", "./assets/shaders/editor/effect_vert.spv" };


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
		PipelineLayout taaLayout = {
			.shaderInfo = taaShaderInfo,
			.vertexLayout = vertexLayout,
			.cullMode = CullMode::NONE,
			.windingMode = WindingMode::COUNTER_CLOCK_WISE,
			.depthWriteEnable = true,
			.depthTestEnable = false
		};



		assetManager.RegisterShader(taaShaderAsset);

		RenderPassInfo renderPassInfo0 =
			RenderPassFactory()
			.name("TAARenderPass0")
			.depthAttachment(true)
			.isSwapChainAttachment(false)
			.subpasses({})
			.dimensions({ .width = 1280, .height = 720 })

			.inputs({ 
				{
					.size = 385,
					.offset = 0,
					.binding = 0,
					.type = engine::UniformBindingType::UNIFORM_BUFFER},
				{
					.size = sizeof(TAAUniformData),
					.offset = 0,
					.binding = 1,
					.type = engine::UniformBindingType::UNIFORM_BUFFER},
				{

					.size = 0,
					.offset = 0,
					.binding = 2,
					.type = engine::UniformBindingType::TEXTURE_IMAGE_COMBINED_SAMPLER
				},
				{

					.size = 0,
					.offset = 0,
					.binding = 3,
					.type = engine::UniformBindingType::TEXTURE_IMAGE_COMBINED_SAMPLER,
				},
				{

					.size = 0,
					.offset = 0,
					.binding = 4,
					.type = engine::UniformBindingType::TEXTURE_IMAGE_COMBINED_SAMPLER
				},
				{

					.size = 0,
					.offset = 0,
					.binding = 5,
					.type = engine::UniformBindingType::TEXTURE_IMAGE_COMBINED_SAMPLER,
				}
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
						  .name = "TAATarget0",
					},
					{
						  .format = DEPTH_F32_STENCIL_8,
						  .depthStencilValue = {1, 0},
						  .isDepthAttachment = true,
						  .name = "TAADepth0",
					}
				})
			.pipelineLayout(taaLayout)
			.pushConstant(72);

		RenderPassInfo renderPassInfo1 =
			RenderPassFactory()
			.name("TAARenderPass1")
			.depthAttachment(true)
			.isSwapChainAttachment(false)
			.subpasses({})
			.dimensions({ .width = 1280, .height = 720 })

			.inputs({
				{
					.size = 384,
					.offset = 0,
					.binding = 0,
					.set = 0,
					.type = engine::UniformBindingType::UNIFORM_BUFFER,
					.descriptorCount = 1,
					.name = "TAAUniformBuffer" },
				{
					.size = sizeof(TAAUniformData),
					.offset = 0,
					.binding = 1,
					.set = 0,
					.type = engine::UniformBindingType::UNIFORM_BUFFER,
					.descriptorCount = 1,
					.name = "TAADataBuffer" },
				{

					.size = 0,
					.offset = 0,
					.binding = 2,
					.type = engine::UniformBindingType::TEXTURE_IMAGE_COMBINED_SAMPLER,
				},
				{

					.size = 0,
					.offset = 0,
					.binding = 3,
					.type = engine::UniformBindingType::TEXTURE_IMAGE_COMBINED_SAMPLER,
				},
				{

					.size = 0,
					.offset = 0,
					.binding = 4,
					.type = engine::UniformBindingType::TEXTURE_IMAGE_COMBINED_SAMPLER,
				},
				{

					.size = 0,
					.offset = 0,
					.binding = 5,
					.type = engine::UniformBindingType::TEXTURE_IMAGE_COMBINED_SAMPLER,
				}
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
						  .name = "TAATarget1",
					},
					{
						  .format = DEPTH_F32_STENCIL_8,
						  .depthStencilValue = {1, 0},
						  .isDepthAttachment = true,
						  .name = "TAADepth1",
					}
				})
			.pipelineLayout(taaLayout)
			.pushConstant(72);

		TAARenderPasses passes;
		passes.renderpass[0] = context->ResourceManager()->createRenderPass(renderPassInfo0);
		passes.renderpass[1] = context->ResourceManager()->createRenderPass(renderPassInfo1);

		return passes;
	}
}