#pragma once

#include "Context.hpp"
#include "Device.hpp"

#include <memory>

namespace Epsilon {
    namespace Platform {
        class WindowBase {

            class WindowData {
                int Width;
                int Height;
            };

            public:
                WindowBase() = default;

                virtual ~WindowBase() {}

                virtual void Create(int, int) = 0;
                virtual void Resize(int, int) = 0;
                virtual void Destroy() = 0;

                const ContextBase_ptr getContext() {
                    return mContext;
                }
            
            private:
                ContextBase_ptr mContext;

        }
    };
}