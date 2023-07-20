#pragma once

#include <vulkan/vulkan.h>

#include "vk_data.hpp"

namespace vk
{

    static void setupRasterizer(VulkanRenderPipeline &renderPipeline, engine::RenderPassInfo& renderPassInfo)
    {

        renderPipeline.rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        renderPipeline.rasterizer.depthClampEnable = VK_FALSE;
        renderPipeline.rasterizer.rasterizerDiscardEnable = VK_FALSE;
        renderPipeline.rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        renderPipeline.rasterizer.lineWidth = 1.0f;
        renderPipeline.rasterizer.cullMode = renderPipeline.cullMode;
        renderPipeline.rasterizer.frontFace = renderPipeline.winding;
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
        renderPipeline.colorBlendAttachments.resize(renderPipeline.numAttachments);
        for (int i = 0; i < renderPipeline.numAttachments; i++)
        {
            renderPipeline.colorBlendAttachments[i].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            renderPipeline.colorBlendAttachments[i].blendEnable = renderPassInfo.attachments[i].blendEnable ? VK_TRUE : VK_FALSE;
            renderPipeline.colorBlendAttachments[i].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
            renderPipeline.colorBlendAttachments[i].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            renderPipeline.colorBlendAttachments[i].colorBlendOp = VK_BLEND_OP_ADD;
            renderPipeline.colorBlendAttachments[i].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            renderPipeline.colorBlendAttachments[i].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
            renderPipeline.colorBlendAttachments[i].alphaBlendOp = VK_BLEND_OP_ADD;
        }

        renderPipeline.colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        renderPipeline.colorBlending.logicOpEnable = VK_FALSE;
        renderPipeline.colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
        renderPipeline.colorBlending.attachmentCount = renderPipeline.numAttachments;

        renderPipeline.colorBlending.pAttachments = renderPipeline.colorBlendAttachments.data();
        renderPipeline.colorBlending.blendConstants[0] = 0.0f; // Optional
        renderPipeline.colorBlending.blendConstants[1] = 0.0f; // Optional
        renderPipeline.colorBlending.blendConstants[2] = 0.0f; // Optional
        renderPipeline.colorBlending.blendConstants[3] = 0.0f; // Optional
    }
}