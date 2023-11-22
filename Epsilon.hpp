#ifndef EPSILON_HPP
#define EPSILON_HPP
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



#if defined(_WIN32)
//  Microsoft
#if defined(EPSILON_BUILD_DLL)
#define EXPORT __declspec(dllexport)
#else
#define IMPORT __declspec(dllimport)
#endif
#elif defined(__linux__)
//  GCC
#if defined(EPSILON_BUILD_DLL)
#define EXPORT __attribute__((visibility("default")))
#else
#define IMPORT
#endif
#else
//  do nothing and hope for the best?
#define EXPORT
#define IMPORT
#pragma warning Unknown dynamic link import / export semantics.
#endif




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
            alignas(16) glm::mat4 view = glm::mat4(1.0f);
            alignas(16) glm::mat4 proj = glm::mat4(1.0f);
            alignas(16) glm::mat4 prev_view = glm::mat4(1.0f);
            alignas(16) glm::mat4 prev_proj = glm::mat4(1.0f);
            alignas(16) glm::mat4 lightMatrix = glm::mat4(1.0f);
            alignas(4) uint32_t iFrame = 0;
            alignas(8) glm::ivec2 iMouse = glm::ivec2(0,0);
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


    public:
        void initWindow(int w, int h);
        void initVulkan();

        void mainLoop();
        void showFPS();

        void exit();

    public:
        std::shared_ptr<engine::Scene> getScene() {
            return m_pScene;
        }

    protected:
        std::pair<int, int> getWindowDimensions();

        std::shared_ptr<engine::Context> m_pContext = nullptr;
        std::shared_ptr<engine::Scene> m_pScene;

        ShaderData shaderData;
        int m_pMaxFPS = 5000;
    };
}

#endif //EPSILON_HPP

#ifdef IMPL_EPSILON_MAIN && BUILD_AS_LIBRARY == FALSE

int EpsilonMain(int argc, char** argv);
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#undef min
#undef max
#include <windows.h>
#include <fcntl.h>
#include <io.h>
#include <ShellScalingAPI.h>
extern "C"
{
    __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 0x00000001;
}
#endif

// Should force the OS to use the dedicated GPU, however, it doesn't work while using hybrid graphics on Pop!_OS, so the command must be run using:
// For OpenGL:
//__NV_PRIME_RENDER_OFFLOAD=1 __GLX_VENDOR_LIBRARY_NAME=nvidia <application>
// For Vulkan:
//__NV_PRIME_RENDER_OFFLOAD=1 <application>

#ifdef __linux__
extern "C"
{
    int NvOptimusEnablement = 1;
    int AmdPowerXpressRequestHighPerformance = 1;
}
#endif


//#if defined(_WIN32)																
//int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR argv, int argc)	
//#else
int main(int argc, char** argv)
//#endif
{

    framework::env::setArgs(argc, argv);


    return EpsilonMain(argc, argv);

}

#endif