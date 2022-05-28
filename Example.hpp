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
            const std::vector<engine::Vertex> vertices = {
                {glm::vec3(-1.0f, -1.0f, 0.0f),
                 glm::vec2(0.0f, 1.0f),
                 glm::vec3(0.0f, 0.0f, 0.0f),
                 glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
                 glm::vec3(0.0f, 0.0f, 0.0f),
                 glm::vec3(0.0f, 0.0f, 0.0f)},
                {glm::vec3(1.0f, -1.0f, 0.0f),
                 glm::vec2(0.0f, 0.0f),
                 glm::vec3(0.0f, 0.0f, 0.0f),
                 glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
                 glm::vec3(0.0f, 0.0f, 0.0f),
                 glm::vec3(0.0f, 0.0f, 0.0f)},
                {glm::vec3(1.0f, 1.0f, 0.0f),
                 glm::vec2(1.0f, 0.0f),
                 glm::vec3(0.0f, 0.0f, 0.0f),
                 glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
                 glm::vec3(0.0f, 0.0f, 0.0f),
                 glm::vec3(0.0f, 0.0f, 0.0f)},
                {glm::vec3(-1.0f, 1.0f, 0.0f),
                 glm::vec2(1.0f, 0.0f),
                 glm::vec3(0.0f, 0.0f, 0.0f),
                 glm::vec4(1.0f, 0.0f, 1.0f, 1.0f),
                 glm::vec3(0.0f, 0.0f, 0.0f),
                 glm::vec3(0.0f, 0.0f, 0.0f)}};

            const std::vector<uint32_t> indices = {
                0, 1, 2, 2, 3, 0};

            const std::vector<engine::Vertex> vertices2 = {
                {glm::vec3(0.5f, -0.5f, 0.0f),
                 glm::vec2(0.0f, 1.0f),
                 glm::vec3(0.0f, 0.0f, 0.0f),
                 glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
                 glm::vec3(0.0f, 0.0f, 0.0f),
                 glm::vec3(0.0f, 0.0f, 0.0f)},
                {glm::vec3(0.5f, 0.0f, 0.0f),
                 glm::vec2(0.0f, 0.0f),
                 glm::vec3(0.0f, 0.0f, 0.0f),
                 glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
                 glm::vec3(0.0f, 0.0f, 0.0f),
                 glm::vec3(0.0f, 0.0f, 0.0f)},
                {glm::vec3(-1.0f, 0.5f, 0.0f),
                 glm::vec2(1.0f, 0.0f),
                 glm::vec3(0.0f, 0.0f, 0.0f),
                 glm::vec4(0.0f, 1.0f, 1.0f, 1.0f),
                 glm::vec3(0.0f, 0.0f, 0.0f),
                 glm::vec3(0.0f, 0.0f, 0.0f)}};

            myObjectId = Submit(vertices, indices, {});
            //myObjectId2 = Submit(vertices2, {});
        }

        void onRender() override
        {
             Draw(myObjectId);
            //Draw(myObjectId2);
        }

        void onExit() override
        {
        }

    private:
        uint32_t myObjectId = 0;
        uint32_t myObjectId2 = 0;
    };
}