#pragma once

#include <vulkan/vulkan.hpp>

#include "swap_chain.hpp"

namespace vk
{
    
    static void createViewport(engine::vk_data_t& vk_data)
    {
        vk_data.viewport.x = 0.0f;
        vk_data.viewport.y = 0.0f;
        vk_data.viewport.width = (float)vk_data.swapChainExtent.width;
        vk_data.viewport.height = (float)vk_data.swapChainExtent.height;
        vk_data.viewport.minDepth = 0.0f;
        vk_data.viewport.maxDepth = 1.0f;

        vk_data.scissor.offset = {0, 0};
        vk_data.scissor.extent = vk_data.swapChainExtent;

        vk_data.viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        vk_data.viewportState.viewportCount = 1;
        vk_data.viewportState.pViewports = &vk_data.viewport;
        vk_data.viewportState.scissorCount = 1;
        vk_data.viewportState.pScissors = &vk_data.scissor;

    }
}