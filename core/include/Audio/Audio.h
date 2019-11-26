#pragma once
#include <unordered_map>
#include <memory>
#include <Audio/AudioElement.h>

namespace IO {
    namespace Audio {

        class Audio {
        public:
            Audio();
            ~Audio();

        public:

            void addAudioElement(int, std::shared_ptr<AudioElement> element);

            void PlayAudio();
            void PlayByID(int);

            bool setMasterVolume(float);
            bool setMusicVolume(float);
            bool setGameVolume(float);

        private:
            float m_MasterVolume;
            float m_MusicVolume;
            float m_GameVolume;


            ALCdevice* device;
            ALCcontext* context;

            std::unordered_map<int, std::shared_ptr<AudioElement> > m_AudioElementsCollection;
        };
    }
}
