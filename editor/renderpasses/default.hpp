#pragma once

#include <core/engine/renderer/resource_manager.hpp>
#include <core/engine/scene/scene.hpp>

#include "shaders/default.hpp"

namespace Editor::Renderpasses {
	static engine::Ref<engine::RenderPass> createDefaultRenderPass (std::shared_ptr<engine::Scene> scene){

		using namespace engine;
		auto context = scene->getContext();
		auto& assetManager = scene->getAssetManager();
		/*
		std::vector<char> vertexCode;

		for (int i = 0; i < (sizeof(Shaders::vertex) / sizeof(uint32_t)); i++) {
			for (int j = 0; j < 4; j++) {
				vertexCode.push_back((char)(((char*)&Shaders::vertex[i])[j]));
			}
		}

		std::vector<char> fragmentCode;

		for (int i = 0; i < (sizeof(Shaders::fragment) / sizeof(uint32_t)); i++) {
			for (int j = 0; j < 4; j++) {
				fragmentCode.push_back((char)(((char*)&Shaders::fragment[i])[j]));
			}
		}
		*/
		auto vertexCode = utils::readFile("./assets/shaders/editor/vertex.spv");
		auto fragmentCode = utils::readFile("./assets/shaders/editor/frag.spv");
		ShaderInfo mainShaderInfo = {
			.stages = {
				{.entryPoint = "main", .shaderCode = vertexCode, .stage = VERTEX},
				{.entryPoint = "main", .shaderCode = fragmentCode, .stage = FRAGMENT}},
			.usedStages = ShaderModuleStage(VERTEX | FRAGMENT),
			.name = "SwapChainShader" 
		};

		engine::ShaderAsset shapChainShaderAsset;
		shapChainShaderAsset.name = "SwapChainShader";
		shapChainShaderAsset.filePaths = { "./assets/shaders/editor/fragment.frag.glsl", "./assets/shaders/editor/vertex.vert.glsl" };
		shapChainShaderAsset.spirvFilePaths = { "./assets/shaders/editor/frag.spv", "./assets/shaders/editor/vertex.spv" };

		assetManager.RegisterShader(shapChainShaderAsset);

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
			.windingMode = WindingMode::CLOCKWISE };

		RenderPassInfo renderPassInfo =
			RenderPassFactory()
			.name("Default")
			.depthAttachment(true)
			.isSwapChainAttachment(true)
			.subpasses({})
			.dimensions({ .width = 1280, .height = 720 })
			.inputs({})
			.outputs({ {
						  .format = COLOR_RGBA,
						  .blendEnable = true,
						  .clearColor = {0.1f, 0.1f, 0.1f, 1.0001f},
						  .isDepthAttachment = false,
						  .isSwapChainAttachment = true,
						  .clearAttachment = true,
					  },
					  {.format = DEPTH_F32_STENCIL_8,
					   .depthStencilValue = {1, 0},
					   .isDepthAttachment = true} })
					   .pipelineLayout(mainLayout)
			.pushConstant(256);

		return context->ResourceManager()->createDefaultRenderPass(renderPassInfo);
	}
}