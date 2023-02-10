#pragma once


#if (BUILD_ANDROID == 0)
#include <vulkan/vulkan.hpp>
#endif

#include "device.hpp"
#include "swap_chain.hpp"
#include "vk_data.hpp"
#include "texture.hpp"

#include "core/engine/renderer/types.hpp"

namespace vk
{

    static void createRenderPass(VulkanData &vk_data, VulkanRenderPass &renderPass, engine::RenderPassInfo &renderPassInfo, bool forPresent)
    {
        bool containsDepthAttachment = false;
        renderPass.renderPassData.colorAttachments.clear();
        renderPass.renderPassData.colorAttachments.resize(0);

        renderPass.renderPassData.colorAttachmentRefs.clear();
        renderPass.renderPassData.colorAttachmentRefs.resize(0);

        unsigned index = 0;
        for (auto &attachment : renderPassInfo.attachments)
        {
            if (attachment.isDepthAttachment)
            {
                containsDepthAttachment = true;
                if (attachment.format == engine::DEPTH_F32_STENCIL_8 ||
                    attachment.format == engine::DEPTH_F16_STENCIL_8 ||
                    attachment.format == engine::DEPTH_F32 ||
                    attachment.format == engine::DEPTH_F16 ||
                    attachment.format == engine::DEPTH_UNORM ||
                    attachment.format == engine::DEPTH_UNORM_STENCIL_8)
                {
                    renderPass.renderPassData.depthAttachment.format = findDepthFormat(vk_data);
                }
                renderPass.renderPassData.depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
                renderPass.renderPassData.depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                renderPass.renderPassData.depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                renderPass.renderPassData.depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                renderPass.renderPassData.depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                renderPass.renderPassData.depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                renderPass.renderPassData.depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

                renderPass.renderPassData.depthAttachmentRef.attachment = index;
                renderPass.renderPassData.depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            }
            else
            {
                VkAttachmentDescription attachmentDesc{};
                attachmentDesc.format = renderPass.renderPassChain.ImageFormats[index];
                attachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
                attachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                attachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                attachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                attachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                attachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                attachmentDesc.finalLayout = forPresent ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                renderPass.renderPassData.colorAttachments.push_back(attachmentDesc);

                VkAttachmentReference attachmentRef{};
                attachmentRef.attachment = index;
                attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                renderPass.renderPassData.colorAttachmentRefs.push_back(attachmentRef);
            }

            index++;
        }

        renderPass.renderPassData.hasDepthAttachment = containsDepthAttachment;

        renderPass.renderPassData.subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        renderPass.renderPassData.subpass.colorAttachmentCount = renderPass.renderPassData.colorAttachments.size();
        renderPass.renderPassData.subpass.pColorAttachments = renderPass.renderPassData.colorAttachmentRefs.data();

        if (containsDepthAttachment)
            renderPass.renderPassData.subpass.pDepthStencilAttachment = &renderPass.renderPassData.depthAttachmentRef;

        renderPass.renderPassData.dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        renderPass.renderPassData.dependency.dstSubpass = 0;
        renderPass.renderPassData.dependency.srcAccessMask = 0;

        renderPass.renderPassData.dependency.srcAccessMask = 0;

        if (containsDepthAttachment)
        {
            renderPass.renderPassData.dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            renderPass.renderPassData.dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            renderPass.renderPassData.dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        }
        else
        {
            renderPass.renderPassData.dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            renderPass.renderPassData.dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            renderPass.renderPassData.dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        }

        std::vector<VkAttachmentDescription> attachments = renderPass.renderPassData.colorAttachments;
        if (containsDepthAttachment)
            attachments.push_back(renderPass.renderPassData.depthAttachment);

        VkRenderPassCreateInfo renderPassCreateInfo{};
        renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassCreateInfo.pAttachments = attachments.data();
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &renderPass.renderPassData.subpass;
        renderPassCreateInfo.dependencyCount = 1;
        renderPassCreateInfo.pDependencies = &renderPass.renderPassData.dependency;

        if (vkCreateRenderPass(vk_data.logicalDevice, &renderPassCreateInfo, nullptr, &renderPass.renderPass) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create render pass!");
        }
    }

    static void createRenderPassInfo(uint32_t imageIndex, VulkanData &vk_data, VulkanRenderPass &renderPass)
    {
        renderPass.renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPass.renderPassInfo.renderPass = renderPass.renderPass;

        if(imageIndex >= renderPass.renderPassChain.Framebuffers.size() ) imageIndex = 0;

        renderPass.renderPassInfo.framebuffer = renderPass.renderPassChain.Framebuffers[imageIndex];
        renderPass.renderPassInfo.renderArea.offset = {0, 0};
        renderPass.renderPassInfo.renderArea.extent = renderPass.renderPassChain.Extent;

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