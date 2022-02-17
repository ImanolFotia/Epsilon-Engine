#pragma once

#include <vulkan/vulkan.h>

#define USE_GLFW 1

namespace LearningVulkan
{
        VkSurfaceKHR surface;

        void createSurface(const VkInstance &instance, GLFWwindow *window)
        {
#if USE_GLFW
                if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
                {
                        throw std::runtime_error("failed to create window surface!");
                }

#else
                VkWin32SurfaceCreateInfoKHR createInfo{};
                createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
                createInfo.hwnd = glfwGetWin32Window(window);
                createInfo.hinstance = GetModuleHandle(nullptr);

                if (vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface) != VK_SUCCESS)
                {
                        throw std::runtime_error("failed to create window surface!");
                }

#endif
        }

        void cleanupSurface(const VkInstance &instance)
        {
                vkDestroySurfaceKHR(instance, surface, nullptr);
                vkDestroyInstance(instance, nullptr);
        }

}