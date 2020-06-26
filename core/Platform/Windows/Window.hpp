#pragma once

#include "Platform/WindowBase.hpp"
#include "Log.h"

#include "IO/KeyBoard.h"
#include "IO/Mouse.h"
#include "IO/Joystick/Joystick.h"

#ifdef WIN32

namespace Epsilon
{
    namespace Platform
    {
        namespace Windows
        {
            class Window : public WindowBase
            {
            public:
                Window() = default;
                ~Window() {}

                virtual void Init(const char *title, int w, int h) const override
                {
                    if (!glfwInit())
                    {
                        Log::WriteToLog("GLFW could not be initialized");
                    }
                    if (mContext->getType() == API::CONTEXT_TYPE::OGL)
                    {
                        GLFWmonitor *lCurrentMonitor;
                        lCurrentMonitor = glfwGetPrimaryMonitor();
                        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
                        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
                        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

                        const GLFWvidmode *mode = glfwGetVideoMode(lCurrentMonitor);
                        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
                        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
                        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
                        glfwWindowHint(GLFW_REFRESH_RATE, 60);
                        glfwWindowHint(GLFW_RED_BITS, 16);
                        glfwWindowHint(GLFW_GREEN_BITS, 16);
                        glfwWindowHint(GLFW_BLUE_BITS, 16);
                        *mWindowHandle = glfwCreateWindow(w, h, title, nullptr, nullptr);
                    }

                    glfwSetErrorCallback([](int error, const char *description) {
                        fprintf(stderr, "Error: %s\n", description);
                    });

                    //Set up IO callbacks
                    glfwSetKeyCallback(mWindowHandle->getHandle(), Input::KeyBoard::KeyBoardCallBack);
                    glfwSetCursorPosCallback(mWindowHandle->getHandle(), Input::Mouse::MouseCallBack);
                    glfwSetJoystickCallback(Input::Joystick::JoystickManager::JoystickCallback);
                }
                virtual void Resize(int w, int h) const override
                {
                }
                
                virtual void Destroy() override
                {
                    glfwDestroyWindow(mWindowHandle->getHandle());
                    glfwTerminate();
                }

            private:
                std::shared_ptr<WindowHandle<>> mWindowHandle;
            };
        } // namespace Windows
    }     // namespace Platform
} // namespace Epsilon

#endif