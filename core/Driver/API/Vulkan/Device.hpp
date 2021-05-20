#pragma once

#include <stdint.h>
#include <string>

#include <Log.h>

#include <vulkan/vulkan.h>

namespace Epsilon::API::Vulkan
{
    static bool isDeviceSuitable(VkPhysicalDevice device)
    {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
        /* Log::WriteToLog("Device found: ", deviceProperties.deviceName, Log::CONSOLE);
            Log::WriteToLog("\tApi version: ", deviceProperties.apiVersion, Log::CONSOLE);
            Log::WriteToLog("\tID: ", deviceProperties.deviceID, Log::CONSOLE);
            Log::WriteToLog("\tDriver version: ", deviceProperties.driverVersion, Log::CONSOLE);
            Log::WriteToLog("\tVendor ID: ", deviceProperties.vendorID, Log::CONSOLE);*/

        return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
               deviceFeatures.geometryShader;
    }

    static void pickPhysicalDevice(VkInstance instance)
    {
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

        if (deviceCount == 0)
        {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        for (const auto &device : devices)
        {
            if (isDeviceSuitable(device))
            {
                physicalDevice = device;
                break;
            }
        }

        if (physicalDevice == VK_NULL_HANDLE)
        {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
    }

    class Device
    {
    public:
        Device() = default;

        bool isDeviceSuitable(VkPhysicalDevice device)
        {
            VkPhysicalDeviceProperties deviceProperties;
            vkGetPhysicalDeviceProperties(device, &deviceProperties);

            VkPhysicalDeviceFeatures deviceFeatures;
            vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
            /* Log::WriteToLog("Device found: ", deviceProperties.deviceName, Log::CONSOLE);
            Log::WriteToLog("\tApi version: ", deviceProperties.apiVersion, Log::CONSOLE);
            Log::WriteToLog("\tID: ", deviceProperties.deviceID, Log::CONSOLE);
            Log::WriteToLog("\tDriver version: ", deviceProperties.driverVersion, Log::CONSOLE);
            Log::WriteToLog("\tVendor ID: ", deviceProperties.vendorID, Log::CONSOLE);*/

            return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
                   deviceFeatures.geometryShader;
        }

    private:
        uint32_t Id;
        uint32_t mVendorID;
        std::string mName;
        uint32_t mScore;
    };
}