#pragma once

#include "LearningVulkanApplication.hpp"

namespace ExampleApp
{
    class ExampleApp : public LearningVulkan::LearningVulkanApplication
    {
    public:
        ExampleApp(std::string appName) : LearningVulkan::LearningVulkanApplication(appName) {}

        virtual void Loop()
        {
            std::cout << "Hello Vulkan App" << std::endl;
            ShouldClose();
        }
    };
}