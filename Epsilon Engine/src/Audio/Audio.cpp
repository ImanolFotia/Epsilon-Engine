#include <Audio/Audio.h>
namespace IO {
    namespace Audio {

        Audio::Audio() {

            device=alcOpenDevice(NULL);
            if(device==NULL) {
                std::cout << "cannot open sound card" << std::endl;
                return ;
            }
            context=alcCreateContext(device,NULL);
            if(context==NULL) {
                std::cout << "cannot open context" << std::endl;
                return ;
            }
            alcMakeContextCurrent(context);
            alDopplerFactor(1.0f);
            alDopplerVelocity(340.29f);
        }
        Audio::~Audio() {

            alcMakeContextCurrent(NULL);
            alcDestroyContext(context);
            alcCloseDevice(device);

            context = nullptr;
            device = nullptr;
            std::cout << "Deleted Audio" << std::endl;
        }


        void Audio::addAudioElement(std::shared_ptr<AudioElement> element) {
            m_AudioElementsCollection.push_back(element);
        }

        void Audio::PlayAudio() {
            for(auto i : m_AudioElementsCollection) {
                i->Play();
            }
        }

        bool Audio::setMasterVolume(float) {}
        bool Audio::setMusicVolume(float) {}
        bool Audio::setGameVolume(float) {}

    }
}
