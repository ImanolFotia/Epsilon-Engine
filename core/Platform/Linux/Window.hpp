#pragma once


#include "Platform/WindowBase.hpp"
#include "Log.h"

#include "IO/KeyBoard.h"
#include "IO/Mouse.h"
#include "IO/Joystick/Joystick.h"

#include <Driver/API/OpenGL/Context.hpp>
#include <Driver/API/Vulkan/Context.hpp>


#ifdef __linux__

namespace Epsilon
{
    namespace Platform
    {
        namespace Linux
        {
            class Window : public WindowBase
            {
            public:
                Window(API::CONTEXT_TYPE type);

                ~Window() {}

                virtual void Init(const char *title, int w, int h) override{}

                virtual void OnResize(GLFWwindow*, int, int) override {}

                virtual void Destroy() override{}
                
                virtual void SwapBuffers() override{}

                virtual void ShowCursor() override{}
                virtual void HideCursor() override{}
                virtual bool WantsToClose() override{}
                virtual unsigned FrameNumber() override{}
                virtual bool NeedsToResize() override{}

            private:
            };
        } // namespace Windows
    }     // namespace Platform
} // namespace Epsilon

#endif