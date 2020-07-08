#pragma once

#include "Platform/WindowBase.hpp"
#include "Log.h"

#include "IO/KeyBoard.h"
#include "IO/Mouse.h"
#include "IO/Joystick/Joystick.h"

#include <Driver/API/OpenGL/Context.hpp>


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
                Window();

                ~Window() {}

                virtual void Init(const char *title, int w, int h) override;

                virtual void Resize(int w, int h) const override {}

                virtual void Destroy() override;
                
                virtual void SwapBuffers() override;

            private:
            };
        } // namespace Windows
    }     // namespace Platform
} // namespace Epsilon

#endif