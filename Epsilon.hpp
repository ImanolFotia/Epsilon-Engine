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

#include "core/framework/common.hpp"
#include "core/framework/def.hpp"
#include "core/framework/window.hpp"
#include "core/framework/env.hpp"
#include "core/framework/utils/image.hpp"
#include "core/framework/utils/file.hpp"
#include "core/framework/clock.hpp"

#include "core/engine/context.hpp"

#include "core/engine/renderer/vulkan/vulkan.hpp"

#include "core/engine/renderer/resource_manager.hpp"
#include "core/engine/renderer/vulkan/resource_manager.hpp"
#include "modules/mono/mono-instance.hpp"

namespace Epsilon
{
    class __attribute__((visibility("default"))) Epsilon : public singleton<Epsilon>
    {

        uint32_t m_CurrentFrame = 0;

    protected:
        uint32_t nbFrames = 0;
        uint32_t lastTime = 0;

        struct MeshPushConstant
        {
            alignas(16) glm::mat4 model;
            alignas(16) uint32_t piece;
        };

        struct ShaderData
        {
            alignas(4) float iTime = 0.0f;
            alignas(8) glm::vec2 iResolution{};
            alignas(8) glm::vec3 lightPosition{};
            alignas(16) glm::mat4 view{};
            alignas(16) glm::mat4 proj{};
            alignas(16) glm::mat4 lightMatrix{};
        };

    public:
        engine::Context &getContext()
        {
            return engine::Context::getSingleton();
        }

        Epsilon() = default;

        explicit Epsilon(const std::string &appName);

        void run()
        {
            if (engine::Context::getSingleton().Window().getSize().first == 0)
                self.initWindow(1280, 720);

            self.initVulkan();
            if (self.onCreate)
                self.onCreate();

            if (self.onReady)
                self.onReady();
            self.mainLoop();
            self.exit();
        }

        std::function<void()> onCreate;
        std::function<void()> onReady;
        std::function<void()> onRender;
        std::function<void()> onExit;

        std::string m_ApplicationName;

    protected:
        bool mShouldClose = false;

        void ShouldClose()
        {
            mShouldClose = true;
        }

        void drawFrame(engine::Ref<engine::RenderPass> renderPassRef)
        {
            engine::Context::getSingleton().Renderer()->Flush(renderPassRef, engine::DrawType::INDEXED);
            engine::Context::getSingleton().Renderer()->End();
            engine::Context::getSingleton().Renderer()->Sync();
        }

        void setOnCreate(std::function<void(void)> fun)
        {
            Epsilon::getSingleton().onCreate = fun;
        }

        void setOnReady(std::function<void(void)> fun)
        {
            Epsilon::getSingleton().onReady = fun;
        }

        void setOnRender(std::function<void(void)> fun)
        {
            Epsilon::getSingleton().onRender = fun;
        }

        void setOnExit(std::function<void(void)> fun)
        {
            Epsilon::getSingleton().onExit = fun;
        }

    private:
        void initWindow(int w, int h)
        {
            engine::Context::getSingleton().Window().init(m_ApplicationName, w, h);
        }

        void initVulkan()
        {
            engine::Context::getSingleton().Renderer()->Init(m_ApplicationName.c_str(), engine::Context::getSingleton().Window());
            engine::Context::getSingleton().ResourceManager()->Init();
        }

        void mainLoop()
        {

            while (!engine::Context::getSingleton().Window().ShouldClose())
            {
                framework::Clock::Tick();
                showFPS();
                if (mShouldClose)
                    break;
                if (self.onRender)
                    self.onRender();
                engine::Context::getSingleton().Window().PollEvents();
            }
        }

        void showFPS()
        {
            // Measure speed
            double currentTime = framework::Clock::TimeSeconds();
            double delta = currentTime - lastTime;
            nbFrames++;
            if (delta >= 1.0)
            {
                double fps = double(nbFrames) / delta;

                std::stringstream ss;
                ss << m_ApplicationName << " | "
                   << " [" << (int)fps << " FPS] | [" << (1000.0 / double(nbFrames)) << " MS]";

                engine::Context::getSingleton().Window().setWindowTitle(ss.str().c_str());

                nbFrames = 0;
                lastTime = currentTime;
            }
        }

        void exit()
        {
            if (self.onExit)
                self.onExit();
            engine::Context::getSingleton().Renderer()->Cleanup();
            engine::Context::getSingleton().Window().cleanup();
        }

    protected:
        std::pair<int, int> getWindowDimensions()
        {
            int w, h;
            return engine::Context::getSingleton().Window().getSize();
        }

        ShaderData shaderData;
    };
}
