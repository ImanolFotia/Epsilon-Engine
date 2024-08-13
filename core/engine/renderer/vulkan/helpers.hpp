#pragma once

#include "../draw_command.hpp"
#include "vulkan/vulkan_core.h"
#include <vulkan/vulkan.h>

namespace engine {

static VkFormat resolveFormat(engine::TextureFormat format,
                              bool isCompressed = false) {
  // Color formats
  if (format == COLOR_R)
    return VK_FORMAT_R8_UNORM;
  if (format == COLOR_RG)
    return VK_FORMAT_R8G8_SRGB;
  if (format == COLOR_RGB)
    return VK_FORMAT_R8G8B8_SRGB;
  if (format == COLOR_RGBA)
    return VK_FORMAT_R8G8B8A8_SRGB;
  if (format == COLOR_R_16F)
    return VK_FORMAT_R16_SFLOAT;
  if (format == COLOR_R_32F || format == NON_COLOR_R_32F)
    return VK_FORMAT_R32_SFLOAT;
  if (format == COLOR_RG_16F)
    return VK_FORMAT_R16G16_SFLOAT;
  if (format == COLOR_RG_32F)
    return VK_FORMAT_R32G32_SFLOAT;
  if (format == COLOR_RGB_16F)
    return VK_FORMAT_R16G16B16_SFLOAT;
  if (format == COLOR_RGB_32F)
    return VK_FORMAT_R32G32B32_SFLOAT;
  if (format == COLOR_RGBA_16F)
    return VK_FORMAT_R16G16B16A16_SFLOAT;
  if (format == COLOR_RGBA_32F)
    return VK_FORMAT_R32G32B32A32_SFLOAT;

  if (format == COLOR_RGB_BC1)
    return VK_FORMAT_BC1_RGB_SRGB_BLOCK;
  if (format == COLOR_RGB_BC2)
    return VK_FORMAT_BC2_SRGB_BLOCK;
  if (format == COLOR_RGBA_BC1)
    return VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
  if (format == COLOR_RGBA_BC3)
    return VK_FORMAT_BC3_SRGB_BLOCK;

  if (format == COLOR_RGBA_BC6HS)
    return VK_FORMAT_BC6H_SFLOAT_BLOCK;
  if (format == COLOR_RGBA_BC6HU)
    return VK_FORMAT_BC6H_UFLOAT_BLOCK;

  if (format == COLOR_RGBA_BC7)
    return VK_FORMAT_BC7_SRGB_BLOCK;

  if (format == NON_COLOR_RGB_BC1)
    return VK_FORMAT_BC1_RGB_UNORM_BLOCK;
  if (format == NON_COLOR_RGB_BC2)
    return VK_FORMAT_BC2_UNORM_BLOCK;
  if (format == NON_COLOR_RGB_BC3)
    return VK_FORMAT_BC3_UNORM_BLOCK;
  if (format == NON_COLOR_RGBA_BC3)
    return VK_FORMAT_BC3_UNORM_BLOCK;

  if (format == NON_COLOR_RG_BC5U)
    return VK_FORMAT_BC5_UNORM_BLOCK;

  if (format == NON_COLOR_RGBA_BC7)
    return VK_FORMAT_BC7_UNORM_BLOCK;

  // Non color formats (i.e. Normal Maps)
  if (format == NON_COLOR_R)
    return VK_FORMAT_R8_UNORM;
  if (format == NON_COLOR_RG)
    return VK_FORMAT_R8G8_UNORM;
  if (format == NON_COLOR_RGB)
    return VK_FORMAT_R8G8B8_UNORM;
  if (format == NON_COLOR_RGBA)
    return VK_FORMAT_R8G8B8A8_UNORM;

  if (format == DEPTH_F16)
    return VK_FORMAT_D16_UNORM_S8_UINT;
  if (format == DEPTH_F32)
    return VK_FORMAT_D32_SFLOAT;
  if (format == DEPTH_F32_STENCIL_8)
    return VK_FORMAT_D32_SFLOAT_S8_UINT;
  return VK_FORMAT_MAX_ENUM;
}

static VkFilter resolveFilter(engine::Filtering filter) {
  if (filter == POINT)
    return VK_FILTER_NEAREST;
  if (filter == LINEAR)
    return VK_FILTER_LINEAR;

  return VK_FILTER_MAX_ENUM;
}

static VkSamplerAddressMode resolveWrapping(engine::WrapMode wrapping) {
  if (wrapping == REPEAT)
    return VK_SAMPLER_ADDRESS_MODE_REPEAT;
  if (wrapping == CLAMP_TO_BORDER)
    return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
  if (wrapping == CLAMP_TO_EDGE)
    return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  return VK_SAMPLER_ADDRESS_MODE_MAX_ENUM;
}

static VkCompareOp resolveCompareOp(engine::CompareFunction compareOp) {
  if (compareOp == ALWAYS)
    return VK_COMPARE_OP_ALWAYS;
  if (compareOp == LESS)
    return VK_COMPARE_OP_LESS;
  if (compareOp == LESS_OR_EQUAL)
    return VK_COMPARE_OP_LESS_OR_EQUAL;
  if (compareOp == EQUAL)
    return VK_COMPARE_OP_EQUAL;

  return VK_COMPARE_OP_ALWAYS;
}

static unsigned resolveNumChannels(engine::TextureFormat format) {
  if (format == COLOR_R || format == NON_COLOR_R || format == DEPTH_F16 ||
      format == DEPTH_F32 || format == DEPTH_F32_STENCIL_8 ||
      format == COLOR_R_32F || format == COLOR_R_16F) {
    return 1;
  }

  if (format == COLOR_RG || format == COLOR_RG_16F || format == COLOR_RG_32F ||
      format == NON_COLOR_RG) {
    return 2;
  }

  if (format == COLOR_RGB || format == COLOR_RGB_16F ||
      format == COLOR_RGB_32F || format == NON_COLOR_RGB) {
    return 3;
  }
  if (format == COLOR_RGBA || format == COLOR_RGBA_16F ||
      format == COLOR_RGBA_32F || format == NON_COLOR_RGBA) {
    return 4;
  }

  return 0;
}

static VkDescriptorType resolveBindingType(engine::UniformBindingType type) {
  using TYPE = engine::UniformBindingType;

  if (type == TYPE::SHADER_STORAGE)
    return VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  if (type == TYPE::ACCELERATION_STRUCTURE)
    return VkDescriptorType::VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
  if (type == TYPE::STORAGE_IMAGE)
    return VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
  if (type == TYPE::TEXTURE_IMAGE_COMBINED_SAMPLER)
    return VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  if (type == TYPE::TEXTURE_SAMPLER)
    return VkDescriptorType::VK_DESCRIPTOR_TYPE_SAMPLER;
  if (type == TYPE::UNIFORM_BUFFER)
    return VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

  return VkDescriptorType::VK_DESCRIPTOR_TYPE_MAX_ENUM;
}

static std::vector<IndirectBatch>
generateIndirectBatch(std::vector<DrawCommand> &commandLists,
                      unsigned int commandCount) {
  std::vector<IndirectBatch> batches{};
  batches.resize(commandLists.size());
  unsigned index = 0;
  unsigned current = 0;

  for (const auto &command : commandLists) {

    if (index >= commandCount)
      break;
    if (index == 0) {
      batches[current] = {.meshResource = command.meshResource,
                          .material = command.material,
                          .uniformIndex = command.uniformIndex,
                          .layoutIndex = command.layoutIndex,
                          .count = 1,
                          .first = 0};
      index++;
      continue;
    }
    bool isSameMaterial =
        Ref<BindGroup>::isSame(command.material, batches[current].material);
    bool isSameVertexBuffer =
        Ref<Buffer>::isSame(command.meshResource.vertexBuffer,
                            batches[current].meshResource.vertexBuffer);
    bool isSameIndexBuffer =
        Ref<Buffer>::isSame(command.meshResource.indexBuffer,
                            batches[current].meshResource.indexBuffer);
    bool isSameLayout = command.layoutIndex == batches[current].layoutIndex;
    if (isSameMaterial && isSameVertexBuffer && isSameIndexBuffer &&
        isSameLayout) {
      batches[current].count++;
    } else {
      current++;
      batches[current] = {.meshResource = command.meshResource,
                          .material = command.material,
                          .uniformIndex = command.uniformIndex,
                          .layoutIndex = command.layoutIndex,
                          .count = 1,
                          .first = index};
    }

    index++;
  }
  if (commandCount > 0)
    batches.resize(current + 1);
  else
    batches.resize(commandCount);

  return batches;
}
} // namespace engine