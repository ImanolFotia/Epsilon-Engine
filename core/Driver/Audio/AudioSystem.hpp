#pragma once

#include <pch.hpp>

#include "OpenAL/AudioDevice.h"
#include "OpenAL/AudioSource.h"
#include "OpenAL/AudioListener.h"

#include <glm/glm.hpp>

namespace Epsilon::Audio
{
    class AudioSystem
    {
        using Listener_ptr = std::shared_ptr<AudioListener>;
        using Source_ptr = std::shared_ptr<AudioSource>;
    public:
        AudioSystem()
        {
            AudioDevice = std::make_shared<OpenAL::AudioDevice>();
        }

        void ListDevices() {

        }

        void addAudioSource(Source_ptr source) {
            mSources.insert(mSources.end(), source);
        }

        void setSourcePosition(uint32_t id, glm::vec3 pos) {
            mSources.at(id)->setPosition(pos);
        }

        void setSourceDirection(uint32_t id, glm::vec3 dir) {
            
        }

        void setSourceVelocity(glm::vec3 vel) {

        }

        void setListenerPosition(glm::vec3 pos) {

        }

        void setListenerDirection(glm::vec3 dir) {

        }

        void setListenerVelocity(glm::vec3 vel) {

        }

    private:
        std::shared_ptr<AudioDevice> AudioDevice;
        std::vector<Listener_ptr> mListeners;
        std::vector<Source_ptr> mSources;

    };
}