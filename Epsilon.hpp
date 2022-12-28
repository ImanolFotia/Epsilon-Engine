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
#include "framework/env.hpp"
#include "framework/utils/image.hpp"
#include "framework/utils/file.hpp"

#include "engine/context.hpp"

#include "engine/renderer/vulkan/vulkan.hpp"

#include "engine/renderer/resource_manager.hpp"
#include "engine/renderer/vulkan/resource_manager.hpp"

namespace Epsilon {
    class Epsilon {

        uint32_t m_CurrentFrame = 0;



    protected:
        engine::Context m_pContext;
        std::string m_ApplicationName;

        uint32_t nbFrames = 0;
        uint32_t lastTime = 0;
        engine::Ref<engine::RenderPass> renderPassRef;

        struct MeshPushConstant {
            alignas(16) glm::mat4 model;
            alignas(16) uint32_t piece;
        };

        struct ShaderData
        {
            alignas(4) float iTime = 0.0f;
            alignas(8) glm::vec2 iResolution{};
            alignas(16) glm::mat4 view{};
            alignas(16) glm::mat4 proj{};
        };
    public:
        Epsilon() = default;

        explicit Epsilon(const std::string &appName) : m_ApplicationName(appName) {
            m_pContext.Init(appName, engine::renderer_type::vulkan);
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

    private:
        void initWindow() {
            m_pContext.Window().init(m_ApplicationName, 1280, 720);
        }

        void initVulkan() {
            m_pContext.Renderer()->Init(m_ApplicationName.c_str(), m_pContext.Window());
            m_pContext.ResourceManager()->Init();
            using namespace engine;
/*
            auto vertexCode = utils::readFile("../assets/shaders/vertex.spv");
            auto fragmentCode = utils::readFile("../assets/shaders/fragment.spv");



            auto boardVertexCode = utils::readFile("../assets/shaders/board-vertex.spv");
            auto boardFragmentCode = utils::readFile("../assets/shaders/board-fragment.spv");

            ShaderInfo shaderInfo = {
                    .stages = {
                            {.entryPoint = "main", .shaderCode = boardVertexCode, .stage = VERTEX},
                            {.entryPoint = "main", .shaderCode = boardFragmentCode, .stage = FRAGMENT}},
                    .usedStages = ShaderModuleStage(VERTEX | FRAGMENT)};


            ShaderInfo boardShaderInfo = {
                    .stages = {
                            {.entryPoint = "main", .shaderCode = vertexCode, .stage = VERTEX},
                            {.entryPoint = "main", .shaderCode = fragmentCode, .stage = FRAGMENT}},
                    .usedStages = ShaderModuleStage(VERTEX | FRAGMENT)};

            RenderPassInfo renderPassInfo =
                    RenderPassFactory()
                            .numDescriptors(6)
                            .size(sizeof(Vertex))
                            .depthAttachment(true)
                            .subpasses({})
                            .vertexLayout({{XYZ_FLOAT,  offsetof(Vertex, position)},
                                           {XY_FLOAT,   offsetof(Vertex, texCoords)},
                                           {XYZ_FLOAT,  offsetof(Vertex, normal)},
                                           {XYZW_FLOAT, offsetof(Vertex, color)},
                                           {XYZ_FLOAT,  offsetof(Vertex, tangent)},
                                           {XYZ_FLOAT,  offsetof(Vertex, bitangent)}})
                            .attachments(
                                    {
                                            {
                                                    .format = COLOR_RGBA,
                                                    .isDepthAttachment = false,
                                                    .isSwapChainAttachment = true
                                            },
                                            {
                                                    .format = DEPTH_F32_STENCIL_8,
                                                    .isDepthAttachment = true
                                            }
                                    })
                            .pipelineLayout( {.shaderInfo = shaderInfo})
                            .pipelineLayout( {.shaderInfo = boardShaderInfo})
                            .pushConstant(sizeof(MeshPushConstant))
                            .bufferInfo({.size = sizeof(ShaderData), .offset = 0});

            renderPassRef = m_pContext.ResourceManager()->createDefaultRenderPass(renderPassInfo);
*/
        }

        void mainLoop() {
            while (!m_pContext.Window().ShouldClose()) {
                showFPS();
                if (mShouldClose)
                    break;
                onRender();
                drawFrame();
                m_pContext.Window().PollEvents();
            }
        }

        void drawFrame() {
            m_pContext.Renderer()->Begin(renderPassRef);
            m_pContext.Renderer()->Flush();
            m_pContext.Renderer()->End();
        }

        void showFPS() {
            // Measure speed
            double currentTime = glfwGetTime();
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
            glfwGetWindowSize(m_pContext.Window().getWindow(), &w, &h);
            return {w, h};
        }

        ShaderData shaderData;
    };
}