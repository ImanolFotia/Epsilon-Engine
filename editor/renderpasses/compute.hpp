#pragma once

#include <core/engine/renderer/resource_manager.hpp>
#include <core/engine/scene/scene.hpp>

namespace Editor::ComputeDispatch {
	static engine::Ref<engine::ComputeShader> createComputeShader(std::shared_ptr<engine::Scene> scene) {

		using namespace engine;
		auto context = scene->getContext();
		auto assetManager = scene->getAssetManager();

		auto computeCode = utils::readFile("./assets/shaders/editor/dummyCompute.spv");

		ShaderInfo ComputeShaderInfo = {
			.stages = {
			{
					.entryPoint = "main", .shaderCode = computeCode, .stage = COMPUTE
			}},
					.usedStages = ShaderModuleStage(COMPUTE),
					.name = "dummyComputeShaderInfo"
		};

		engine::ShaderAsset computeShaderAsset;
		computeShaderAsset.name = "Dummy Compute Shader";
		computeShaderAsset.filePaths = { "./assets/shaders/editor/dummyCompute.glsl" };
		computeShaderAsset.spirvFilePaths = { "./assets/shaders/editor/dummyCompute.spv" };

		assetManager->RegisterShader(computeShaderAsset);


		PipelineLayout computeLayout = {
			.shaderInfo = ComputeShaderInfo,
			.name = "DummyComputeLayout"
		};

		engine::ComputeShaderInfo computeShaderInfo;
		computeShaderInfo.name = "Dummy Shader";
		computeShaderInfo.pipelineLayout = computeLayout;
		computeShaderInfo.bindingInfo = {};
		computeShaderInfo.groupCountX = 256;
		return context->ResourceManager()->createComputeShader(computeShaderInfo);

	}

}