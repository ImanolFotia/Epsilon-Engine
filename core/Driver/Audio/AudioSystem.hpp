#pragma once

#include <pch.hpp>

#include "OpenAL/AudioDevice.h"
#include "OpenAL/AudioSource.h"

#include <glm/glm.hpp>

namespace Epsilon::Audio
{
    class AudioSystem
    {
    public:
        AudioSystem()
        {
            AudioDevice = std::make_shared<OpenAL::AudioDevice>();
        }

        void ListDevices() {

        }

        void addAudioSource(std::shared_ptr<AudioSource> source) {

        }

        void setSourcePosition(uint32_t id, glm::vec3 pos) {

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
    };
}