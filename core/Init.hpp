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

        Engine::Get().Width(lWindowWidth);
        Engine::Get().Height(lWindowHeight);

        std::shared_ptr<Platform::WindowBase> lWindow;
        #ifdef _WIN32
            lWindow = std::make_shared<Platform::Windows::Window>();
        #elif defined(__linux__)
            lWindow = std::make_shared<Platform::Linux::Window>();
        #endif
        
        Platform::WindowData wData = Platform::WindowData();

        wData.CurrentMonitor = wData.CurrentMonitor;
        wData.Width = lWindowWidth;
        wData.Height = lWindowHeight;
        wData.State = DATA.FULLSCREEN;
        wData.Title = ProgramName;
        wData.VSync = DATA.VSYNC;

        lWindow->setWindowData(wData);
        lWindow->Init(ProgramName, lWindowWidth, lWindowHeight);

        return lWindow;
    }
} // namespace Epsilon