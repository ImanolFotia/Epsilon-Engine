#pragma once

#include <pch.hpp>
#include <Driver/API/Context.hpp>
#include "Device.hpp"
#include "WindowHandle.hpp"

namespace Epsilon
{
    namespace Platform
    {

        class WindowData
        {
            public:
            int Width;
            int Height;
            int NumMonitors;
            int CurrentMonitor;
            int VSync;
            int State;
            const char* Title;
        };

        class WindowBase
        {
        public:
            WindowBase() = default;

            virtual ~WindowBase() {}

            virtual void Init(const char *, int, int) = 0;
            virtual void Resize(int, int) const = 0;
            virtual void Destroy() = 0;
            virtual void SwapBuffers() = 0;
            virtual void ShowCursor() = 0;
            virtual void HideCursor() = 0;

            void setWindowData(const WindowData &data)
            {
                mWindowData = data;
            }

            const WindowData &getWindowData()
            {
                return mWindowData;
            }

            const API::ContextBase_ptr getContext()
            {
                return mContext;
            }

            const std::shared_ptr<WindowHandle<>> getHandle() {
                return mWindowHandle;
            }

            

        protected:
            std::shared_ptr<API::ContextBase> mContext;
            std::shared_ptr<WindowHandle<>> mWindowHandle;
            WindowData mWindowData;
        };
    } // namespace Platform
} // namespace Epsilon