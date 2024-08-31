#pragma once

#include <optional>
#include <vector>

#if !defined(__ANDROID__)
#include <vulkan/vulkan.hpp>
#endif

#include "surface.hpp"
#include "vk_data.hpp"

namespace vk {

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;
  std::optional<uint32_t> computeFamily;
  std::optional<uint32_t> transferFamily;

  std::unordered_map<uint32_t, uint32_t> queueFamilyCount;

  int queueCount = 0;

  bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value() && computeFamily.has_value() && transferFamily.has_value(); }
};

static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice &physicalDevice, const VulkanData &vk_data) {
  QueueFamilyIndices indices;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

  uint32_t i = 0;
  int graphics_and_transfer = 0;
  int graphics_and_compute = 0;
  for (const auto &queueFamily : queueFamilies) {
    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT && !indices.graphicsFamily.has_value()) {
      indices.graphicsFamily = i;
      indices.queueFamilyCount[i] = queueFamily.queueCount;
      if(queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT)
        indices.transferFamily = i;
    }

    if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) {
      if ((indices.graphicsFamily.has_value() && indices.graphicsFamily.value() != i) &&
          (indices.transferFamily.has_value() && indices.transferFamily.value() != i)) {
        indices.computeFamily = i;
        indices.queueFamilyCount[i] = queueFamily.queueCount;
      }
    }

    if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT && !indices.transferFamily.has_value()) {
      if (indices.graphicsFamily.has_value() && indices.graphicsFamily.value() != i) {
        indices.transferFamily = i;
        indices.queueFamilyCount[i] = queueFamily.queueCount;
      }
    }

    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT && queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) {
      graphics_and_transfer = i;
    }

    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT && queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) {
      graphics_and_compute = i;
    }

    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, vk_data.surface, &presentSupport);
    if (presentSupport && !indices.presentFamily.has_value()) {
      indices.presentFamily = i;
      indices.queueFamilyCount[i] = queueFamily.queueCount;
    }

    if (indices.isComplete()) {
      indices.queueCount = queueFamily.queueCount;
      break;
    }

    i++;
  }

  if (!indices.transferFamily.has_value())
    indices.transferFamily = graphics_and_transfer;
  if (!indices.computeFamily.has_value())
    indices.computeFamily = graphics_and_compute;

  return indices;
}

} // namespace vk