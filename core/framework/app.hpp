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
#include "glm/glm.hpp"

#include "common.hpp"
#include "def.hpp"
#include "window.hpp"
#include "env.hpp"

#include "../engine/renderer/vulkan.hpp"

namespace engine
{
    class App
    {

        uint32_t m_CurrentFrame = 0;
        framework::Window m_Window;

        std::string m_ApplicationName = "Default";

        std::unique_ptr<Renderer> m_pRenderer;

        uint32_t nbFrames = 0;
        uint32_t lastTime = 0.0;

    public:
        App() = default;

        App(std::string appName) : m_ApplicationName(appName)
        {
            m_pRenderer = std::make_unique<VulkanRenderer>();
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
        Renderer::ObjectDataId RegisterMesh(const std::vector<Vertex> &vertices, std::vector<uint32_t> &indices, bool group)
        {
            return m_pRenderer->RegisterMesh(vertices, indices, group);
        }


        Renderer::TexturesDataId RegisterTexture(unsigned char* data, TextureInfo info)
        {
            return m_pRenderer->RegisterTexture(data, info);
        }

        void Draw(engine::RenderObject object_id)
        {
            m_pRenderer->Push(object_id);
        }

        void PushCameraData(const ShaderData& camData) {
            m_pRenderer->PushCameraData(camData);
        }

        std::pair<int, int> getWindowDimensions() {
            int w, h;
            glfwGetWindowSize(m_Window.getWindow(), &w, &h);
            return {w, h};
        }


    };
}