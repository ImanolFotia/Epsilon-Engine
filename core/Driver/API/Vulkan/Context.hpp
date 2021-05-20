#pragma once

#include <cassert>

#include "Driver/API/Context.hpp"
#include <glfw/glfw3.h>

#include "Device.hpp"

#include <vulkan/vulkan.h>

namespace Epsilon::API::Vulkan
{
    class Context : public ContextBase
    {

        virtual void Init(CONTEXT_TYPE type) override
        {
            mType = type;
        }

        virtual void AttachContext(std::shared_ptr<Platform::WindowHandle<>> windowHandle)
        {

            mWindowHandle = windowHandle;

            initVulkan();
        }

        virtual void SwapBuffers()
        {
            assert(false);
        }

        virtual void Shutdown() override
        {
            vkDestroyInstance(instance, nullptr);

#ifndef EPSILON_STATIC
            if (mWindowHandle->getType() == Platform::WINDOW_HANDLE_TYPE::GLFW)
                glfwTerminate();
#endif
        }

    private:
        void initVulkan()
        {
            createInstance();
            pickPhysicalDevice(instance);
        }

        void createInstance()
        {
            if (enableValidationLayers && !checkValidationLayerSupport())
            {
                throw std::runtime_error("validation layers requested, but not available!");
            }
            VkApplicationInfo appInfo{};
            appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            appInfo.pApplicationName = mApplicationName.c_str();
            appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
            appInfo.pEngineName = "No Engine";
            appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
            appInfo.apiVersion = VK_API_VERSION_1_0;

            VkInstanceCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            createInfo.pApplicationInfo = &appInfo;

            uint32_t glfwExtensionCount = 0;
            const char **glfwExtensions;

            glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

            createInfo.enabledExtensionCount = glfwExtensionCount;
            createInfo.ppEnabledExtensionNames = glfwExtensions;
            createInfo.enabledLayerCount = 0;

            if (enableValidationLayers)
            {
                createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
                createInfo.ppEnabledLayerNames = validationLayers.data();
            }
            else
            {
                createInfo.enabledLayerCount = 0;
            }

            VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

            if(!result) {}

            if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create instance!");
            }

            checkExtensions();
        }

        void checkExtensions()
        {
            uint32_t extensionCount = 0;
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
            std::vector<VkExtensionProperties> extensions(extensionCount);
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
            std::cout << "available extensions:\n";

            for (const auto &extension : extensions)
            {
                std::cout << '\t' << extension.extensionName << '\n';
            }

            std::cout << std::endl;
        }

        bool checkValidationLayerSupport()
        {
            uint32_t layerCount;
            vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

            std::vector<VkLayerProperties> availableLayers(layerCount);
            vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

            for (const char *layerName : validationLayers)
            {
                bool layerFound = false;

                for (const auto &layerProperties : availableLayers)
                {
                    if (std::strcmp(layerName, layerProperties.layerName) == 0)
                    {
                        layerFound = true;
                        break;
                    }
                }

                if (!layerFound)
                {
                    return false;
                }
            }

            return true;
        }

    private:
        std::string mApplicationName = "Default";
        VkInstance instance;

        const std::vector<const char *> validationLayers = {
            "VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
        const bool enableValidationLayers = false;
#else
        const bool enableValidationLayers = true;
#endif
    };
}