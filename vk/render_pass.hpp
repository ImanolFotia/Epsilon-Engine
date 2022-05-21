#pragma once

#include <vulkan/vulkan.hpp>

#include "device.hpp"
#include "swap_chain.hpp"

namespace vk
{
    struct render_pass_data_t
    {
        VkAttachmentDescription colorAttachment{};
        VkAttachmentReference colorAttachmentRef{};
        VkSubpassDescription subpass{};
        VkSubpassDependency dependency{};
    };



    static void createRenderPass(engine::vk_data_t& vk_data)
    {
        render_pass_data_t render_pass_data{};
        render_pass_data.colorAttachment.format = vk_data.swapChainImageFormat;
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

        render_pass_data.dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        render_pass_data.dependency.dstSubpass = 0;
        render_pass_data.dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        render_pass_data.dependency.srcAccessMask = 0;
        render_pass_data.dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        render_pass_data.dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo renderPassCreateInfo{};
        renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCreateInfo.attachmentCount = 1;
        renderPassCreateInfo.pAttachments = &render_pass_data.colorAttachment;
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &render_pass_data.subpass;
        renderPassCreateInfo.dependencyCount = 1;
        renderPassCreateInfo.pDependencies = &render_pass_data.dependency;

        if (vkCreateRenderPass(vk_data.logicalDevice, &renderPassCreateInfo, nullptr, &vk_data.renderPass) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create render pass!");
        }
    }

    static void createRenderPassInfo(uint32_t imageIndex, engine::vk_data_t& vk_data)
    {
        vk_data.renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        vk_data.renderPassInfo.renderPass = vk_data.renderPass;
        vk_data.renderPassInfo.framebuffer = vk_data.swapChainFramebuffers[imageIndex];
        vk_data.renderPassInfo.renderArea.offset = {0, 0};
        vk_data.renderPassInfo.renderArea.extent = vk_data.swapChainExtent;

        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        vk_data.renderPassInfo.clearValueCount = 1;
        vk_data.renderPassInfo.pClearValues = &clearColor;
    }

    static void beginRenderPass(const VkCommandBuffer &commandBuffer, const engine::vk_data_t& vk_data)
    {
        vkCmdBeginRenderPass(commandBuffer, &vk_data.renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    static void endRenderPass(const VkCommandBuffer &commandBuffer, const engine::vk_data_t& vk_data)
    {
        vkCmdEndRenderPass(commandBuffer);
    }
}