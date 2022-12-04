#pragma once

#include <vulkan/vulkan.hpp>

#include "vk_data.hpp"

namespace vk
{

    static void setupRasterizer(VulkanRenderPipeline& renderPipeline)
    {
        renderPipeline.rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        renderPipeline.rasterizer.depthClampEnable = VK_FALSE;
        renderPipeline.rasterizer.rasterizerDiscardEnable = VK_FALSE;
        renderPipeline.rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        renderPipeline.rasterizer.lineWidth = 1.0f;
        renderPipeline.rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        renderPipeline.rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        renderPipeline.rasterizer.depthBiasEnable = VK_FALSE;
        renderPipeline.rasterizer.depthBiasConstantFactor = 0.0f; // Optional
        renderPipeline.rasterizer.depthBiasClamp = 0.0f;          // Optional
        renderPipeline.rasterizer.depthBiasSlopeFactor = 0.0f;    // Optional

        renderPipeline.multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        renderPipeline.multisampling.sampleShadingEnable = VK_FALSE;
        renderPipeline.multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        renderPipeline.multisampling.minSampleShading = 1.0f;          // Optional
        renderPipeline.multisampling.pSampleMask = nullptr;            // Optional
        renderPipeline.multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
        renderPipeline.multisampling.alphaToOneEnable = VK_FALSE;      // Optional

        renderPipeline.colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        renderPipeline.colorBlendAttachment.blendEnable = VK_FALSE;
        renderPipeline.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        renderPipeline.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        renderPipeline.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        renderPipeline.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        renderPipeline.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        renderPipeline.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        renderPipeline.colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        renderPipeline.colorBlending.logicOpEnable = VK_FALSE;
        renderPipeline.colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
        renderPipeline.colorBlending.attachmentCount = 1;
        renderPipeline.colorBlending.pAttachments = &renderPipeline.colorBlendAttachment;
        renderPipeline.colorBlending.blendConstants[0] = 0.0f; // Optional
        renderPipeline.colorBlending.blendConstants[1] = 0.0f; // Optional
        renderPipeline.colorBlending.blendConstants[2] = 0.0f; // Optional
        renderPipeline.colorBlending.blendConstants[3] = 0.0f; // Optional
    }
}