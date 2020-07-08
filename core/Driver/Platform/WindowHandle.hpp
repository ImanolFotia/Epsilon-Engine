#pragma once

#include <memory>

struct GLFWwindow;

namespace Epsilon
{
    namespace Platform
    {

        template <typename T = GLFWwindow>
        class WindowHandle
        {

            enum WINDOW_HANDLE_TYPE
            {
                OGL = 0,
                D3D,
                VULKAN,
                METAL,
                D3D12
            };

        public:
            WindowHandle(WINDOW_HANDLE_TYPE type) : mType(type)
            {
            }

            void SetHandle(T handle) {
                mHandle = handle;
            }

            T* getHandle()
            {
                return mHandle.get();
            }

            const WINDOW_HANDLE_TYPE getType()
            {
                return mType;
            }

            void operator=(const T* t) {
                mHandle = t;
            }

        private:
            std::shared_ptr<T> mHandle;
            WINDOW_HANDLE_TYPE mType;
        };

    } // namespace Platform
} // namespace Epsilon