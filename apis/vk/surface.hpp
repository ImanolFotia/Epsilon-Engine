#pragma once

#include <vulkan/vulkan.hpp>

#if USE_GLFW

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#endif

#ifdef _WIN32
#include <windows.h>
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#ifdef BUILD_ANDROID
    #undef USE_GLFW
    #include <jni.h>
#endif

#include "vk_data.hpp"

#include <core/framework/window.hpp>


namespace vk {

    static void createSurface(VulkanData &vk_data, framework::Window::windowType *window) {

#if USE_GLFW

        if (glfwCreateWindowSurface(vk_data.instance, window, nullptr, &vk_data.surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
#elif _WIN32
        VkWin32SurfaceCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createInfo.hwnd = glfwGetWin32Window(window);
        createInfo.hinstance = GetModuleHandle(nullptr);

        if (vkCreateWin32SurfaceKHR(vk_data.instance, &vk_data.createInfo, nullptr, &vk_data.surface) != VK_SUCCESS)
        {
                throw std::runtime_error("failed to create window surface!");
        }
#elif BUILD_ANDROID
        VkAndroidSurfaceCreateInfoKHR surface_info;
        surface_info.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
        surface_info.pNext = NULL;
        surface_info.flags = 0;
        surface_info.window = mWindow;
        if(vkCreateAndroidSurfaceKHR(instance, &surface_info, NULL, &surface) != VK_SUCCESS) {
          throw std::runtime_error("failed to create window surface!");
        }
#endif
    }

    static void cleanupSurface(const VulkanData &vk_data) {
        vkDestroySurfaceKHR(vk_data.instance, vk_data.surface, nullptr);
        vkDestroyInstance(vk_data.instance, nullptr);
    }

}