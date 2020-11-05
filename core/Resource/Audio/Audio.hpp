#pragma once

#include <pch.hpp>

#include "Loaders/WAVFile.h"

#include "../Resource.hpp"

namespace Epsilon
{
    namespace Resources
    {

        class Audio : public Resource<ResourceType::AUDIO>
        {
            public:
            Audio(const char *filepath)
            {
                //Only WAV files supported for now
                
                mAudioFile = std::make_shared<WAVfile>();
                mData = mAudioFile->Load(filepath);

                mBitRate = mAudioFile->getSampleRate();
                mChannels = mAudioFile->getNumberOfChannels();
                mBitsPerSecond = mAudioFile->getBPS();
                mFileSize = mAudioFile->getFileSize();
            }

            const uint32_t BitRate() { return mBitRate; }
            const uint32_t Channels() { return mChannels; }
            const uint32_t BitsPerSecond() { return mBitsPerSecond; }
            const uint32_t FileSize() { return mFileSize; }

            private:
                uint32_t mBitRate;
                uint32_t mChannels;
                uint32_t mBitsPerSecond;
                uint32_t mFileSize;

                std::shared_ptr<WAVfile> mAudioFile;

        };
    } // namespace Resources
} // namespace Epsilon