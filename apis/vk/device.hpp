#pragma once

#include "core/framework/log.hpp"
#include "queues.hpp"
#include "swap_chain.hpp"
// #include "validation_layers.hpp"
#include "command.hpp"
#include "instance.hpp"
#include "pipeline.hpp"
#include "sync_objects.hpp"
#include "vk_data.hpp"

#if defined(ANDROID) || defined(__ANDROID__)

#include <android/log.h>

#define LOG_TAG "Epsilon"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#endif

namespace vk {
static void createLogicalDevice(VulkanData &vk_data) {
  QueueFamilyIndices indices = findQueueFamilies(vk_data.physicalDevice, vk_data);

  vk_data.uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value(), indices.computeFamily.value(), indices.transferFamily.value()};

  std::vector<float> computeQueuePriority;
  computeQueuePriority.resize(indices.queueFamilyCount.at(indices.computeFamily.value()));
  std::fill(computeQueuePriority.begin(), computeQueuePriority.end(),
            1.0f); // [3] = { 1.0f , 1.0f, 1.0f };

  std::vector<float> graphicsQueuePriority;
  graphicsQueuePriority.resize(indices.queueFamilyCount.at(indices.graphicsFamily.value()));
  std::fill(graphicsQueuePriority.begin(), graphicsQueuePriority.end(),
            1.0f);
  /*for (uint32_t queueFamily : vk_data.uniqueQueueFamilies)
  {
      VkDeviceQueueCreateInfo queueCreateInfo{};
      queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queueCreateInfo.queueFamilyIndex = queueFamily;
      queueCreateInfo.queueCount = 1;
      queueCreateInfo.pQueuePriorities = &queuePriority;
      queueCreateInfos.push_back(queueCreateInfo);
  }*/

  //!!! TODO: Should ask where these extensions are avaliable
  VkPhysicalDeviceFeatures deviceFeatures{};

#if !defined(__ANDROID__)

  deviceFeatures.samplerAnisotropy = VK_TRUE;
  deviceFeatures.multiDrawIndirect = VK_TRUE;
  deviceFeatures.shaderClipDistance = VK_TRUE;
  deviceFeatures.shaderSampledImageArrayDynamicIndexing = VK_TRUE;
  VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures layoutFeatures{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SEPARATE_DEPTH_STENCIL_LAYOUTS_FEATURES};
  VkPhysicalDeviceDescriptorIndexingFeatures indexing_features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT, nullptr};
  VkPhysicalDeviceSynchronization2Features sync_features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES, nullptr, VK_TRUE};
  VkPhysicalDeviceMemoryProperties memory_properties{};
  VkPhysicalDeviceProperties physicalProperties{};
  VkFormatProperties formatProperties;
  VkPhysicalDeviceFeatures2 device_features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, &indexing_features};
  VkPhysicalDeviceFeatures2 device_features2{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, &layoutFeatures};

  vkGetPhysicalDeviceFeatures2(vk_data.physicalDevice, &device_features);
  vkGetPhysicalDeviceFeatures2(vk_data.physicalDevice, &device_features2);

  vkGetPhysicalDeviceMemoryProperties(vk_data.physicalDevice, &memory_properties);
  vkGetPhysicalDeviceFormatProperties(vk_data.physicalDevice, VK_FORMAT_B8G8R8A8_SRGB, &formatProperties);

  vkGetPhysicalDeviceProperties(vk_data.physicalDevice, &physicalProperties);


  vk_data.max_memory_heaps = memory_properties.memoryHeapCount;

  bool bindless_supported = indexing_features.descriptorBindingPartiallyBound && indexing_features.runtimeDescriptorArray;
  bool separateStencilSupported = layoutFeatures.separateDepthStencilLayouts;
  bool supporstBlit = formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT;

  VkPhysicalDeviceFeatures2 physical_features2 = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2};
  vkGetPhysicalDeviceFeatures2(vk_data.physicalDevice, &physical_features2);

  if (separateStencilSupported) {
    indexing_features.pNext = &layoutFeatures;
    Log::Info("Separate Stencil is supported");
  }

  if(supporstBlit) {
    vk_data.blitSupported = true;
    Log::Info("Blit for format 'VK_FORMAT_B8G8R8A8_SRGB' is supported");
  }

  Log::Info(std::format("timestampPeriod is{} supported", physicalProperties.limits.timestampPeriod == 0 ? " not" : ""));


  if (bindless_supported) {
    Log::Info("bindless is supported");
    indexing_features.descriptorBindingPartiallyBound = VK_TRUE;
    indexing_features.descriptorBindingSampledImageUpdateAfterBind = VK_TRUE;
    indexing_features.runtimeDescriptorArray = VK_TRUE;
    indexing_features.pNext = &sync_features;

    physical_features2.pNext = &indexing_features;
    vk_data.bindless_supported = true;
  } else {
    Log::Info("bindless is not supported");
  }

