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
            Submit(vertices, {});
        }

        void onRender() override
        {
        }

        void onExit() override
        {
        }

    private:
        const std::vector<engine::Vertex> vertices = {
            {glm::vec3(0.0f, -0.5f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f)},
            {glm::vec3(0.5f, 0.5f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f)},
            {glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f)}};
    };
}