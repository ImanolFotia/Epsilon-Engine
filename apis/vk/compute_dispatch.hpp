#include <vulkan/vulkan.hpp>

#include "shader.hpp"
#include "vk_data.hpp"
#include "core/engine/renderer/types.hpp"

namespace vk {


	static void createComputePipeline(VulkanData& vk_data, engine::ComputeShaderInfo& shaderInfo, VulkanComputeShader& computeShader) {

		createDescriptorSetLayout(vk_data, computeShader.pipeline.descriptorSetLayout, shaderInfo.bindingInfo);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &computeShader.pipeline.descriptorSetLayout;

		if (vkCreatePipelineLayout(vk_data.logicalDevice, &pipelineLayoutInfo, nullptr, &computeShader.pipeline.pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create compute pipeline layout!");
		}

		VkComputePipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		pipelineInfo.layout = computeShader.pipeline.pipelineLayout;
		pipelineInfo.stage = shader::createShaderStage(vk_data, shaderInfo.pipelineLayout.shaderInfo.stages.back());
		

		if (vkCreateComputePipelines(vk_data.logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &computeShader.pipeline.computePipeline) != VK_SUCCESS) {
			throw std::runtime_error("failed to create compute pipeline!");
		}

	}
	static void createComputeShader(VulkanData& vk_data, engine::ComputeShaderInfo& shaderInfo, VulkanComputeShader& computeShader) {
		createComputePipeline(vk_data, shaderInfo, computeShader);
	}
}