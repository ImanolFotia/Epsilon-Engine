#pragma once

#include <vulkan/vulkan.hpp>

#include "device.hpp"
#include "render_pass.hpp"

namespace vk
{

    static void createFramebuffers(VulkanData& vk_data, VulkanRenderPass& renderPass, RenderPassChain& chain)
    {
        chain.Framebuffers.resize(chain.ImageViews.size());

        for (size_t i = 0; i < chain.ImageViews.size(); i++)
        {
            VkImageView attachments[] = {
                    chain.ImageViews[i],
                    chain.DepthTexture.imageView};

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderPass.renderPass;
            framebufferInfo.attachmentCount = 2;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = chain.Extent.width;
            framebufferInfo.height = chain.Extent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(vk_data.logicalDevice, &framebufferInfo, nullptr, &chain.Framebuffers[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }
}