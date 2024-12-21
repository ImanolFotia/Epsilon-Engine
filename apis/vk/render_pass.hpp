#pragma once

#include <vulkan/vulkan_core.h>
#if !defined(__ANDROID__)
#include <vulkan/vulkan.hpp>
#endif

#include "device.hpp"
#include "swap_chain.hpp"
#include "texture.hpp"
#include "vk_data.hpp"

#include "core/engine/renderer/types.hpp"
#include <core/framework/containers/static_array.hpp>

namespace vk {
static void GetNumSamplesFormat(uint8_t numSamples) {}

static void createRenderPass(VulkanData &vk_data, VulkanRenderPass &renderPass, engine::RenderPassInfo &renderPassInfo, bool forPresent) {
  bool containsDepthAttachment = false;
  renderPass.renderPassData.colorAttachments.clear();
  renderPass.renderPassData.colorAttachments.resize(0);

  renderPass.renderPassData.colorAttachmentRefs.clear();
  renderPass.renderPassData.colorAttachmentRefs.resize(0);

  unsigned index = 0;
  for (auto &attachment : renderPassInfo.attachments) {
    if (attachment.isDepthAttachment) {
      containsDepthAttachment = true;
      if (attachment.format == engine::DEPTH_F32_STENCIL_8 || attachment.format == engine::DEPTH_F16_STENCIL_8 || attachment.format == engine::DEPTH_F32 ||
          attachment.format == engine::DEPTH_F16 || attachment.format == engine::DEPTH_UNORM || attachment.format == engine::DEPTH_UNORM_STENCIL_8) {
        renderPass.renderPassData.depthAttachment.format = findDepthFormat(vk_data);
      }

      if (renderPassInfo.numSamples == 1) {
        renderPass.renderPassData.depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        renderPass.renderPassData.depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        renderPass.renderPassData.depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
      } else {
        renderPass.renderPassData.depthAttachment.samples = (VkSampleCountFlagBits)renderPassInfo.numSamples;
        renderPass.renderPassData.depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        renderPass.renderPassData.depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
      }

      renderPass.renderPassData.depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

      if (renderPass.renderPassData.depthAttachment.loadOp == VK_ATTACHMENT_LOAD_OP_LOAD) {
        renderPass.renderPassData.depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
      }

      renderPass.renderPassData.depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
      renderPass.renderPassData.depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;

      renderPass.renderPassData.depthAttachmentRef.layout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;

      if (renderPass.renderPassData.depthWriteEnabled) {
        renderPass.renderPassData.depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
      } else {
        renderPass.renderPassData.depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
      }

        if (renderPass.renderPassData.depthAttachment.loadOp == VK_ATTACHMENT_LOAD_OP_LOAD) {
          renderPass.renderPassData.depthAttachment.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        }

      renderPass.renderPassData.depthAttachmentRef.attachment = index;
      // renderPass.renderPassData.depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
      /*if (forPresent)
      {
          renderPass.renderPassData.depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
          renderPass.renderPassData.depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
          renderPass.renderPassData.depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
      }*/
    } else {
      VkAttachmentDescription attachmentDesc{};
      attachmentDesc.format = renderPass.renderPassChain.ImageFormats[index];
      attachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;

      if (renderPassInfo.numSamples == 1) {
        attachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
        attachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD; // attachment.clearAttachment ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;;
        attachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
      } else {
        attachmentDesc.samples = (VkSampleCountFlagBits)renderPassInfo.numSamples;
        attachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
      }

      attachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
      attachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
      if (forPresent) {
        attachmentDesc.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        if (attachmentDesc.loadOp == VK_ATTACHMENT_LOAD_OP_LOAD) {
          attachmentDesc.initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        }
        // attachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        // attachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
      } else {
        attachmentDesc.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        if (attachmentDesc.loadOp == VK_ATTACHMENT_LOAD_OP_LOAD)
          attachmentDesc.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

      }
      if (forPresent) {
        if (renderPassInfo.numSamples == 1)
          attachmentDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        else
          attachmentDesc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

      } else if (attachment.format == engine::DEPTH_F32 || attachment.format == engine::DEPTH_F32_STENCIL_8) {
        // if (attachment.clearAttachment)
        //{
        //     attachmentDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        // }
        // else
        {
          attachmentDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

          if (renderPass.renderPassData.depthWriteEnabled) {
            attachmentDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
          } else {
            attachmentDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
          }
        }
      } else {
        attachmentDesc.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        attachmentDesc.finalLayout = attachment.storageImage ? VK_IMAGE_LAYOUT_GENERAL : attachmentDesc.finalLayout;
      }
      renderPass.renderPassData.colorAttachments.push_back(attachmentDesc);

      VkAttachmentReference attachmentRef{};
      attachmentRef.attachment = index;
      
      if (attachment.format == engine::DEPTH_F32 || attachment.format == engine::DEPTH_F32_STENCIL_8) {

        if (renderPass.renderPassData.depthWriteEnabled) {
          attachmentRef.layout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
        } else {
          attachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
        }

        
      } else {
        attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      }
      // attachmentRef.layout = attachment.format == engine::DEPTH_F32 ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL :
      // VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      renderPass.renderPassData.colorAttachmentRefs.push_back(attachmentRef);
    }

    index++;
  }

  VkAttachmentDescription colorAttachmentResolve{};
  VkAttachmentReference colorAttachmentResolveRef{};

  if (renderPassInfo.numSamples > 1) {
    colorAttachmentResolve.format = renderPass.renderPassChain.ImageFormats[0];
    colorAttachmentResolve.samples = (VkSampleCountFlagBits)renderPassInfo.numSamples;
    colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    if (forPresent) {
      colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    } else {
      colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      // colorAttachmentResolve.finalLayout = attachment.storageImage ? VK_IMAGE_LAYOUT_GENERAL : colorAttachmentResolve.finalLayout;
    }
    colorAttachmentResolveRef.attachment = 2;
    colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    renderPass.renderPassData.subpass.pResolveAttachments = &colorAttachmentResolveRef;
  }

  renderPass.renderPassData.hasDepthAttachment = containsDepthAttachment;

  renderPass.renderPassData.subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  renderPass.renderPassData.subpass.colorAttachmentCount = renderPass.renderPassData.colorAttachments.size();

  renderPass.numAttachments = renderPass.renderPassData.subpass.colorAttachmentCount;
  renderPass.renderPassData.subpass.pColorAttachments = renderPass.renderPassData.colorAttachmentRefs.data();

  if (containsDepthAttachment)
    renderPass.renderPassData.subpass.pDepthStencilAttachment = &renderPass.renderPassData.depthAttachmentRef;
  std::array<VkSubpassDependency, 2> dependencies;

  dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
  dependencies[0].dstSubpass = 0;
  dependencies[0].srcStageMask =
      VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT; // VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
  dependencies[0].dstStageMask =
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
  ;                                                          // VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
  dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT; // VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
  dependencies[0].dstAccessMask =
      VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
      VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;              // VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
  dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT; // 0;

  dependencies[1].srcSubpass = 0;
  dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL; // VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // 0;
  dependencies[1].srcStageMask =
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
  ;                                                                     // VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT; // VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                                  VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
  ;                                                              // 0;
  dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;     // VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
  dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT; // 0;

  if (containsDepthAttachment)
    renderPass.renderPassData.colorAttachments.push_back(renderPass.renderPassData.depthAttachment);

  if (renderPassInfo.numSamples > 1) {
    renderPass.renderPassData.colorAttachments.push_back(colorAttachmentResolve);
    renderPass.renderPassData.colorAttachmentRefs.push_back(colorAttachmentResolveRef);
  }

  // renderPass.renderPassData.subpass.preserveAttachmentCount = renderPass.renderPassData.colorAttachments.size();

  // std::vector<uint32_t> preserve_indices;
  // for(int i = 0; i < renderPass.renderPassData.colorAttachments.size(); i++)  preserve_indices.push_back(i);

  // renderPass.renderPassData.subpass.pPreserveAttachments = preserve_indices.data();

  VkRenderPassCreateInfo renderPassCreateInfo{};
  renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassCreateInfo.attachmentCount = renderPass.renderPassData.colorAttachments.size();
  renderPassCreateInfo.pAttachments = renderPass.renderPassData.colorAttachments.data();
  renderPassCreateInfo.subpassCount = 1;
  renderPassCreateInfo.pSubpasses = &renderPass.renderPassData.subpass;
  renderPassCreateInfo.dependencyCount = 2;
  renderPassCreateInfo.pDependencies = dependencies.data(); //&renderPass.renderPassData.dependency;

  if (vkCreateRenderPass(vk_data.logicalDevice, &renderPassCreateInfo, nullptr, &renderPass.renderPass) != VK_SUCCESS) {
    throw std::runtime_error("failed to create render pass!");
  }
}

static void createRenderPassInfo(uint32_t imageIndex, VulkanData &vk_data, VulkanRenderPass &renderPass) {
  renderPass.renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPass.renderPassInfo.renderPass = renderPass.renderPass;

  if (imageIndex >= renderPass.renderPassChain.Framebuffers.size())
    imageIndex = 0;

  renderPass.renderPassInfo.framebuffer = renderPass.renderPassChain.Framebuffers[imageIndex];

  // renderPass.renderPassInfo.clearValueCount = renderPass.clearValues.size();
  // renderPass.renderPassInfo.pClearValues = renderPass.clearValues.data();
}

static void beginRenderPass(const VkCommandBuffer &commandBuffer, VulkanRenderPass &renderPass) {
  if (renderPass.renderPassInfo.renderPass == 0x0) {
    std::cout << "renderPass is invalid" << std::endl;
    return;
  }
  vkCmdBeginRenderPass(commandBuffer, &renderPass.renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

static void endRenderPass(const VkCommandBuffer &commandBuffer, const VulkanData &vk_data) { vkCmdEndRenderPass(commandBuffer); }

static void cleanupRenderPass(const VulkanData &vk_data, VkRenderPass &renderPass) { vkDestroyRenderPass(vk_data.logicalDevice, renderPass, nullptr); }
} // namespace vk