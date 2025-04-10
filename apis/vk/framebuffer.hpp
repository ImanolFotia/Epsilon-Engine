#pragma once

#include "device.hpp"
#include "render_pass.hpp"

namespace vk {
static void createSwapChainFramebuffers(VulkanData &vk_data, VulkanRenderPass &renderPass, RenderPassChain &chain) {
  renderPass.renderPassChain.Framebuffers.clear();
  renderPass.renderPassChain.Framebuffers.resize(renderPass.renderPassChain.ImageViews.size());

  for (std::size_t i = 0; i < renderPass.renderPassChain.ImageViews.size(); i++) {
    // std::vector<VkImageView> attachments;

    std::vector<VkImageView> attachments = {renderPass.renderPassChain.ImageViews[i], renderPass.renderPassChain.DepthTexture.imageView};

    if (renderPass.multisampled) {
      attachments.push_back(chain.ResolveTexture.imageView);
    }

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass.renderPass;
    framebufferInfo.attachmentCount = attachments.size();
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = renderPass.renderPassChain.Extent.width;
    framebufferInfo.height = renderPass.renderPassChain.Extent.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(vk_data.logicalDevice, &framebufferInfo, nullptr, &renderPass.renderPassChain.Framebuffers[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create framebuffer!");
    }
  }
}
static void createFramebuffers(VulkanData &vk_data, VulkanRenderPass &renderPass, RenderPassChain &chain) {
  renderPass.renderPassChain.Framebuffers.resize(1);

  int numAttachments = renderPass.renderPassChain.ImageFormats.size();
  int numLayers = renderPass.renderPassChain.ImageViews.size();

  (void)numLayers;

  std::vector<VkImageView> attachments = renderPass.renderPassChain.ImageViews;

  if (!renderPass.renderPassChain.hasDepthSampler)
    attachments.push_back(renderPass.renderPassChain.DepthTexture.imageView);

  VkFramebufferCreateInfo framebufferInfo{};
  framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  framebufferInfo.renderPass = renderPass.renderPass;
  framebufferInfo.attachmentCount = numAttachments;
  framebufferInfo.pAttachments = attachments.data();
  framebufferInfo.width = renderPass.renderPassChain.Extent.width;
  framebufferInfo.height = renderPass.renderPassChain.Extent.height;
  framebufferInfo.layers = 1;

  if (vkCreateFramebuffer(vk_data.logicalDevice, &framebufferInfo, nullptr, &renderPass.renderPassChain.Framebuffers[0]) != VK_SUCCESS) {
    throw std::runtime_error("failed to create framebuffer!");
  }
}
} // namespace vk