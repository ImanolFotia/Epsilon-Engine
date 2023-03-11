#pragma once

#if (BUILD_ANDROID == 0)
#include <vulkan/vulkan.hpp>
#endif

#include "device.hpp"
#include "render_pass.hpp"

namespace vk
{
    static void createSwapChainFramebuffers(VulkanData &vk_data, VulkanRenderPass &renderPass, RenderPassChain &chain)
    {
        renderPass.renderPassChain.Framebuffers.clear();
        renderPass.renderPassChain.Framebuffers.resize(renderPass.renderPassChain.ImageViews.size());

        for (size_t i = 0; i < renderPass.renderPassChain.ImageViews.size(); i++)
        {
            // std::vector<VkImageView> attachments;

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
    static void createFramebuffers(VulkanData &vk_data, VulkanRenderPass &renderPass, RenderPassChain &chain)
    {
        renderPass.renderPassChain.Framebuffers.resize(1);

        int numAttachments = renderPass.renderPassChain.ImageFormats.size();
        int numLayers = renderPass.renderPassChain.ImageViews.size();

        std::vector<VkImageView> attachments = renderPass.renderPassChain.ImageViews;
        attachments.push_back(renderPass.renderPassChain.DepthTexture.imageView);

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass.renderPass;
        framebufferInfo.attachmentCount = numAttachments;
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = renderPass.renderPassChain.Extent.width;
        framebufferInfo.height = renderPass.renderPassChain.Extent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(vk_data.logicalDevice, &framebufferInfo, nullptr, &renderPass.renderPassChain.Framebuffers[0]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}