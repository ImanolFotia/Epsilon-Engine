#pragma once

#include <vulkan/vulkan.hpp>

#include "swap_chain.hpp"
#include <engine/renderers/vk_data.hpp>

namespace vk
{
    
    static void createViewport(engine::VulkanData& vk_data, engine::VulkanRenderPipeline& renderPipeline)
    {
        renderPipeline.viewport.x = 0.0f;
        renderPipeline.viewport.y = 0.0f;
        renderPipeline.viewport.width = (float)vk_data.swapChainExtent.width;
        renderPipeline.viewport.height = (float)vk_data.swapChainExtent.height;
        renderPipeline.viewport.minDepth = 0.0f;
        renderPipeline.viewport.maxDepth = 1.0f;

        renderPipeline.scissor.offset = {0, 0};
        renderPipeline.scissor.extent = vk_data.swapChainExtent;

        renderPipeline.viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        renderPipeline.viewportState.viewportCount = 1;
        renderPipeline.viewportState.pViewports = &renderPipeline.viewport;
        renderPipeline.viewportState.scissorCount = 1;
        renderPipeline.viewportState.pScissors = &renderPipeline.scissor;

    }
}