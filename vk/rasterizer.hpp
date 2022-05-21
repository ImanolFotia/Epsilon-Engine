#pragma once

#include <vulkan/vulkan.hpp>

#include "../engine/renderers/vk_data.hpp"

namespace vk
{

    static void setupRasterizer(engine::vk_data_t& vk_data)
    {
        vk_data.rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        vk_data.rasterizer.depthClampEnable = VK_FALSE;
        vk_data.rasterizer.rasterizerDiscardEnable = VK_FALSE;
        vk_data.rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        vk_data.rasterizer.lineWidth = 1.0f;
        vk_data.rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        vk_data.rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        vk_data.rasterizer.depthBiasEnable = VK_FALSE;
        vk_data.rasterizer.depthBiasConstantFactor = 0.0f; // Optional
        vk_data.rasterizer.depthBiasClamp = 0.0f;          // Optional
        vk_data.rasterizer.depthBiasSlopeFactor = 0.0f;    // Optional

        vk_data.multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        vk_data.multisampling.sampleShadingEnable = VK_FALSE;
        vk_data.multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        vk_data.multisampling.minSampleShading = 1.0f;          // Optional
        vk_data.multisampling.pSampleMask = nullptr;            // Optional
        vk_data.multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
        vk_data.multisampling.alphaToOneEnable = VK_FALSE;      // Optional

        vk_data.colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        vk_data.colorBlendAttachment.blendEnable = VK_TRUE;
        vk_data.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        vk_data.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        vk_data.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        vk_data.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        vk_data.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        vk_data.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        vk_data.colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        vk_data.colorBlending.logicOpEnable = VK_FALSE;
        vk_data.colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
        vk_data.colorBlending.attachmentCount = 1;
        vk_data.colorBlending.pAttachments = &vk_data.colorBlendAttachment;
        vk_data.colorBlending.blendConstants[0] = 0.0f; // Optional
        vk_data.colorBlending.blendConstants[1] = 0.0f; // Optional
        vk_data.colorBlending.blendConstants[2] = 0.0f; // Optional
        vk_data.colorBlending.blendConstants[3] = 0.0f; // Optional
    }
}