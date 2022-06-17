#pragma once

#include <vulkan/vulkan.hpp>

#include "shader.hpp"
#include "swap_chain.hpp"
#include "viewport.hpp"
#include "rasterizer.hpp"
#include "render_pass.hpp"
#include <engine/renderer/types.hpp>

#include "vk_data.hpp"

namespace vk
{

    // std::pair<>

    template <uint32_t num_stages>
    static std::array<VkPipelineShaderStageCreateInfo, num_stages> createShaderStages(const char *vertexPath,
                                                                                      const char *fragmentPath,
                                                                                      VkShaderModule &vertShaderModule,
                                                                                      VkShaderModule &fragShaderModule,
                                                                                      VulkanData &vk_data)
    {

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

    static VkPipelineShaderStageCreateInfo createShaderStage(VulkanData &vk_data, engine::ShaderStageInfo info)
    {

        VkPipelineShaderStageCreateInfo shaderStageInfo{};

        shaderStageInfo.pName = info.entryPoint;
        shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

        if (info.stage == engine::ShaderModuleStage::FRAGMENT)
        {
            shaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        }
        else if (info.stage == engine::ShaderModuleStage::VERTEX)
        {
            shaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        }
        else if (info.stage == engine::ShaderModuleStage::COMPUTE)
        {
            shaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        }
        else
        {
        }

        shaderStageInfo.module = shader::createShaderModule(info.shaderCode, vk_data);

        return shaderStageInfo;
    }

    template <class PushConstantType>
    static VkPushConstantRange setupPushConstant()
    {
        VkPushConstantRange push_constant;
        push_constant.offset = 0;
        push_constant.size = sizeof(PushConstantType);
        push_constant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        return push_constant;
    }

    template <typename PushConstantType>
    static void createPipelineLayout(
        VulkanData &vk_data,
        VkPushConstantRange push_constant,
        VulkanRenderPipeline &renderPipeline)
    {
        // Pipeline layout
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1; // Optional
        // pipelineLayoutInfo.pSetLayouts = nullptr;         // Optional
        pipelineLayoutInfo.pSetLayouts = &renderPipeline.descriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 1;           // Optional
        pipelineLayoutInfo.pPushConstantRanges = &push_constant; // Optional

        if (vkCreatePipelineLayout(vk_data.logicalDevice, &pipelineLayoutInfo, nullptr, &renderPipeline.pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    template <typename PushConstantType, uint32_t C>
    static VkPipeline createGraphicsPipeline(
        VulkanData &vk_data,
        VulkanRenderPass &renderPass,
        VulkanRenderPipeline &renderPipeline,
        VulkanVertexInfo<C> VertexInfo,
        engine::ShaderInfo shaderInfo)
    {

        // VkShaderModule vertShaderModule;
        // VkShaderModule fragShaderModule;
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
        shaderStages.resize(shaderInfo.stages.size());

        for (unsigned i = 0; i < shaderInfo.stages.size(); i++)
        {
            std::cout << shaderInfo.stages[i].shaderCode.data() << std::endl;
            shaderStages[i] = createShaderStage(vk_data, shaderInfo.stages[i]);
        }

        // auto shaderStages = createShaderStages<2>("../assets/shaders/vertex.spv", "../assets/shaders/fragment.spv", vertShaderModule, fragShaderModule, vk_data);
        //   Vertex Stage
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = &VertexInfo.bindingDescription; // Optional
        vertexInputInfo.vertexAttributeDescriptionCount = VertexInfo.attributeDescriptions.size();
        vertexInputInfo.pVertexAttributeDescriptions = VertexInfo.attributeDescriptions.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        // Viewport stage
        createViewport(vk_data, renderPipeline);

        // Rasterizer stage
        setupRasterizer(renderPipeline);

        auto push_constant = setupPushConstant<PushConstantType>();

        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_LINE_WIDTH};

        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        // createPipelineLayout<PushConstantType>(vk_data, push_constant);

        // Pipeline layout
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1; // Optional
        // pipelineLayoutInfo.pSetLayouts = nullptr;         // Optional
        pipelineLayoutInfo.pSetLayouts = &renderPipeline.descriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 1;           // Optional
        pipelineLayoutInfo.pPushConstantRanges = &push_constant; // Optional

        if (vkCreatePipelineLayout(vk_data.logicalDevice, &pipelineLayoutInfo, nullptr, &renderPipeline.pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }

        // Creating the graphics pipeline
        VkGraphicsPipelineCreateInfo pipelineInfo = {};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = shaderStages.size();
        pipelineInfo.pStages = shaderStages.data();

        if (renderPass.renderPassData.hasDepthAttachment)
        {
            VkPipelineDepthStencilStateCreateInfo depthStencil{};
            depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            depthStencil.depthTestEnable = VK_TRUE;
            depthStencil.depthWriteEnable = VK_TRUE;
            depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
            depthStencil.depthBoundsTestEnable = VK_FALSE;
            depthStencil.minDepthBounds = 0.0f; // Optional
            depthStencil.maxDepthBounds = 1.0f; // Optional
            depthStencil.stencilTestEnable = VK_FALSE;
            depthStencil.front = {}; // Optional
            depthStencil.back = {};  // Optional
            pipelineInfo.pDepthStencilState = &depthStencil;
        }

        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &renderPipeline.viewportState;
        pipelineInfo.pRasterizationState = &renderPipeline.rasterizer;
        pipelineInfo.pMultisampleState = &renderPipeline.multisampling; // Optional
        pipelineInfo.pColorBlendState = &renderPipeline.colorBlending;

        pipelineInfo.layout = renderPipeline.pipelineLayout;

        pipelineInfo.renderPass = renderPass.renderPass;
        pipelineInfo.subpass = 0;
        // pipelineInfo.pDynamicState = &dynamicState;

        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
        if (auto res = vkCreateGraphicsPipelines(vk_data.logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &renderPipeline.graphicsPipeline); res != VK_SUCCESS)
        {
            std::cerr << "Result id: " << res << std::endl;
            throw std::runtime_error("failed to create graphics pipeline!");
        }
        for (auto &stage : shaderStages)
        {
            vkDestroyShaderModule(vk_data.logicalDevice, stage.module, nullptr);
        }

        return renderPipeline.graphicsPipeline;
    }

    static void draw(const VkCommandBuffer &commandBuffer,
                     uint32_t vertexCount,
                     uint32_t instanceCount,
                     uint32_t firstVertex,
                     uint32_t firstInstance)
    {
        vkCmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
    }

    static void drawIndexed(const VkCommandBuffer &commandBuffer,
                            uint32_t indexCount,
                            uint32_t instanceCount,
                            uint32_t firstIndex,
                            int32_t vertexOffset,
                            uint32_t firstInstance)
    {
        vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    }

    static void destroyGraphicsPipeline(const VulkanData &vk_data, VulkanRenderPipeline &renderPipeline)
    {

        vkDestroyPipelineLayout(vk_data.logicalDevice, renderPipeline.pipelineLayout, nullptr);
        vkDestroyDescriptorSetLayout(vk_data.logicalDevice, renderPipeline.descriptorSetLayout, nullptr);
        vkDestroyPipeline(vk_data.logicalDevice, renderPipeline.graphicsPipeline, nullptr);
    }

    static void bindPipeline(const VulkanRenderPipeline &renderPipeline, const VkCommandBuffer &commandBuffer)
    {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, renderPipeline.graphicsPipeline);
    }
}