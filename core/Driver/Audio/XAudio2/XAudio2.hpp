#pragma once

#include <Driver/Audio/AudioDriver.hpp>

#ifdef _WIN32

#include <windows.h>
#include "XAudio2.h"

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
                
                virtual int Shutdown(){ return 0; }

                virtual int getNumDevices(){ return 0; }

                virtual int Start(){ return 0; }
                virtual int End(){ return 0; }

                virtual bool isActive(){ return 0; }
            };
        } // namespace Audio
    } // namespace API
} // namespace Epsilon

#endif