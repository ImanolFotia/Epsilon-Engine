#pragma once

#if !defined(ANDROID) || !defined(__ANDROID__)
#include <vulkan/vulkan.hpp>
#endif

#include "device.hpp"
#include "render_pass.hpp"

namespace vk
{

    static void createFramebuffers(VulkanData& vk_data, VulkanRenderPass& renderPass, RenderPassChain& chain)
    {
        renderPass.renderPassChain.Framebuffers.resize(renderPass.renderPassChain.ImageViews.size());

        for (size_t i = 0; i < renderPass.renderPassChain.ImageViews.size(); i++)
        {
            VkImageView attachments[] = {
                    renderPass.renderPassChain.ImageViews[i],
                    renderPass.renderPassChain.DepthTexture.imageView};

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderPass.renderPass;
            framebufferInfo.attachmentCount = 2;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = renderPass.renderPassChain.Extent.width;
            framebufferInfo.height = renderPass.renderPassChain.Extent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(vk_data.logicalDevice, &framebufferInfo, nullptr, &renderPass.renderPassChain.Framebuffers[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }
}