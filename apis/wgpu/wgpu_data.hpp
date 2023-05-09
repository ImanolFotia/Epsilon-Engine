#pragma once

#include <webgpu/webgpu.h>

struct WebGPURenderPipeline
{
    std::vector<WGPUBindGroupLayout> descriptorSetLayouts;
    std::vector<WGPUPipelineLayout> pipelineLayout{};
    WGPURenderPipeline graphicsPipeline;
    VkPipelineViewportStateCreateInfo viewportState{};
    VkRect2D scissor{};
    VkViewport viewport{};
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    VkPipelineMultisampleStateCreateInfo multisampling{};
    std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
    WGPUBlendState colorBlending{};
    VkFrontFace winding;
    VkCullModeFlags cullMode;

    // VkClearColorValue clearColor = {0.1f, 0.1f, 0.1f, 1.0f};
    VkClearColorValue clearColor = {1.0f, 1.0f, 1.0f, 1.0f};
    VkClearDepthStencilValue depthStencilClearColor = {1.0f, 0};
    uint32_t numAttachments = 1;
};

struct WebGPUData
{
    WGPUSwapChain swapchain;
    WGPUInstance instance;
    WGPUQueue queue;
};