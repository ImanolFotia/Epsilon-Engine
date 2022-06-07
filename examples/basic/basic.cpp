#define GLM_FORCE_RADIANS

#include <framework/app.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <engine/renderer/drawables/primitives/cube.hpp>

namespace ExampleApp
{
    class ExampleApp : public engine::App
    {
    public:
        ExampleApp(std::string appName) : engine::App(appName) {}

        void onCreate() override
        {
            const std::vector<engine::Vertex> vertices = {
                {glm::vec3(-1.0f, -1.0f, 0.0f),
                 glm::vec2(0.0f, 0.0f),
                 glm::vec3(0.0f, 0.0f, 1.0f),
                 glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
                 glm::vec3(0.0f, 0.0f, 0.0f),
                 glm::vec3(0.0f, 0.0f, 0.0f)},
                {glm::vec3(1.0f, -1.0f, 0.0f),
                 glm::vec2(1.0f, 0.0f),
                 glm::vec3(0.0f, .0f, 1.0f),
                 glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
                 glm::vec3(0.0f, 0.0f, 0.0f),
                 glm::vec3(0.0f, 0.0f, 0.0f)},
                {glm::vec3(1.0f, 1.0f, 0.0f),
                 glm::vec2(1.0f, 1.0f),
                 glm::vec3(0.0f, 0.0f, 1.0f),
                 glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
                 glm::vec3(0.0f, 0.0f, 0.0f),
                 glm::vec3(0.0f, 0.0f, 0.0f)},
                {glm::vec3(-1.0f, 1.0f, 0.0f),
                 glm::vec2(0.0f, 1.0f),
                 glm::vec3(0.0f, 0.0f, 1.0f),
                 glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
                 glm::vec3(0.0f, 0.0f, 0.0f),
                 glm::vec3(0.0f, 0.0f, 0.0f)}};

            std::vector<uint32_t> indices = {
                0, 1, 2, 2, 3, 0};

            myObjectId = RegisterMesh(vertices, indices, {}, false);

            auto cube_data = m_pCube.data();
            CubeId = RegisterMesh(cube_data.Vertices, cube_data.Indices, {}, false);
        }

        void onRender() override
        {
            engine::ShaderData camData;
            camData.iResolution = glm::vec2(getWindowDimensions().first, getWindowDimensions().second);
            static auto startTime = std::chrono::high_resolution_clock::now();

            auto currentTime = std::chrono::high_resolution_clock::now();
            float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

            camData.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            camData.proj = glm::perspective(glm::radians(45.0f), getWindowDimensions().first / (float) getWindowDimensions().second, 0.1f, 10.0f);
            camData.proj[1][1] *= -1;
            camData.iTime += time;

            PushCameraData(camData);

            myObjectId->push_constant.model = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f)); 
            Draw(myObjectId);

            CubeId->push_constant.model = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(.0f, .0f, 1.0f));
            CubeId->push_constant.model = glm::scale(CubeId->push_constant.model, glm::vec3(0.5));
            
            Draw(CubeId);
        }

        void onExit() override
        {
        }

    private:
        engine::Renderer::ObjectDataId myObjectId = {};

        engine::Cube m_pCube;
        engine::Renderer::ObjectDataId CubeId = {};
    };
}

/*
    Just my adventures learning vulkan, primarily based on https://vulkan-tutorial.com/
    The makefile is for MSYS2 MingW
*/

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
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

int main(int argc, char **argv)
{
    framework::env::setArgs(argc, argv);

    ExampleApp::ExampleApp app("Vulkan Engine");

    try
    {
        app.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}