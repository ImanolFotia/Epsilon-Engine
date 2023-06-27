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

#include <core/engine/scene/scene.hpp>

namespace Epsilon
{
    class EPSILON_API Epsilon
    {

        uint32_t m_CurrentFrame = 0;

    protected:
        uint32_t nbFrames = 0;
        uint32_t lastTime = 0;
        int32_t m_pFrame = 0;

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
            alignas(8) glm::vec3 viewPosition{};
            alignas(16) glm::mat4 view{};
            alignas(16) glm::mat4 proj{};
            alignas(16) glm::mat4 lightMatrix{};
            alignas(4) uint32_t iFrame {};
            alignas(8) glm::ivec2 iMouse{};
        };

    public:
        std::shared_ptr<engine::Context> getContext();

        Epsilon() = default;

        explicit Epsilon(const std::string &appName);

        void run();

        std::function<void()> onCreate;
        std::function<void()> onReady;
        std::function<void()> onUpdate;
        std::function<void()> onRender;
        std::function<void()> onExit;

        std::string m_ApplicationName;

        int getMaxFPS() { return m_pMaxFPS; }
        void setMaxFPS(int fps) { m_pMaxFPS = fps; }

    protected:
        bool mShouldClose = false;

        void ShouldClose();
        void drawFrame(engine::Ref<engine::RenderPass> renderPassRef);
        void setOnCreate(std::function<void(void)> fun);
        void setOnUpdate(std::function<void(void)> fun);
        void setOnReady(std::function<void(void)> fun);
        void setOnRender(std::function<void(void)> fun);
        int32_t Frame();

        void setOnExit(std::function<void(void)> fun);


    private:
        void initWindow(int w, int h);
        void initVulkan();

        void mainLoop();
        void showFPS();

        void exit();

    protected:
        std::pair<int, int> getWindowDimensions();

        std::shared_ptr<engine::Context> m_pContext = nullptr;
        engine::Scene m_pScene;

        ShaderData shaderData;
        int m_pMaxFPS = 5000;
    };
}
