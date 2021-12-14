#pragma once

#include <pch.hpp>

#include <Core.hpp>

#include <ProgramData.h>
#include <Texture.h>
#include <Log.h>

#include <Platform/WindowBase.hpp>
#include <Platform/Windows/Window.hpp>
#include <Platform/Linux/Window.hpp>

#include <GLFW/glfw3.h>



#ifdef _WIN32
extern "C"
{
    __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 0x00000001;
}
#endif

//Should force the OS to use the dedicated GPU, however, it doesn't work while using hybrid graphics on Pop!_OS, so the command must be run using:
//For OpenGL:
//__NV_PRIME_RENDER_OFFLOAD=1 __GLX_VENDOR_LIBRARY_NAME=nvidia <application>
//For Vulkan:
//__NV_PRIME_RENDER_OFFLOAD=1 <application>

#ifdef __linux__
extern "C"
{
    int NvOptimusEnablement = 1;
    int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

namespace Epsilon
{
    
    std::shared_ptr<Platform::WindowBase> Init(const char *ProgramName)
    {
        Log::OpenFile("log.txt");

        Log::WriteToLog("Initializing Epsilon Engine");

        ProgramData DATA;

        int lWindowWidth = DATA.WINDOW_WIDTH;
        int lWindowHeight = DATA.WINDOW_HEIGHT;
        
        auto & ref = Engine::Get();

        if(!ref.hasArgument(ref.ArgumentNames.width)) ref.Width(lWindowWidth);
        
        if(!ref.hasArgument(ref.ArgumentNames.height)) ref.Height(lWindowHeight);

        std::shared_ptr<Platform::WindowBase> lWindow;
#ifdef _WIN32
        lWindow = std::make_shared<Platform::Windows::Window>(API::CONTEXT_TYPE::OGL);
#elif defined(__linux__)
        lWindow = std::make_shared<Platform::Linux::Window>(API::CONTEXT_TYPE::OGL);
#endif

        Platform::WindowData wData = Platform::WindowData();

        wData.CurrentMonitor = wData.CurrentMonitor;
        wData.Width = ref.Width();
        wData.Height = ref.Height();
        wData.State = DATA.FULLSCREEN;
        wData.Title = ProgramName;
        wData.VSync = DATA.VSYNC;

        lWindow->setWindowData(wData);
        lWindow->Init(ProgramName, ref.Width(), ref.Height());

        return lWindow;
    }
} // namespace Epsilon