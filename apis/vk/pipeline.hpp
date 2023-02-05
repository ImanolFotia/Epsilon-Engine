#pragma once

#if !defined(ANDROID) || !defined(__ANDROID__)
#include <vulkan/vulkan.hpp>
#endif

#include "shader.hpp"
#include "swap_chain.hpp"
#include "viewport.hpp"
#include "rasterizer.hpp"
#include "render_pass.hpp"
#include "vertex_buffer.hpp"
#include "core/engine/renderer/types.hpp"
#include <functional>

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

    static VkPushConstantRange setupPushConstant(size_t size)
    {
        VkPushConstantRange push_constant;
        push_constant.offset = 0;
        push_constant.size = size;
        push_constant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        return push_constant;
    }

    static void createPipelineLayout(
        VulkanData &vk_data,
        VkPushConstantRange push_constant,
        VkPipelineLayout &pipelineLayout,
        VkDescriptorSetLayout& descriptorSetLayout)
    {
        // Pipeline layout
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1; // Optional
        // pipelineLayoutInfo.pSetLayouts = nullptr;         // Optional
        pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
        if(push_constant.size > 0) {
            pipelineLayoutInfo.pushConstantRangeCount = 1;           // Optional
            pipelineLayoutInfo.pPushConstantRanges = &push_constant; // Optional
        }

        if (vkCreatePipelineLayout(vk_data.logicalDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }


    static void createGraphicsPipeline(
        VulkanData &vk_data,
        VulkanRenderPass &renderPass,
        VulkanRenderPipeline &renderPipeline,
        engine::RenderPassInfo& renderPassInfo)
    {
        std::vector<std::function<void(VulkanData&)>> destroyShaderStages;

        std::vector<VkGraphicsPipelineCreateInfo> pipelinesInfo;
        std::vector<std::vector<VkPipelineShaderStageCreateInfo>> shaderStages;


        std::vector<vk::VulkanVertexInfo> vertexInfos;
        for(int layout_index = 0; layout_index < renderPassInfo.numLayouts; layout_index++) {

            auto& shaderInfo = renderPassInfo.pipelineLayout[layout_index].shaderInfo;
            shaderStages.emplace_back();
            shaderStages.back().resize(renderPassInfo.pipelineLayout[layout_index].shaderInfo.stages.size());

            for (unsigned i = 0; i < shaderInfo.stages.size(); i++) {
                std::cout << shaderInfo.stages[i].shaderCode.data() << std::endl;
                shaderStages[layout_index][i] = createShaderStage(vk_data, shaderInfo.stages[i]);

                destroyShaderStages.emplace_back([stage = shaderStages[layout_index][i]](VulkanData &vk_data) {
                    vkDestroyShaderModule(vk_data.logicalDevice, stage.module, nullptr);
                });
            }

            // auto shaderStages = createShaderStages<2>("../assets/shaders/vertex.spv", "../assets/shaders/fragment.spv", vertShaderModule, fragShaderModule, vk_data);
            //   Vertex Stage


            if(renderPassInfo.pipelineLayout[layout_index].cullMode == engine::CullMode::BACK)
                renderPipeline.cullMode = VK_CULL_MODE_BACK_BIT;
            else if(renderPassInfo.pipelineLayout[layout_index].cullMode == engine::CullMode::FRONT)
                renderPipeline.cullMode = VK_CULL_MODE_FRONT_BIT;
            else
                renderPipeline.cullMode = VK_CULL_MODE_NONE;

            if(renderPassInfo.pipelineLayout[layout_index].windingMode == engine::WindingMode::COUNTER_CLOCK_WISE)
                renderPipeline.winding = VK_FRONT_FACE_COUNTER_CLOCKWISE;
            else
                renderPipeline.winding = VK_FRONT_FACE_CLOCKWISE;

            vk::VulkanVertexInfo& vertexInfo = vertexInfos.emplace_back();

            vertexInfo.attributeDescriptions =
                    getAttributeDescriptions(0, renderPassInfo.pipelineLayout[layout_index].vertexLayout);

            vertexInfo.bindingDescription = getBindingDescription(renderPassInfo.size);

            VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
            vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vertexInputInfo.vertexBindingDescriptionCount = 1;
            vertexInputInfo.pVertexBindingDescriptions = &vertexInfo.bindingDescription; // Optional
            vertexInputInfo.vertexAttributeDescriptionCount = vertexInfo.attributeDescriptions.size();
            vertexInputInfo.pVertexAttributeDescriptions = vertexInfo.attributeDescriptions.data();

            VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
            inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            inputAssembly.primitiveRestartEnable = VK_FALSE;

            // Viewport stage
            createViewport(vk_data, renderPipeline, renderPass.renderPassChain);

            // Rasterizer stage
            setupRasterizer(renderPipeline);

            auto push_constant = setupPushConstant(renderPassInfo.pushConstant.size);

            std::vector<VkDynamicState> dynamicStates = {
                    VK_DYNAMIC_STATE_VIEWPORT,
                    VK_DYNAMIC_STATE_LINE_WIDTH};

            VkPipelineDynamicStateCreateInfo dynamicState{};
            dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
            dynamicState.pDynamicStates = dynamicStates.data();


            renderPipeline.pipelineLayout.emplace_back();
            createPipelineLayout(vk_data, push_constant, renderPipeline.pipelineLayout.back(), renderPipeline.descriptorSetLayout);

            // Creating the graphics pipeline
            VkGraphicsPipelineCreateInfo& pipelineInfo = pipelinesInfo.emplace_back();

            pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            pipelineInfo.stageCount = shaderStages[layout_index].size();
            pipelineInfo.pStages = shaderStages[layout_index].data();

            VkPipelineDepthStencilStateCreateInfo depthStencil{};
            if (renderPass.renderPassData.hasDepthAttachment) {
                depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
                depthStencil.depthTestEnable = VK_TRUE;
                depthStencil.depthWriteEnable = VK_TRUE;
                depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
                depthStencil.depthBoundsTestEnable = VK_FALSE;
                depthStencil.minDepthBounds = 0.0f; // Optional
                depthStencil.maxDepthBounds = 1.0f; // Optional
                depthStencil.stencilTestEnable = VK_FALSE;
                depthStencil.pNext = NULL;
                depthStencil.flags = 0;
                pipelineInfo.pDepthStencilState = &depthStencil;
            }

            pipelineInfo.pVertexInputState = &vertexInputInfo;
            pipelineInfo.pInputAssemblyState = &inputAssembly;
            pipelineInfo.pViewportState = &renderPipeline.viewportState;
            pipelineInfo.pRasterizationState = &renderPipeline.rasterizer;
            pipelineInfo.pMultisampleState = &renderPipeline.multisampling; // Optional
            pipelineInfo.pColorBlendState = &renderPipeline.colorBlending;

            pipelineInfo.layout = renderPipeline.pipelineLayout.back();

            pipelineInfo.renderPass = renderPass.renderPass;
            pipelineInfo.subpass = 0;
            pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        }
        // pipelineInfo.pDynamicState = &dynamicState;

        renderPipeline.graphicsPipeline.resize(pipelinesInfo.size());
        if (auto res = vkCreateGraphicsPipelines(vk_data.logicalDevice, VK_NULL_HANDLE, pipelinesInfo.size(), pipelinesInfo.data(), nullptr, renderPipeline.graphicsPipeline.data()); res != VK_SUCCESS)
        {
            std::cerr << "Result id: " << res << std::endl;
            throw std::runtime_error("failed to create graphics pipeline!");
        }


        renderPass.vertexInfo = vertexInfos;

        for(auto& func: destroyShaderStages) {
            func(vk_data);
        }

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
        for(auto& layout: renderPipeline.pipelineLayout)
            vkDestroyPipelineLayout(vk_data.logicalDevice, layout, nullptr);

        vkDestroyDescriptorSetLayout(vk_data.logicalDevice, renderPipeline.descriptorSetLayout, nullptr);

        for(auto& pipeline: renderPipeline.graphicsPipeline)
            vkDestroyPipeline(vk_data.logicalDevice, pipeline, nullptr);
    }

    static void bindPipeline(const VkPipeline &renderPipeline, const VkCommandBuffer &commandBuffer)
    {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, renderPipeline);
    }
}