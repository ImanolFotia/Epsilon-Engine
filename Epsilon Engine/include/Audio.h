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
            alDopplerFactor(1.0f);
            alDopplerVelocity(340.29f);
        }
        ~Audio() {

            alcMakeContextCurrent(NULL);
            alcDestroyContext(context);
            alcCloseDevice(device);

            context = nullptr;
            device = nullptr;
            std::cout << "Deleted Audio" << std::endl;
        }

    public:

        void addAudioElement(std::shared_ptr<AudioElement> element)
        {
            m_AudioElementsCollection.push_back(element);
        }

        void PlayAudio()
        {
            for(auto i : m_AudioElementsCollection)
            {
                i->Play();
            }
        }

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
