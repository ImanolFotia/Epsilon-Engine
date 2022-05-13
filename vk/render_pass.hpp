#pragma once

#include <vulkan/vulkan.h>

#include "device.hpp"
#include "swap_chain.hpp"

namespace vk
{
    struct render_pass_data_t
    {
        VkAttachmentDescription colorAttachment{};
        VkAttachmentReference colorAttachmentRef{};
        VkSubpassDescription subpass{};
    };

    VkRenderPass createRenderPass(VkDevice device)
    {
        render_pass_data_t render_pass_data{};
        render_pass_data.colorAttachment.format = swapChainImageFormat;
        render_pass_data.colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        render_pass_data.colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        render_pass_data.colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        render_pass_data.colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        render_pass_data.colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        render_pass_data.colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        render_pass_data.colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        render_pass_data.colorAttachmentRef.attachment = 0;
        render_pass_data.colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        render_pass_data.subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        render_pass_data.subpass.colorAttachmentCount = 1;
        render_pass_data.subpass.pColorAttachments = &render_pass_data.colorAttachmentRef;

        VkRenderPass renderPass;
        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &render_pass_data.colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &render_pass_data.subpass;

        if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create render pass!");
        }

        return renderPass;
    }

    VkRenderPassBeginInfo createRenderPassInfo(const VkRenderPass &renderPass, uint32_t imageIndex)
    {
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapChainExtent;

        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        return renderPassInfo;
    }

    void beginRenderPass(const VkCommandBuffer& commandBuffer, const VkRenderPassBeginInfo& renderPassInfo) {
        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    }
}