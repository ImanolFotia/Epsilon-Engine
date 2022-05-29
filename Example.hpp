#pragma once
#define GLM_FORCE_RADIANS

#include "LearningVulkanApplication.hpp"

#include <glm/gtc/matrix_transform.hpp>

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
            // myObjectId2 = Submit(vertices2, {});
        }

        void onRender() override
        {
            engine::CameraData camData;
            static auto startTime = std::chrono::high_resolution_clock::now();

            auto currentTime = std::chrono::high_resolution_clock::now();
            float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

            camData.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            camData.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            camData.proj = glm::perspective(glm::radians(45.0f), getWindowDimensions().first / (float) getWindowDimensions().second, 0.1f, 10.0f);
            camData.proj[1][1] *= -1;
            
            PushCameraData(camData);
            Draw(myObjectId);
            // Draw(myObjectId2);
        }

        void onExit() override
        {
        }

    private:
        uint32_t myObjectId = 0;
        uint32_t myObjectId2 = 0;
    };
}