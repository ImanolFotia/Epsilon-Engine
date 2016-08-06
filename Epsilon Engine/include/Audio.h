#pragma once
#include <vector>
#include <memory>
#include <AudioElement.h>


namespace Audio {

    class Audio
    {
    public:
        Audio() {

            device=alcOpenDevice(NULL);
            if(device==NULL)
            {
                std::cout << "cannot open sound card" << std::endl;
                return ;
            }
            context=alcCreateContext(device,NULL);
            if(context==NULL)
            {
                std::cout << "cannot open context" << std::endl;
                return ;
            }
            alcMakeContextCurrent(context);
        }
        ~Audio() {

            alcMakeContextCurrent(NULL);
            alcDestroyContext(context);
            alcCloseDevice(device);

            context = nullptr;
            device = nullptr;
        }

    public:

        void addAudioElement(std::shared_ptr<AudioElement>);

        bool setMasterVolume(float);
        bool setMusicVolume(float);
        bool setGameVolume(float);

    private:
        float m_MasterVolume;
        float m_MusicVolume;
        float m_GameVolume;


        ALCdevice* device;
        ALCcontext* context;

        std::vector<std::shared_ptr<AudioElement>> m_AudioElementsCollection;
    };
}