#endif
  VkSurfaceCapabilitiesKHR surfaceCapabilities;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_data.physicalDevice, vk_data.surface, &surfaceCapabilities);

  MIN_FRAMES_IN_FLIGHT = surfaceCapabilities.minImageCount;
  MAX_FRAMES_IN_FLIGHT = std::max(MIN_FRAMES_IN_FLIGHT, MAX_FRAMES_IN_FLIGHT);

  Log::Info("Min frames in flight: ", MIN_FRAMES_IN_FLIGHT);

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfo{};
  queueCreateInfo.resize(vk_data.uniqueQueueFamilies.size());
  
 
  queueCreateInfo[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueCreateInfo[0].queueFamilyIndex = indices.graphicsFamily.value();
  queueCreateInfo[0].queueCount = std::min(indices.queueFamilyCount.at(indices.graphicsFamily.value()), 1+(MAX_FRAMES_IN_FLIGHT*2));
  queueCreateInfo[0].pQueuePriorities = graphicsQueuePriority.data();

  if (indices.computeFamily.value() != indices.graphicsFamily.value()) {
    queueCreateInfo[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo[1].queueFamilyIndex = indices.computeFamily.value();
    queueCreateInfo[1].queueCount = std::min(indices.queueFamilyCount.at(indices.computeFamily.value()), MAX_FRAMES_IN_FLIGHT);
    queueCreateInfo[1].pQueuePriorities = computeQueuePriority.data();
  }

  Log::Info(std::format("Graphics queue family: {} support timestamp queries", indices.timestampValidBits[indices.graphicsFamily.value()] == 0 ? "does not" : "does"));
  Log::Info(std::format("Compute queue family: {} support timestamp queries", indices.timestampValidBits[indices.computeFamily.value()] == 0 ? "does not" : "does"));
  Log::Info(std::format("Transfer queue family: {} support timestamp queries", indices.timestampValidBits[indices.transferFamily.value()] == 0 ? "does not" : "does"));
  Log::Info(std::format("Present queue family: {} support timestamp queries", indices.timestampValidBits[indices.presentFamily.value()] == 0 ? "does not" : "does"));


  VkDeviceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.pQueueCreateInfos = queueCreateInfo.data();
  createInfo.queueCreateInfoCount = vk_data.uniqueQueueFamilies.size();

  createInfo.enabledExtensionCount = static_cast<uint32_t>(vk_data.deviceExtensions.size());
  createInfo.ppEnabledExtensionNames = vk_data.deviceExtensions.data();

  Log::Info("Required extensions\n");
  for (const auto &extension : vk_data.deviceExtensions) {
    Log::Info(extension);
  }
  Log::Info("\n");

#if !defined(__ANDROID__)
  createInfo.pNext = &physical_features2;
#else
  createInfo.pEnabledFeatures = &deviceFeatures;
#endif
  //   createInfo.enabledExtensionCount = 0;

  if (enableValidationLayers) {
    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
  } else {
    createInfo.enabledLayerCount = 0;
  }

  if (auto deviceResult = vkCreateDevice(vk_data.physicalDevice, &createInfo, nullptr, &vk_data.logicalDevice); deviceResult != VK_SUCCESS) {
    Log::Error("Error creating the device: ", deviceResult);
    throw std::runtime_error("failed to create logical device!");
  }

  vk_data.presentQueue.resize(MAX_FRAMES_IN_FLIGHT);
  vk_data.graphicsQueue.resize(MAX_FRAMES_IN_FLIGHT);
  vk_data.computeQueue.resize(MAX_FRAMES_IN_FLIGHT);
  // if (vk_data.uniqueQueueFamilies.size() >= MAX_FRAMES_IN_FLIGHT) {

  //this might break on some gpus
  for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
    vkGetDeviceQueue(vk_data.logicalDevice, indices.graphicsFamily.value(), 0, &vk_data.presentQueue[i]);

  for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
    vkGetDeviceQueue(vk_data.logicalDevice, indices.graphicsFamily.value(), 0, &vk_data.graphicsQueue[i]);

  for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkGetDeviceQueue(vk_data.logicalDevice, indices.graphicsFamily.value(), 0, &vk_data.computeQueue[i]);
  }

  vkGetDeviceQueue(vk_data.logicalDevice, indices.graphicsFamily.value(), 1, &vk_data.transferQueue);
  /* } else {

     for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
       vkGetDeviceQueue(vk_data.logicalDevice, indices.presentFamily.value(), 0, &vk_data.presentQueue[i]);
       vkGetDeviceQueue(vk_data.logicalDevice, indices.graphicsFamily.value(), 0, &vk_data.graphicsQueue[i]);
       vkGetDeviceQueue(vk_data.logicalDevice, indices.computeFamily.value(), 0, &vk_data.computeQueue[i]);
     }
     vkGetDeviceQueue(vk_data.logicalDevice, indices.transferFamily.value(), 0, &vk_data.transferQueue);
   }*/
}

