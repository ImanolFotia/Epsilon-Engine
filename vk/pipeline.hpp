#pragma once

#include <vulkan/vulkan.hpp>

#include "shader.hpp"
#include "swap_chain.hpp"
#include "viewport.hpp"
#include "rasterizer.hpp"
#include "render_pass.hpp"

#include "../engine/renderers/vk_data.hpp"

namespace vk
{

    //std::pair<>

    static VkPipeline createGraphicsPipeline(engine::vk_data_t& vk_data)
    {
        auto vertShaderCode = shader::readFile("../assets/shaders/vertex.spv", vk_data);
        auto fragShaderCode = shader::readFile("../assets/shaders/fragment.spv", vk_data);

        VkShaderModule vertShaderModule = shader::createShaderModule(vertShaderCode, vk_data);
        VkShaderModule fragShaderModule = shader::createShaderModule(fragShaderCode, vk_data);

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

        VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

        // Vertex Stage
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
        vertexInputInfo.vertexAttributeDescriptionCount = 0;
        vertexInputInfo.pVertexAttributeDescriptions = nullptr;

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        // Viewport stage
        createViewport(vk_data);

        // Rasterizer stage
        setupRasterizer(vk_data);

        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_LINE_WIDTH};

        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        // Pipeline layout
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;            // Optional
        pipelineLayoutInfo.pSetLayouts = nullptr;         // Optional
        pipelineLayoutInfo.pushConstantRangeCount = 0;    // Optional
        pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

        if (vkCreatePipelineLayout(vk_data.logicalDevice, &pipelineLayoutInfo, nullptr, &vk_data.pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }

        // Creating the graphics pipeline

        VkGraphicsPipelineCreateInfo pipelineInfo = {};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;

        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &vk_data.viewportState;
        pipelineInfo.pRasterizationState = &vk_data.rasterizer;
        pipelineInfo.pMultisampleState = &vk_data.multisampling;// Optional
        pipelineInfo.pColorBlendState = &vk_data.colorBlending;
       
        pipelineInfo.layout = vk_data.pipelineLayout;

        pipelineInfo.renderPass = vk_data.renderPass;
        pipelineInfo.subpass = 0;
        //pipelineInfo.pDynamicState = &dynamicState;

        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
        

        if (auto res = vkCreateGraphicsPipelines(vk_data.logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &vk_data.graphicsPipeline); res != VK_SUCCESS)
        {
            std::cerr << "Result id: " << res << std::endl;
            throw std::runtime_error("failed to create graphics pipeline!");
        }

        vkDestroyShaderModule(vk_data.logicalDevice, fragShaderModule, nullptr);
        vkDestroyShaderModule(vk_data.logicalDevice, vertShaderModule, nullptr);

        return vk_data.graphicsPipeline;
    }

    static void draw(const VkCommandBuffer &commandBuffer,
              uint32_t vertexCount,
              uint32_t instanceCount,
              uint32_t firstVertex,
              uint32_t firstInstance)
    {
        vkCmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
    }

    static void destroyGraphicsPipeline(const engine::vk_data_t& vk_data)
    {
        vkDestroyPipeline(vk_data.logicalDevice, vk_data.graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(vk_data.logicalDevice, vk_data.pipelineLayout, nullptr);
    }

    static void bindPipeline(const engine::vk_data_t& vk_data, const VkCommandBuffer& commandBuffer)
    {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_data.graphicsPipeline);
    }
}