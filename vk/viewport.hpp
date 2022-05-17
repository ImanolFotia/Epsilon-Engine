#pragma once

#include <vulkan/vulkan.hpp>

#include "swap_chain.hpp"

namespace vk
{
    
    VkPipelineViewportStateCreateInfo viewportState{};
        VkRect2D scissor{};
        VkViewport viewport{};
    void createViewport()
    {
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)vk::swapChainExtent.width;
        viewport.height = (float)vk::swapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        scissor.offset = {0, 0};
        scissor.extent = vk::swapChainExtent;

        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

    }
}