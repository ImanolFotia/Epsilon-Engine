#pragma once

#include "vulkan/vulkan_core.h"
#if !defined(__ANDROID__)
#include <vulkan/vulkan.hpp>
#endif

#include "core/engine/renderer/types.hpp"
#include "descriptors.hpp"
#include "rasterizer.hpp"
#include "render_pass.hpp"
#include "shader.hpp"
#include "swap_chain.hpp"
#include "vertex_buffer.hpp"
#include "viewport.hpp"
#include <core/framework/containers/static_array.hpp>
#include <functional>

#include "vk_data.hpp"

namespace vk {

// std::pair<>

template <uint32_t num_stages>
static std::array<VkPipelineShaderStageCreateInfo, num_stages>
createShaderStages(const char *vertexPath, const char *fragmentPath, VkShaderModule &vertShaderModule, VkShaderModule &fragShaderModule, VulkanData &vk_data) {

  auto vertShaderCode = shader::readFile(vertexPath /*"../assets/shaders/vertex.spv"*/, vk_data);
  auto fragShaderCode = shader::readFile(fragmentPath /*"../assets/shaders/fragment.spv"*/, vk_data);

  vertShaderModule = shader::createShaderModule(vertShaderCode, vk_data);
  fragShaderModule = shader::createShaderModule(fragShaderCode, vk_data);

  VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
  vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertShaderStageInfo.module = vertShaderModule;
  vertShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
  fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragShaderStageInfo.module = fragShaderModule;
  fragShaderStageInfo.pName = "main";

  return {vertShaderStageInfo, fragShaderStageInfo};
}

static void appendShaderStageMacro(engine::ShaderStageInfo &info) {
  auto versionPosition = [](const std::vector<char> &vec) {
    const char *version_string = "#version";
    int version_length = 8;
    int search_pos = 0;
    int position = -1;
    for (int i = 0; i < vec.size(); i++) {
      if (version_string[search_pos] == vec[i]) {
        if (search_pos == version_length - 1) {
          position = search_pos - version_length - 1;
        } else {
          search_pos++;
        }
      } else {
        search_pos = 0;
      }
    }

    return position;
  };

  auto result = versionPosition(info.shaderCode);

  (void)result;
}

static VkPushConstantRange setupPushConstant(std::size_t size) {
  VkPushConstantRange push_constant;
  push_constant.offset = 0;
  push_constant.size = size;
  push_constant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  return push_constant;
}

static void createPipelineLayout(VulkanData &vk_data, VkPushConstantRange push_constant, VkPipelineLayout &pipelineLayout,
                                 std::vector<VkDescriptorSetLayout> &descriptorSetLayouts) {
  // Pipeline layout
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = descriptorSetLayouts.size(); // Optional
  // pipelineLayoutInfo.pSetLayouts = nullptr;         // Optional
  pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
  if (push_constant.size > 0) {
    pipelineLayoutInfo.pushConstantRangeCount = 1;           // Optional
    pipelineLayoutInfo.pPushConstantRanges = &push_constant; // Optional
  }

  if (vkCreatePipelineLayout(vk_data.logicalDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }
}

static void createGraphicsPipeline(VulkanData &vk_data, VulkanRenderPass &renderPass, engine::RenderPassInfo &renderPassInfo) {

  std::vector<std::function<void(VulkanData &)>> destroyShaderStages;

  std::vector<VkGraphicsPipelineCreateInfo> pipelinesInfo;
  std::vector<std::vector<VkPipelineShaderStageCreateInfo>> shaderStages;

  std::vector<vk::VulkanVertexInfo> vertexInfos;
  std::vector<VkPipelineVertexInputStateCreateInfo> vertexInputCreateInfo;

  std::vector<VkPipelineDepthStencilStateCreateInfo> depthStencil;

  pipelinesInfo.resize(renderPassInfo.numLayouts);
  vertexInfos.resize(renderPassInfo.numLayouts);
  vertexInputCreateInfo.resize(renderPassInfo.numLayouts);
  depthStencil.resize(renderPassInfo.numLayouts);

  VkPipelineDynamicStateCreateInfo dynamicState{};
  VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
  for (int layout_index = 0; layout_index < renderPassInfo.numLayouts; layout_index++) {
    auto &renderPipeline = renderPass.renderPipelines.at(layout_index);
    renderPipeline.numAttachments = renderPass.numAttachments;
    vk::createDescriptorSetLayout(vk_data, renderPipeline.descriptorSetLayouts.at(0), renderPassInfo.bindingInfo, 0);
    // vk::createDescriptorSetLayout(vk_data, renderPipeline.descriptorSetLayouts.at(1), renderPassInfo.bindingInfo, 1);

    auto &pipelineLayout = renderPassInfo.pipelineLayout[layout_index];
    // renderPass.renderPipelines.at(layout_index).numAttachments = renderPass.renderPassChain.ImageViews.size();
    auto &shaderInfo = pipelineLayout.shaderInfo;
    shaderStages.emplace_back();
    shaderStages.back().resize(pipelineLayout.shaderInfo.stages.size());

    for (unsigned i = 0; i < shaderInfo.stages.size(); i++) {
      shaderStages[layout_index][i] = shader::createShaderStage(vk_data, shaderInfo.stages[i]);
      shaderStages[layout_index][i].pName = shaderInfo.stages[i].entryPoint.c_str();
      shaderInfo.stages[i].renderPassIds.push_back(renderPass.id);
      shaderInfo.stages[i].pipelines[renderPass.id].push_back(layout_index);
      destroyShaderStages.emplace_back(
          [stage = shaderStages[layout_index][i]](VulkanData &vk_data) { vkDestroyShaderModule(vk_data.logicalDevice, stage.module, nullptr); });
    }

    // auto shaderStages = createShaderStages<2>("../assets/shaders/vertex.spv", "../assets/shaders/fragment.spv", vertShaderModule, fragShaderModule, vk_data);
    //   Vertex Stage

    if (pipelineLayout.cullMode == engine::CullMode::BACK)
      renderPipeline.cullMode = VK_CULL_MODE_BACK_BIT;
    else if (pipelineLayout.cullMode == engine::CullMode::FRONT)
      renderPipeline.cullMode = VK_CULL_MODE_FRONT_BIT;
    else
      renderPipeline.cullMode = VK_CULL_MODE_NONE;

    if (pipelineLayout.windingMode == engine::WindingMode::COUNTER_CLOCK_WISE)
      renderPipeline.winding = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    else
      renderPipeline.winding = VK_FRONT_FACE_CLOCKWISE;

    vk::VulkanVertexInfo &vertexInfo = vertexInfos.at(layout_index);

    vertexInfo.attributeDescriptions = getAttributeDescriptions(0, pipelineLayout.vertexLayout.descriptors);

    vertexInfo.bindingDescription = getBindingDescription(pipelineLayout.vertexLayout.size);

    VkPipelineVertexInputStateCreateInfo &vertexInputInfo = vertexInputCreateInfo.at(layout_index);
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &vertexInfo.bindingDescription; // Optional
    vertexInputInfo.vertexAttributeDescriptionCount = vertexInfo.attributeDescriptions.size();
    vertexInputInfo.pVertexAttributeDescriptions = vertexInfo.attributeDescriptions.data();

    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    // Viewport stage
    createViewport(vk_data, renderPipeline, renderPass.renderPassChain);
    // Rasterizer stage
    setupRasterizer(renderPipeline, renderPassInfo);

    auto push_constant = setupPushConstant(renderPassInfo.pushConstant.size);

    VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_LINE_WIDTH
        /*VK_DYNAMIC_STATE_LINE_RASTERIZATION_MODE_EXT,
        VK_DYNAMIC_STATE_LINE_STIPPLE_ENABLE_EXT,
        VK_DYNAMIC_STATE_LINE_STIPPLE_EXT,
        VK_DYNAMIC_STATE_LINE_WIDTH*/
    };

    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 3;
    dynamicState.pDynamicStates = dynamicStates;

    auto &pipelineLayoutVK = renderPipeline.pipelineLayout.emplace_back();
    createPipelineLayout(vk_data, push_constant, pipelineLayoutVK, renderPipeline.descriptorSetLayouts);

    // Creating the graphics pipeline
    VkGraphicsPipelineCreateInfo &pipelineInfo = pipelinesInfo.at(layout_index);

    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = shaderStages[layout_index].size();
    pipelineInfo.pStages = shaderStages[layout_index].data();

    if (renderPass.renderPassData.hasDepthAttachment) {
      depthStencil.at(layout_index).sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
      depthStencil.at(layout_index).depthTestEnable = pipelineLayout.depthTestEnable ? VK_TRUE : VK_FALSE;
      depthStencil.at(layout_index).depthWriteEnable = pipelineLayout.depthWriteEnable ? VK_TRUE : VK_FALSE;
      depthStencil.at(layout_index).depthCompareOp = VK_COMPARE_OP_LESS;
      depthStencil.at(layout_index).depthBoundsTestEnable = VK_FALSE;
      depthStencil.at(layout_index).minDepthBounds = 0.0f; // Optional
      depthStencil.at(layout_index).maxDepthBounds = 1.0f; // Optional
      depthStencil.at(layout_index).stencilTestEnable = VK_FALSE;
      depthStencil.at(layout_index).pNext = NULL;
      depthStencil.at(layout_index).flags = 0;
      depthStencil.at(layout_index).front = {}; // Optional
      depthStencil.at(layout_index).back = {};  // Optional
      pipelineInfo.pDepthStencilState = &depthStencil[layout_index];
    }

    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &renderPipeline.viewportState;
    pipelineInfo.pRasterizationState = &renderPipeline.rasterizer;
    pipelineInfo.pMultisampleState = &renderPipeline.multisampling; // Optional
    /*
    for(int i = 0; i < renderPipeline.colorBlending.attachmentCount;i++) {
        renderPipeline.colorBlending.pAttachments[i].blendEnable = VK_FALSE;
    }*/
    pipelineInfo.pColorBlendState = &renderPipeline.colorBlending;

    pipelineInfo.layout = renderPipeline.pipelineLayout.back();

    pipelineInfo.renderPass = renderPass.renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.pDynamicState = &dynamicState;
  }

  // renderPipeline.graphicsPipeline.resize(pipelinesInfo.size());
  std::vector<VkPipeline> pipelines;
  pipelines.resize(pipelinesInfo.size());

  if (auto res = vkCreateGraphicsPipelines(vk_data.logicalDevice, VK_NULL_HANDLE, pipelinesInfo.size(), pipelinesInfo.data(), nullptr, pipelines.data());
      res != VK_SUCCESS) {
    std::cerr << "Result id: " << res << std::endl;
    throw std::runtime_error("failed to create graphics pipeline!");
  }

  for (int i = 0; i < renderPass.renderPipelines.size(); i++)
    renderPass.renderPipelines.at(i).graphicsPipeline = pipelines.at(i);

  renderPass.vertexInfo = vertexInfos;

  for (auto &func : destroyShaderStages) {
    func(vk_data);
  }
}

static void draw(const VkCommandBuffer &commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) {
  vkCmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

static void drawIndexed(const VkCommandBuffer &commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset,
                        uint32_t firstInstance) {
  vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

static void destroyGraphicsPipeline(const VulkanData &vk_data, VulkanRenderPipeline &renderPipeline) {
  for (auto &layout : renderPipeline.pipelineLayout)
    vkDestroyPipelineLayout(vk_data.logicalDevice, layout, nullptr);

  vkDestroyDescriptorSetLayout(vk_data.logicalDevice, renderPipeline.descriptorSetLayouts.at(0), nullptr);

  vkDestroyPipeline(vk_data.logicalDevice, renderPipeline.graphicsPipeline, nullptr);
}

static void bindPipeline(const VkPipeline &renderPipeline, const VkCommandBuffer &commandBuffer) {
  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, renderPipeline);
}
} // namespace vk