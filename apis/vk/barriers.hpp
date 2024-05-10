#pragma once

#include <vulkan/vulkan.h>
#include <core/engine/renderer/types.hpp>

namespace vk
{

	static void ResolveMasks(engine::MemoryBarrierHint hint,
							 VkAccessFlags &srcAccessMask,
							 VkAccessFlags &dstAccessMask,
							 VkPipelineStageFlags2 &srcStageMask,
							 VkPipelineStageFlags2 &dstStageMask)
	{
		using engine::MemoryBarrierHint;
		switch (hint)
		{
		case MemoryBarrierHint::COMPUTE_WRITE_BUFFER_COMPUTE_READ_BUFFER:
			srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
			dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			srcStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
			dstStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
			break;
		case MemoryBarrierHint::COMPUTE_READ_BUFFER_COMPUTE_WRITE_BUFFER:
			srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
			srcStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
			dstStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
			break;
		case MemoryBarrierHint::COMPUTE_WRITE_STORAGE_IMAGE_COMPUTE_READ_STORAGE_IMAGE:
			srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
			dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			srcStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
			dstStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
			break;
		case MemoryBarrierHint::COMPUTE_READ_STORAGE_IMAGE_COMPUTE_WRITE_STORAGE_IMAGE:
			srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
			srcStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
			dstStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
			break;
		case MemoryBarrierHint::COMPUTE_WRITE_BUFFER_GRAPHICS_READ_BUFFER:
			srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
			dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			srcStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
			dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			break;
		case MemoryBarrierHint::COMPUTE_WRITE_BUFFER_GRAPHICS_READ_INDEX:
			srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
			dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			srcStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
			dstStageMask = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
			break;
		case MemoryBarrierHint::COMPUTE_WRITE_BUFFER_GRAPHICS_READ_UNIFORM:

			srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
			dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			srcStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
			dstStageMask = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
			break;
		case MemoryBarrierHint::COMPUTE_WRITE_BUFFER_GRAPHICS_READ_INDIRECT:
			srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
			dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			srcStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
			dstStageMask = VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
			break;
		case MemoryBarrierHint::COMPUTE_WRITE_STORAGE_IMAGE_GRAPHICS_SAMPLE_IMAGE:
			srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
			dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			srcStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
			dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			break;
		case MemoryBarrierHint::COMPUTE_WRITE_STORAGE_TEXEL_GRAPHICS_READ_INDIRECT:
			srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
			dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			srcStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
			dstStageMask = VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
			break;
		case MemoryBarrierHint::COMPUTE_WRITE_STORAGE_TEXEL_GRAPHICS_READ_UNIFORM:
			srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
			dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			srcStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
			dstStageMask = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
			break;
		case MemoryBarrierHint::GRAPHICS_WRITE_COLOR_COMPUTE_SAMPLE_IMAGE:
			srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT_KHR;
			dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
			srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			dstStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
			break;
		case MemoryBarrierHint::GRAPHICS_WRITE_DEPTH_COMPUTE_SAMPLE_IMAGE:
			srcAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
			srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			dstStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
			break;
		case MemoryBarrierHint::NONE:
			break;
		}
	}

	static void CreateMemoryBarrier() {}
	/*
		static void CreateImageMemoryBarrier(engine::ImageMemoryBarrierInfo barrierInfo)
		{

			VkImageSubresourceRange subresourceRange;
			subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
			subresourceRange.baseMipLevel = 0;
			subresourceRange.levelCount = 1;
			subresourceRange.baseArrayLayer = 0;
			subresourceRange.layerCount = 1;

			VkAccessFlags & srcAccessMask;
			VkAccessFlags & dstAccessMask;
			VkPipelineStageFlag & srcStageMask;
			VkPipelineStageFlag & dstStageMask;

			ResolveMasks(barrierInfo.hint, srcAccessMask, dstAccessMask, srcStageMask, dstStageMask);

			VkImageMemoryBarrier imageMemoryBarrier = {};
			imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			// We won't be changing the layout of the image
			imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
			imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
			imageMemoryBarrier.image = barrierInfo.image;
			imageMemoryBarrier.subresourceRange = {VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, 0, 1, 0, 1};
			imageMemoryBarrier.srcAccessMask = srcAccessMask;
			imageMemoryBarrier.dstAccessMask = dstAccessMask;
			imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

			vkCmdPipelineBarrier(
				m_pFrame.ComputeCommandBuffer(),
				srcStageMask,
				dstStageMask,
				0,
				0, nullptr,
				0, nullptr,
				1, &imageMemoryBarrier);

			return imageMemoryBarrier;
		}*/
}