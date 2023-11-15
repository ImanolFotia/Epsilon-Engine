#pragma once

#include <core/engine/renderer/resource_manager.hpp>
#include <core/engine/scene/scene.hpp>

namespace Editor::ComputeDispatch {
	engine::Ref<engine::ComputeShader> createComputeShader() {

		using namespace engine;
		auto context = scene->getContext();
		auto& assetManager = scene->getAssetManager();

		auto computeCode = utils::readFile("./assets/shaders/editor/dummyCompute.spv");

		ShaderInfo ComputeShaderInfo = {
			.stages = {
			{
					.entryPoint = "main", .shaderCode = computeCode, .stage = COMPUTE},
					.usedStages = ShaderModuleStage(COMPUTE),
					.name = "dummyComputeShaderInfo"
			}
		};

		engine::ShaderAsset computeShaderAsset;
		computeShaderAsset.name = "Dummy Compute Shader";
		computeShaderAsset.filePaths = { "./assets/shaders/editor/dummyCompute.glsl" };
		computeShaderAsset.spirvFilePaths = { "./assets/shaders/editor/dummyCompute.spv" };

		assetManager.RegisterShader(computeShaderAsset);


		PipelineLayout computeLayout = {
			.shaderInfo = ComputeShaderInfo,
			.name = "DummyComputeLayout"
		};

		engine::ComputeShaderInfo computeShaderInfo;
		computeShaderInfo.name = "Dummy Shader";
		computeShaderInfo.pipelineLayout = computeLayout;
		computeShaderInfo.bindingInfo = {};
		return context->ResourceManager()->createComputeShader(computeShaderInfo);

	}

}