#pragma once
#include <vector>
#include <memory>

#include <AudioElement.h>


namespace Audio {

    class Audio
    {
    public:
        Audio() {}
        ~Audio() {}

    public:

        void addAudioElement(std::shared_ptr<AudioElement>);

        bool setMasterVolume(float);
        bool setMusicVolume(float);
        bool setGameVolume(float);

    private:
        float m_MasterVolume;
        float m_MusicVolume;
        float m_GameVolume;

        std::vector<std::shared_ptr<AudioElement>> m_AudioElementsCollection;
    };
}