static VkSampleCountFlagBits getMaxUsableSampleCount(VkPhysicalDevice physicalDevice) {
  VkPhysicalDeviceProperties physicalDeviceProperties;
  vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
  Log::Info("max indirected draws: ", physicalDeviceProperties.limits.maxDrawIndirectCount);

  VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
  if (counts & VK_SAMPLE_COUNT_64_BIT) {
    return VK_SAMPLE_COUNT_64_BIT;
  }
  if (counts & VK_SAMPLE_COUNT_32_BIT) {
    return VK_SAMPLE_COUNT_32_BIT;
  }
  if (counts & VK_SAMPLE_COUNT_16_BIT) {
    return VK_SAMPLE_COUNT_16_BIT;
  }
  if (counts & VK_SAMPLE_COUNT_8_BIT) {
    return VK_SAMPLE_COUNT_8_BIT;
  }
  if (counts & VK_SAMPLE_COUNT_4_BIT) {
    return VK_SAMPLE_COUNT_4_BIT;
  }
  if (counts & VK_SAMPLE_COUNT_2_BIT) {
    return VK_SAMPLE_COUNT_2_BIT;
  }

  return VK_SAMPLE_COUNT_1_BIT;
}

static void showDeviceFeatures(VkPhysicalDevice device) {

  VkPhysicalDeviceProperties deviceProperties;
  vkGetPhysicalDeviceProperties(device, &deviceProperties);

  VkPhysicalDeviceFeatures deviceFeatures;
  vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
  Log::Info("Selected device: ", deviceProperties.deviceName);
  Log::Info("\tApi version: ", deviceProperties.apiVersion);
  Log::Info("\tID: ", deviceProperties.deviceID);
  Log::Info("\tDriver version: ", deviceProperties.driverVersion);
  Log::Info("\tVendor ID: ", deviceProperties.vendorID);
  Log::Info("\tIs discrete device: ", (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? "yes" : "no"));
}

static bool checkDeviceExtensionSupport(VkPhysicalDevice device, const VulkanData &vk_data) {
  uint32_t extensionCount;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

  std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

  std::set<std::string> requiredExtensions(vk_data.deviceExtensions.begin(), vk_data.deviceExtensions.end());

  for (const auto &extension : availableExtensions) {
    requiredExtensions.erase(extension.extensionName);
  }

  return requiredExtensions.empty();
}

static bool isDeviceSuitable(VkPhysicalDevice device, const VulkanData &vk_data) {
  showDeviceFeatures(device);

  bool extensionsSupported = checkDeviceExtensionSupport(device, vk_data);

  QueueFamilyIndices indices = findQueueFamilies(device, vk_data);

  bool swapChainAdequate = false;

  if (extensionsSupported) {
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device, vk_data);
    swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
  }

  VkPhysicalDeviceFeatures supportedFeatures;
  vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

  return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

static VkPhysicalDevice pickPhysicalDevice(VulkanData &vk_data) {
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(vk_data.instance, &deviceCount, nullptr);

  if (deviceCount == 0) {
    throw std::runtime_error("failed to find GPUs with Vulkan support!");
  }

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(vk_data.instance, &deviceCount, devices.data());
  VkPhysicalDevice candidate;
  bool found = false;
  for (const auto &device : devices) {
    if (isDeviceSuitable(device, vk_data)) {

      vk_data.msaaSamples = getMaxUsableSampleCount(device);
      VkPhysicalDeviceProperties deviceProperties;
      vkGetPhysicalDeviceProperties(device, &deviceProperties);
      std::string deviceName = deviceProperties.deviceName;
      std::transform(deviceName.begin(), deviceName.end(), deviceName.begin(), ::toupper);

      Log::Info(deviceName);
      Log::Info("\t* Max MSAA Samples: ", vk_data.msaaSamples);
      //if (deviceName.find("INTEL") != std::string::npos) {

      if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        physicalDevice = device;
        Log::Info("\t* Discrete GPU");
        found = true;
        break;
      }
    }
    candidate = device;
  }

  if (!found)
    physicalDevice = candidate;

  if (physicalDevice == VK_NULL_HANDLE) {
    throw std::runtime_error("failed to find a suitable GPU!");
  }
  vk_data.physicalDevice = physicalDevice;

  return physicalDevice;
}

static void cleanup(VulkanData &vk_data) {
  vkDestroySwapchainKHR(vk_data.logicalDevice, vk_data.swapChain, nullptr);

  for (auto framebuffer : vk_data.defaultRenderPass.renderPassChain.Framebuffers) {
    vkDestroyFramebuffer(vk_data.logicalDevice, framebuffer, nullptr);
  }

  for (auto imageView : vk_data.defaultRenderPass.renderPassChain.ImageViews) {
    vkDestroyImageView(vk_data.logicalDevice, imageView, nullptr);
  }

  vkDestroyImageView(vk_data.logicalDevice, vk_data.defaultRenderPass.renderPassChain.DepthTexture.imageView, nullptr);
  vkDestroyImage(vk_data.logicalDevice, vk_data.defaultRenderPass.renderPassChain.DepthTexture.image, nullptr);
  vkFreeMemory(vk_data.logicalDevice, vk_data.defaultRenderPass.renderPassChain.DepthTextureBuffer.deviceMemory, nullptr);

  if (enableValidationLayers) {
    DestroyDebugUtilsMessengerEXT(vk_data.instance, debugMessenger, nullptr);
  }

  vkDestroyDevice(vk_data.logicalDevice, nullptr);

  cleanupSurface(vk_data);
}
} // namespace vk