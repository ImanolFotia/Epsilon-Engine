#pragma once

#include <core/engine/renderer/resource_manager.hpp>
#include <core/engine/scene/scene.hpp>

#include "shaders/forward.hpp"

namespace Editor::Renderpasses {

	struct ForwardPassPushConstant {
		glm::mat4 transform;
		uint32_t material_index;
	};

	static engine::Ref<engine::RenderPass> createForwardRenderPass(std::shared_ptr<engine::Scene> scene) {

		using namespace engine;
		auto context = scene->getContext();
		auto assetManager = scene->getAssetManager();
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
		auto vertexCode = utils::readFile("./assets/shaders/editor/forward_vertex.spv");
		auto fragmentCode = utils::readFile("./assets/shaders/editor/forward_frag.spv");

		ShaderInfo mainShaderInfo = {
			.stages = {
				{.entryPoint = "main", .shaderCode = vertexCode, .stage = VERTEX},
				{.entryPoint = "main", .shaderCode = fragmentCode, .stage = FRAGMENT}
			},
			.usedStages = ShaderModuleStage(VERTEX | FRAGMENT),
			.name = "ForwardShader"
		};

		engine::ShaderAsset forwardShaderAsset;
		forwardShaderAsset.name = "ForwardShader";
		forwardShaderAsset.filePaths = { "./assets/shaders/editor/forward.frag.glsl", "./assets/shaders/editor/forward.vert.glsl" };
		forwardShaderAsset.spirvFilePaths = { "./assets/shaders/editor/forward_frag.spv", "./assets/shaders/editor/forward_vertex.spv" };


		auto SkyVertexCode = utils::readFile("./assets/shaders/editor/sky-vertex.spv");
		auto SkyFragmentCode = utils::readFile("./assets/shaders/editor/sky-fragment.spv");

		ShaderInfo skyShaderInfo = {
			.stages = {
				{.entryPoint = "main", .shaderCode = SkyVertexCode, .stage = VERTEX},
				{.entryPoint = "main", .shaderCode = SkyFragmentCode, .stage = FRAGMENT}
			},
			.usedStages = ShaderModuleStage(VERTEX | FRAGMENT),
			.name = "SkyShader"
		};
		engine::ShaderAsset skyShaderAsset;
		skyShaderAsset.name = "SkyShader";
		skyShaderAsset.filePaths = { "./assets/shaders/editor/sky.frag.glsl", "./assets/shaders/editor/sky.vert.glsl" };
		skyShaderAsset.spirvFilePaths = { "./assets/shaders/editor/sky-fragment.spv", "./assets/shaders/editor/sky-vertex.spv" };


		auto GrassVertexCode = utils::readFile("./assets/shaders/editor/grass-vertex.spv");
		auto GrassFragmentCode = utils::readFile("./assets/shaders/editor/grass-fragment.spv");

		ShaderInfo grassShaderInfo = {
			.stages = {
				{.entryPoint = "main", .shaderCode = GrassVertexCode, .stage = VERTEX},
				{.entryPoint = "main", .shaderCode = GrassFragmentCode, .stage = FRAGMENT}
			},
			.usedStages = ShaderModuleStage(VERTEX | FRAGMENT),
			.name = "GrassShader"
		};
		engine::ShaderAsset grassShaderAsset;
		grassShaderAsset.name = "GrassShader";
		grassShaderAsset.filePaths = { "./assets/shaders/editor/grass.frag.glsl", "./assets/shaders/editor/grass.vert.glsl" };
		grassShaderAsset.spirvFilePaths = { "./assets/shaders/editor/grass-fragment.spv", "./assets/shaders/editor/grass-vertex.spv" };


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
		PipelineLayout mainLayout = {
			.shaderInfo = mainShaderInfo,
			.vertexLayout = vertexLayout,
			.cullMode = CullMode::BACK,
			.windingMode = WindingMode::COUNTER_CLOCK_WISE,
			.depthWriteEnable = true,
			.depthTestEnable = true 
		};


		PipelineLayout skyLayout = {
			.shaderInfo = skyShaderInfo,
			.vertexLayout = vertexLayout,
			.cullMode = CullMode::FRONT,
			.windingMode = WindingMode::COUNTER_CLOCK_WISE,
			.depthWriteEnable = true,
			.depthTestEnable = false
		};

		PipelineLayout grassLayout = {
			.shaderInfo = grassShaderInfo,
			.vertexLayout = vertexLayout,
			.cullMode = CullMode::NONE,
			.windingMode = WindingMode::COUNTER_CLOCK_WISE,
			.depthWriteEnable = true,
			.depthTestEnable = true
		};

		auto gridFragmentCode = utils::readFile("./assets/shaders/editor/grid-fragment.spv");
		auto gridVertexCode = utils::readFile("./assets/shaders/editor/grid-vertex.spv");
		ShaderInfo gridShaderInfo = mainShaderInfo;
		gridShaderInfo.stages[0].shaderCode = gridVertexCode;
		gridShaderInfo.stages[1].shaderCode = gridFragmentCode;
		gridShaderInfo.name = "GridShader";
		PipelineLayout gridLayout = mainLayout;
		gridLayout.shaderInfo = gridShaderInfo;
		gridLayout.depthTestEnable = false;
		gridLayout.depthWriteEnable = true;
		gridLayout.cullMode = CullMode::FRONT;

		engine::ShaderAsset gridShaderAsset;
		gridShaderAsset.name = "GridShader";
		gridShaderAsset.filePaths = { "./assets/shaders/editor/grid.frag.glsl", "./assets/shaders/editor/forward.vert.glsl" };
		gridShaderAsset.spirvFilePaths = { "./assets/shaders/editor/grid-fragment.spv", "./assets/shaders/editor/forward_vertex.spv" };

		assetManager->RegisterShader(forwardShaderAsset);
		assetManager->RegisterShader(skyShaderAsset);
		assetManager->RegisterShader(gridShaderAsset);
		assetManager->RegisterShader(grassShaderAsset);

		RenderPassInfo renderPassInfo =
			RenderPassFactory()
			.name("Forward")
			.depthAttachment(true)
			.isSwapChainAttachment(false)
			.subpasses({})
			.dimensions({ .width = 1280, .height = 720 })
			
			.inputs({ {
					.size = 384,
					.offset = 0,
					.binding = 0,
					.set = 0,
					.type = engine::UniformBindingType::UNIFORM_BUFFER,
					.descriptorCount = 1,
					.name = "ForwardUniformBuffer" },
				{
					 .size = 4800000,
					 .offset = 0,
					 .binding= 1,
					 .set= 0,
					 .type= engine::UniformBindingType::SHADER_STORAGE,
					 .descriptorCount= 1,
					 .name = "material_buffer"
				},
				{
					.size = 6400000,
					.offset = 0,
					.binding = 5,
					.set = 0,
					.type = engine::UniformBindingType::SHADER_STORAGE,
					.descriptorCount = 1,
					.name = "transform_buffer",
				},
				{
					.size = 1200000,
					.offset = 0,
					.binding = 6,
					.set = 0,
					.type = engine::UniformBindingType::SHADER_STORAGE,
					.descriptorCount = 1,
					.name = "object_buffer",
				}, })
			.outputs({
					{
						  .format = COLOR_RGBA_16F,
						  .wrapMode = engine::WrapMode::CLAMP_TO_EDGE,
						  .blendEnable = true,
						  .clearColor = {0.1f, 0.1f, 0.1f, 1.0f},
						  .isSampler = true,
						  .isDepthAttachment = false,
						  .isSwapChainAttachment = false,
						  .clearAttachment = true,
						  .name = "Forward0",
					},
					{
						  .format = COLOR_RG_32F,
						  .wrapMode = engine::WrapMode::CLAMP_TO_EDGE,
						  .blendEnable = false,
						  .clearColor = {0.0f, 0.0f, 0.0f, 1.0f},
						  .isSampler = true,
						  .isDepthAttachment = false,
						  .isSwapChainAttachment = false,
						  .clearAttachment = true,
						  .name = "VelocityBuffer",
					},
					{
						  .format = DEPTH_F32_STENCIL_8,
						  .wrapMode = engine::WrapMode::CLAMP_TO_EDGE,
						  .depthCompare = false,
						  .blendEnable = false,
						  .depthStencilValue = {1, 0},
						  .isSampler = true,
						  .isDepthAttachment = true,
						  .clearAttachment = true,
						  .name = "ForwardDepth",
					}
				})
			.pipelineLayout(mainLayout)
			.pipelineLayout(skyLayout)
			.pipelineLayout(gridLayout)
			.pipelineLayout(grassLayout)
			.pushConstant(72);

		return context->ResourceManager()->createRenderPass(renderPassInfo);
	}
}