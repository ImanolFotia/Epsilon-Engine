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

namespace Epsilon {
    class Epsilon {

        uint32_t m_CurrentFrame = 0;



    protected:
        engine::Context m_pContext;
        std::string m_ApplicationName;

        uint32_t nbFrames = 0;
        uint32_t lastTime = 0;

        struct MeshPushConstant {
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
        Epsilon() = default;

        explicit Epsilon(const std::string &appName) : m_ApplicationName(appName) {
            //try {
                m_pContext.Init(appName, engine::renderer_type::vulkan);
          /*  } catch(std::exception& e) {
                std::cout << e.what() << std::endl;
                std::exit(255);
            }*/
        }

        void run() {
            initWindow();
            initVulkan();
            onCreate();
            onReady();
            mainLoop();
            exit();
        }

        virtual void onCreate() = 0;

        virtual void onReady() = 0;

        virtual void onRender() = 0;

        virtual void onExit() = 0;

    protected:
        bool mShouldClose = false;

        void ShouldClose() {
            mShouldClose = true;
        }

        void drawFrame(engine::Ref<engine::RenderPass> renderPassRef) {
            //m_pContext.Renderer()->Begin(renderPassRef);
            m_pContext.Renderer()->Flush(renderPassRef);
            m_pContext.Renderer()->End();
            m_pContext.Renderer()->Sync();
        }

    private:
        void initWindow() {
            m_pContext.Window().init(m_ApplicationName, 1280, 720);
        }

        void initVulkan() {
            m_pContext.Renderer()->Init(m_ApplicationName.c_str(), m_pContext.Window());
            m_pContext.ResourceManager()->Init();
        }

        void mainLoop() {
            while (!m_pContext.Window().ShouldClose()) {
                framework::Clock::Tick();
                showFPS();
                if (mShouldClose)
                    break;
                onRender();
                m_pContext.Window().PollEvents();
            }
        }


        void showFPS() {
            // Measure speed
            double currentTime = framework::Clock::TimeSeconds();
            double delta = currentTime - lastTime;
            nbFrames++;
            if (delta >= 1.0) {
                double fps = double(nbFrames) / delta;

                std::stringstream ss;
                ss << m_ApplicationName << " | "
                   << " [" << (int) fps << " FPS] | [" << (1000.0 / double(nbFrames)) << " MS]";

                m_pContext.Window().setWindowTitle(ss.str().c_str());

                nbFrames = 0;
                lastTime = currentTime;
            }
        }

        void exit() {
            onExit();
            m_pContext.Renderer()->Cleanup();
            m_pContext.Window().cleanup();
        }

    protected:

        engine::Renderer::TexturesDataId RegisterTexture(unsigned char *data, engine::TextureInfo info) {
            return m_pContext.Renderer()->RegisterTexture(data, info);
        }

        void Draw(engine::RenderObject object_id) {
            //m_pContext.Renderer()->Push(object_id);
        }

        void PushShaderData(ShaderData data) {
            shaderData = data;
            std::static_pointer_cast<engine::VulkanRenderer>(m_pContext.Renderer())->perPassData = &shaderData;
        }

        std::pair<int, int> getWindowDimensions() {
            int w, h;
            return m_pContext.Window().getSize();
        }

        ShaderData shaderData;
    };
}