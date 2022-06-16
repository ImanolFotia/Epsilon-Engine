#pragma once

#include <vulkan/vulkan.hpp>

#include "device.hpp"
#include "swap_chain.hpp"
#include "vk_data.hpp"
#include "texture.hpp"

namespace vk
{
    struct render_pass_data_t
    {
        VkAttachmentDescription colorAttachment{};
        VkAttachmentReference colorAttachmentRef{};
        VkSubpassDescription subpass{};
        VkSubpassDependency dependency{};

        VkAttachmentDescription depthAttachment{};
        VkAttachmentReference depthAttachmentRef{};
    };

    static void createRenderPass(VulkanData &vk_data, VulkanRenderPass &renderPass, engine::RenderPassInfo &renderPassInfo)
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

        for (auto &attachment : renderPassInfo.attachments)
        {
           /* if (attachment.format == engine::DEPTH_F32_STENCIL_8 ||
                attachment.format == engine::DEPTH_F16_STENCIL_8 ||
                attachment.format == engine::DEPTH_F32 ||
                attachment.format == engine::DEPTH_F16 ||
                attachment.format == engine::DEPTH_UNORM ||
                attachment.format == engine::DEPTH_UNORM_STENCIL_8)*/
            {
                render_pass_data.depthAttachment.format = findDepthFormat(vk_data);
            }
            render_pass_data.depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
            render_pass_data.depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            render_pass_data.depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            render_pass_data.depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            render_pass_data.depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            render_pass_data.depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            render_pass_data.depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            render_pass_data.depthAttachmentRef.attachment = 1;
            render_pass_data.depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        }

        render_pass_data.subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        render_pass_data.subpass.colorAttachmentCount = 1;
        render_pass_data.subpass.pColorAttachments = &render_pass_data.colorAttachmentRef;
        render_pass_data.subpass.pDepthStencilAttachment = &render_pass_data.depthAttachmentRef;

        render_pass_data.dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        render_pass_data.dependency.dstSubpass = 0;
        render_pass_data.dependency.srcAccessMask = 0;
        
        //render_pass_data.dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        render_pass_data.dependency.srcAccessMask = 0;
        //render_pass_data.dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        //render_pass_data.dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        render_pass_data.dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        render_pass_data.dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        render_pass_data.dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        std::array<VkAttachmentDescription, 2> attachments = {render_pass_data.colorAttachment, render_pass_data.depthAttachment};
        VkRenderPassCreateInfo renderPassCreateInfo{};
        renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassCreateInfo.pAttachments = attachments.data();
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &render_pass_data.subpass;
        renderPassCreateInfo.dependencyCount = 1;
        renderPassCreateInfo.pDependencies = &render_pass_data.dependency;

        if (vkCreateRenderPass(vk_data.logicalDevice, &renderPassCreateInfo, nullptr, &renderPass.renderPass) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create render pass!");
        }
    }

    static void createRenderPassInfo(uint32_t imageIndex, VulkanData &vk_data, VulkanRenderPass &renderPass)
    {
        renderPass.renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPass.renderPassInfo.renderPass = renderPass.renderPass;
        renderPass.renderPassInfo.framebuffer = vk_data.swapChainFramebuffers[imageIndex];
        renderPass.renderPassInfo.renderArea.offset = {0, 0};
        renderPass.renderPassInfo.renderArea.extent = vk_data.swapChainExtent;

        renderPass.renderPassInfo.clearValueCount = 2;
        renderPass.renderPassInfo.pClearValues = renderPass.clearValues;
    }

    static void beginRenderPass(const VkCommandBuffer &commandBuffer, VulkanRenderPass &renderPass)
    {
        vkCmdBeginRenderPass(commandBuffer, &renderPass.renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    static void endRenderPass(const VkCommandBuffer &commandBuffer, const VulkanData &vk_data)
    {
        vkCmdEndRenderPass(commandBuffer);
    }

    static void cleanupRenderPass(const VulkanData &vk_data, VkRenderPass &renderPass)
    {

        vkDestroyRenderPass(vk_data.logicalDevice, renderPass, nullptr);
    }
}