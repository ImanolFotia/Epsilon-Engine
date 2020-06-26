#pragma once

#include <Driver/Audio/AudioDriver.hpp>

#define EPSILON_XAUDIO_FIXED 0

#if EPSILON_XAUDIO_FIXED

#include <xaudio2redist.h>

namespace Epsilon
{
    namespace Driver
    {
        namespace Audio
        {
            class XAudio2Driver : public AudioDriver
            {
            public:
                XAudio2Driver() {}

                int Init()
                {
                    IXAudio2 *pXAudio2 = NULL;
                    HRESULT hr;
                    if (FAILED(hr = XAudio2Create(&pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR)))
                        return hr;
                }
                
                virtual int Shutdown(){}

                virtual int getNumDevices(){}

                virtual int Start(){}
                virtual int End(){}

                virtual bool isActive(){}
            };
        } // namespace Audio
    } // namespace API
} // namespace Epsilon

#endif