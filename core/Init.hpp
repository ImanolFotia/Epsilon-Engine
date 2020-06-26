#pragma once

#include <iostream>

#include <Core.hpp>

#include <GLFW/glfw3.h>

#include <ProgramData.h>
#include <Texture.h>
#include <Log.h>

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
    void error_callback(int error, const char *description)
    {
        fprintf(stderr, "Error: %s\n", description);
    }

    GLFWwindow *Init(const char *ProgramName)
    {

        Log::OpenFile("log.txt");

        Log::WriteToLog("Initializing Epsilon Engine");

        ProgramData DATA;

        if (!glfwInit())
        {
            Log::WriteToLog("GLFW could not be initialized");
        }
        glfwSetErrorCallback(error_callback);

        int lNumberOfMonitors = 0;

        GLFWmonitor **lMonitor = glfwGetMonitors(&lNumberOfMonitors);

        for (int i = 0; i < lNumberOfMonitors; ++i)
        {
            int numberofmodes;
            const GLFWvidmode *modes = glfwGetVideoModes(lMonitor[i], &numberofmodes);
            int widthMM, heightMM;
            glfwGetMonitorPhysicalSize(lMonitor[i], &widthMM, &heightMM);
            const GLFWvidmode *mode = glfwGetVideoMode(lMonitor[i]);
            const double dpi = mode->width / (widthMM / 25.4);
        }

        GLFWmonitor *lCurrentMonitor;

        if (lNumberOfMonitors <= 1)
            lCurrentMonitor = glfwGetPrimaryMonitor();
        else
            lCurrentMonitor = lMonitor[DATA.MONITOR];

        int numberofmodes;

        const GLFWvidmode *modes = glfwGetVideoModes(lCurrentMonitor, &numberofmodes);

        GLFWwindow *window;

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

        const GLFWvidmode *mode = glfwGetVideoMode(lCurrentMonitor);
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, 60);
        glfwWindowHint(GLFW_RED_BITS, 16);
        glfwWindowHint(GLFW_GREEN_BITS, 16);
        glfwWindowHint(GLFW_BLUE_BITS, 16);

        Log::WriteToLog("Creating window || Window Resolution: ", Log::FILE | Log::CONSOLE);
        int lWindowWidth = DATA.WINDOW_WIDTH;
        int lWindowHeight = DATA.WINDOW_HEIGHT;
        Log::WriteToLog(std::to_string(lWindowWidth) + "x" + std::to_string(lWindowHeight));

        //Window is created in full screen mode
        if (DATA.FULLSCREEN == 1)
        {
            window = glfwCreateWindow(lWindowWidth, lWindowHeight, ProgramName, lCurrentMonitor, 0);
        }
        //Window is created in borderless fullscreen mode
        else if (DATA.FULLSCREEN == 2)
        {
            mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
            glfwWindowHint(GLFW_RED_BITS, mode->redBits);
            glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
            glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
            glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
            glfwWindowHint(GLFW_DECORATED, GL_FALSE);
            window = glfwCreateWindow(mode->width, mode->height, ProgramName, lCurrentMonitor, 0);
        }
        //Window is created in windowed mode and centered in the middle of the display
        else
        {
            window = glfwCreateWindow(lWindowWidth, lWindowHeight, ProgramName, nullptr, nullptr);
            glfwSetWindowPos(window, (mode->width / 2) - (lWindowWidth * 0.5), (mode->height / 2) - (lWindowHeight * 0.5));
        }

        glfwMakeContextCurrent(window);

        if (!window)
        {
            Log::WriteToLog("Window could not be created");
        }

#ifndef EPSILON_STATIC
        glewExperimental = GL_TRUE;
        glewInit();
#endif

        glfwSetInputMode(window, GLFW_STICKY_KEYS, true);

        //Set up IO callbacks
        glfwSetKeyCallback(window, Input::KeyBoard::KeyBoardCallBack);
        glfwSetCursorPosCallback(window, Input::Mouse::MouseCallBack);
        glfwSetJoystickCallback(Input::Joystick::JoystickManager::JoystickCallback);

        glfwSwapInterval(DATA.VSYNC);

        return window;
    }
} // namespace Epsilon