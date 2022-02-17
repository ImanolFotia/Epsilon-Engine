#pragma once

#include "LearningVulkanApplication.hpp"

namespace ExampleApp
{
    class ExampleApp : public LearningVulkan::LearningVulkanApplication
    {
    public:
        ExampleApp(std::string appName) : LearningVulkan::LearningVulkanApplication(appName) {}

        void onCreate() override
        {

        }

        void onRender() override
        {
            std::cout << "Hello Vulkan App" << std::endl;
            ShouldClose();
        }
        
        void onExit() override {

        }
    };
}