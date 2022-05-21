#pragma once

#include <vulkan/vulkan.hpp>

#include "device.hpp"
#include "render_pass.hpp"

namespace vk
{

    static void createFramebuffers(engine::VulkanData& vk_data, engine::VulkanRenderPipeline& renderPipeline)
    {
        /*swapChainFramebuffers.resize(swapChainImageViews.size());

        for (size_t i = 0; i < swapChainImageViews.size(); i++)
        {
            VkImageView attachments[] = {
                swapChainImageViews[i]};

            createFramebuffer(device, &swapChainFramebuffers[i], attachments);
        }
        */

        vk_data.swapChainFramebuffers.resize(vk_data.swapChainImageViews.size());

        for (size_t i = 0; i < vk_data.swapChainImageViews.size(); i++)
        {
            VkImageView attachments[] = {
                vk_data.swapChainImageViews[i]};

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderPipeline.renderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = vk_data.swapChainExtent.width;
            framebufferInfo.height = vk_data.swapChainExtent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(vk_data.logicalDevice, &framebufferInfo, nullptr, &vk_data.swapChainFramebuffers[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }
}