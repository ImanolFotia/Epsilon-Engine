#pragma once

#include <memory>

namespace Epsilon
{
    namespace Driver
    {
        namespace Audio
        {
            class AudioDriver
            {
            public:
                AudioDriver() {}

                virtual int Init() = 0;
                virtual int Shutdown() = 0;

                virtual int getNumDevices() = 0;

                virtual int Start() = 0;
                virtual int End() = 0;

                virtual bool isActive() = 0;
            };
        } // namespace Audio
    }     // namespace Driver
} // namespace Epsilon