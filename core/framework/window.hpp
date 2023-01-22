#pragma once

#include <string>

#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#ifdef __linux__
#define VK_USE_PLATFORM_XLIB_KHR
#endif

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#include "core/framework/IO/KeyBoard.hpp"
#include "core/framework/IO/Mouse.hpp"
#include "core/framework/IO/Joystick/Joystick.hpp"

namespace framework
{
    class Window
    {
    public:
        void init(std::string appName, int w = 1280, int h = 720)
        {
            mWidth = w;
            mHeight = h;

            glfwInit();

            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

            mWindow = glfwCreateWindow(mWidth, mHeight, appName.c_str(), nullptr, nullptr);

            //Set up IO callbacks
            glfwSetKeyCallback(mWindow, Input::KeyBoard::KeyBoardCallBackGLFW);
            glfwSetCursorPosCallback(mWindow, Input::Mouse::MouseCallBackGLFW);
            glfwSetJoystickCallback(Input::Joystick::JoystickManager::JoystickCallbackGLFW);
            glfwSetMouseButtonCallback(mWindow, Input::Mouse::MouseButtonCallbackGLFW);
            glfwSetScrollCallback(mWindow, Input::Mouse::MouseWheelCallbackGLFW);

            glfwSetInputMode(mWindow, GLFW_STICKY_KEYS, true);
            glfwSetInputMode(mWindow, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
        }

        void mainLoop()
        {
            while (!glfwWindowShouldClose(mWindow))
            {
                glfwPollEvents();
            }
        }

        bool ShouldClose() {
            return glfwWindowShouldClose(mWindow);
        }

        void PollEvents() {
            glfwPollEvents();
        }

        void ShowCursor()
        {
            glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        void HideCursor()
        {
            glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        void setCursorPosition(int x, int y) {
            glfwSetCursorPos(mWindow, x, y);
        }

        void cleanup()
        {
            glfwDestroyWindow(mWindow);
            glfwTerminate();
        }

        void setWindowTitle(const char* title) {
            glfwSetWindowTitle(mWindow, title);
        }

        GLFWwindow* getWindow() const {
            return mWindow;
        }

    private:
        unsigned mWidth = 1280;
        unsigned mHeight = 720;

        GLFWwindow *mWindow;
    };
}