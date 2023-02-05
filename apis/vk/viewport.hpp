#pragma once

#if !defined(ANDROID) || !defined(__ANDROID__)
#include <vulkan/vulkan.hpp>
#endif

#include "swap_chain.hpp"
#include "vk_data.hpp"

namespace vk
{
    
    static void createViewport(VulkanData& vk_data, VulkanRenderPipeline& renderPipeline, const RenderPassChain& chain)
    {
        renderPipeline.viewport.x = 0.0f;
        renderPipeline.viewport.y = 0.0f;
        renderPipeline.viewport.width = (float)chain.Extent.width;
        renderPipeline.viewport.height = (float)chain.Extent.height;
        renderPipeline.viewport.minDepth = 0.0f;
        renderPipeline.viewport.maxDepth = 1.0f;

        renderPipeline.scissor.offset = {0, 0};
        renderPipeline.scissor.extent = chain.Extent;

        renderPipeline.viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        renderPipeline.viewportState.viewportCount = 1;
        renderPipeline.viewportState.pViewports = &renderPipeline.viewport;
        renderPipeline.viewportState.scissorCount = 1;
        renderPipeline.viewportState.pScissors = &renderPipeline.scissor;

    }
}