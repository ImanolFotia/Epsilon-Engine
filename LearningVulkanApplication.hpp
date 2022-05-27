#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <cstring>
#include <set>
#include <sstream>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "framework/common.hpp"
#include "framework/def.hpp"
#include "framework/window.hpp"

#include "engine/renderers/vulkan.hpp"

namespace LearningVulkan
{
    class LearningVulkanApplication
    {

        uint32_t m_CurrentFrame = 0;
        framework::Window m_Window;
        std::string m_ApplicationName = "Default";

        std::unique_ptr<engine::Renderer> m_pRenderer;

        uint32_t nbFrames = 0;
        uint32_t lastTime = 0.0;

    public:
        LearningVulkanApplication() = default;

        LearningVulkanApplication(std::string appName) : m_ApplicationName(appName)
        {
            m_pRenderer = std::make_unique<engine::VulkanRenderer>();
        }

        void run()
        {
            initWindow();
            initVulkan();
            onCreate();
            mainLoop();
            exit();
        }

        virtual void onCreate() = 0;
        virtual void onRender() = 0;
        virtual void onExit() = 0;

    protected:
        bool mShouldClose = false;
        void ShouldClose()
        {
            mShouldClose = true;
        }

    private:
        void initWindow()
        {
            m_Window.init(m_ApplicationName, 1280, 720);
        }

        void initVulkan()
        {
            m_pRenderer->Init(m_ApplicationName.c_str(), m_Window);
        }

        void mainLoop()
        {
            while (!m_Window.ShouldClose())
            {
                showFPS();
                if (mShouldClose)
                    break;
                onRender();
                drawFrame();
                m_Window.PollEvents();

            }
        }

        void drawFrame()
        {
            m_pRenderer->Begin();

            m_pRenderer->Flush();

            m_pRenderer->End();
        }

        void showFPS()
        {
            // Measure speed
            double currentTime = glfwGetTime();
            double delta = currentTime - lastTime;
            nbFrames++;
            if (delta >= 1.0)
            {
                double fps = double(nbFrames) / delta;

                std::stringstream ss;
                ss << m_ApplicationName << " | " << " [" << (int)fps << " FPS] | [" << (1000.0 / double(nbFrames)) << " MS]";

                m_Window.setWindowTitle(ss.str().c_str());

                nbFrames = 0;
                lastTime = currentTime;
            }
        }

        void exit()
        {
            onExit();
            m_pRenderer->Cleanup();
            m_Window.cleanup();
        }

    protected:
        uint32_t Submit(const std::vector<engine::Vertex> &vertices, const std::vector<uint32_t> &indices, const engine::MaterialInfo &materialInfo)
        {
            return m_pRenderer->Submit(vertices, indices, materialInfo);
        }

        void Draw(uint32_t object_id)
        {
            m_pRenderer->Push(object_id);
        }
    };
}