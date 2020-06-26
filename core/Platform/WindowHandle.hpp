#pragma once

#include <memory>

struct GLFWwindow;

namespace Epsilon
{
    namespace Platform
    {

        enum WINDOW_HANDLE_TYPE
        {
            HDC = 0,
            GLFW,
            GLUT,
            SFML,
            X11
        };
        template <typename T = GLFWwindow>
        class WindowHandle
        {

        public:
            WindowHandle(WINDOW_HANDLE_TYPE type) : mType(type)
            {
            }

            void SetHandle(T handle)
            {
                mHandle = handle;
            }

            T *getHandle()
            {
                return mHandle.get();
            }

            const WINDOW_HANDLE_TYPE getType()
            {
                return mType;
            }

            void operator=(const T *t)
            {
                mHandle = t;
            }

        private:
            std::shared_ptr<T> mHandle;
            WINDOW_HANDLE_TYPE mType;
        };

    } // namespace Platform
} // namespace Epsilon