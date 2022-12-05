#pragma once

#include <vulkan/vulkan.hpp>

#include <GLFW/glfw3.h>
#include "vk_data.hpp"

#define USE_GLFW 1

namespace vk
{

        static void createSurface(VulkanData& vk_data, GLFWwindow *window)
        {
#if USE_GLFW
                if (glfwCreateWindowSurface(vk_data.instance, window, nullptr, &vk_data.surface) != VK_SUCCESS)
                {
                        throw std::runtime_error("failed to create window surface!");
                }

#else
                VkWin32SurfaceCreateInfoKHR createInfo{};
                createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
                createInfo.hwnd = glfwGetWin32Window(window);
                createInfo.hinstance = GetModuleHandle(nullptr);

                if (vkCreateWin32SurfaceKHR(vk_data.instance, &vk_data.createInfo, nullptr, &vk_data.surface) != VK_SUCCESS)
                {
                        throw std::runtime_error("failed to create window surface!");
                }

#endif
        }

        static void cleanupSurface(const VulkanData& vk_data)
        {
                vkDestroySurfaceKHR(vk_data.instance, vk_data.surface, nullptr);
                vkDestroyInstance(vk_data.instance, nullptr);
        }

}