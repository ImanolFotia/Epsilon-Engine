//
// Created by solaire on 2/2/23.
//

#include "window.hpp"

#include "core/framework/IO/KeyBoard.hpp"
#include "core/framework/IO/Mouse.hpp"
#include "core/framework/IO/DragDrop.hpp"
#include "core/framework/IO/Joystick/Joystick.hpp"

namespace framework
{
    void Window::init(std::string appName, int w, int h)
    {
        mWidth = w;
        mHeight = h;

#if USE_GLFW
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        /*
        auto mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        glfwWindowHint(GLFW_DECORATED, GL_TRUE);*/

        int count = 0;
        const GLFWvidmode* modes = glfwGetVideoModes(glfwGetPrimaryMonitor(), &count);


        mWindow = glfwCreateWindow(mWidth, mHeight, appName.c_str(), nullptr, nullptr);
        auto currentMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        for (int i = 0; i < count; i++) {
            WindowSizeDescription size_desc{};
            auto& mode = modes[i];
            size_desc.id = i;
            size_desc.width = mode.width;
            size_desc.height = mode.height;
            size_desc.refreshRate = (float)mode.refreshRate;

            char buffer[256] = {0};
            std::sprintf(buffer, "%dx%d@%.2f", mode.width, mode.height, size_desc.refreshRate);
            size_desc.size_string = std::string(buffer);
            if (currentMode->width == size_desc.width && currentMode->height == size_desc.height && (float)currentMode->refreshRate == size_desc.refreshRate) {
                size_desc.isCurrent = true;
            }
            m_pAvailableSizes.push_back(size_desc);
        }

        // Set up IO callbacks
        glfwSetKeyCallback(mWindow, Input::KeyBoard::KeyBoardCallBackGLFW);
        glfwSetCursorPosCallback(mWindow, Input::Mouse::MouseCallBackGLFW);
        glfwSetJoystickCallback(Input::Joystick::JoystickManager::JoystickCallbackGLFW);
        glfwSetMouseButtonCallback(mWindow, Input::Mouse::MouseButtonCallbackGLFW);
        glfwSetScrollCallback(mWindow, Input::Mouse::MouseWheelCallbackGLFW);
        glfwSetDropCallback(mWindow, Input::DragDrop::DropCallbackGLFW);

        glfwSetInputMode(mWindow, GLFW_STICKY_KEYS, true);
        glfwSetInputMode(mWindow, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
#endif
#if defined(_WIN32) && (USE_GLFW == false)
        WNDCLASSEX wcex;

        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = hInstance;
        wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
        wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wcex.lpszMenuName = NULL;
        wcex.lpszClassName = appName;
        wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
        if (!RegisterClassEx(&wcex))
            exitOnError("Failed to register window");

        windowInstance = hInstance;

        window = CreateWindow(
            appName,
            appName,
            WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
            0,
            0,
            mWidth,
            mWidth,
            NULL,
            NULL,
            windowInstance,
            NULL);
#endif
#ifdef BUILD_ANDROID
#endif
    }
}