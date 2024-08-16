#pragma once

#if !defined(__ANDROID__)
#include <vulkan/vulkan.hpp>
#endif

#if USE_GLFW

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#endif

#ifdef _WIN32
#include <windows.h>
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#if defined(ANDROID) || defined(__ANDROID__)
#undef USE_GLFW
#include <jni.h>
#endif

#include "vk_data.hpp"

#include <core/framework/window.hpp>
#include <core/framework/log.hpp>

namespace vk
{

    static void createSurface(VulkanData &vk_data, framework::Window::windowType *window)
    {

#if USE_GLFW

        if (auto result = glfwCreateWindowSurface(vk_data.instance, window, nullptr, &vk_data.surface); result != VK_SUCCESS)
        {
            Log::Error("Error code: ", result);
            throw std::runtime_error("failed to create window surface!");
        }
#elif _WIN32
        VkWin32SurfaceCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createInfo.hwnd = glfwGetWin32Window(window);
        createInfo.hinstance = GetModuleHandle(nullptr);

        if (auto result = vkCreateWin32SurfaceKHR(vk_data.instance, &vk_data.createInfo, nullptr, &vk_data.surface); result != VK_SUCCESS)
        {
            Log::Error("Error code: ", result);
            throw std::runtime_error("failed to create window surface!");
        }
#elif defined(__ANDROID__)
        VkAndroidSurfaceCreateInfoKHR surface_info;
        surface_info.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
        surface_info.pNext = NULL;
        surface_info.flags = 0;
        surface_info.window = window;
        if (auto result = vkCreateAndroidSurfaceKHR(vk_data.instance, &surface_info, NULL, &vk_data.surface); result != VK_SUCCESS)
        {
            Log::Error("Error code: ", result);
            throw std::runtime_error("failed to create window surface!");
        }
#endif
    }

    static void cleanupSurface(const VulkanData &vk_data)
    {
        vkDestroySurfaceKHR(vk_data.instance, vk_data.surface, nullptr);
        vkDestroyInstance(vk_data.instance, nullptr);
    }

}