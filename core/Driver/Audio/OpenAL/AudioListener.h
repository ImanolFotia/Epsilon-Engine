#pragma once

#include <pch.hpp>

#include <AL/al.h>
#include <AL/alc.h>
#include <glm/glm.hpp>

#include "../AudioListener.hpp"

namespace Epsilon::Audio::OpenAL
{
    class AudioListener : public Epsilon::Audio::AudioListener
    {

    public:
        AudioListener() {}
        ~AudioListener()
        {
            std::cout << "Deleted AudioListener" << std::endl;
        }

        void UpdateListener()
        {
            alListener3f(AL_POSITION, this->m_ListenerPosition.x, this->m_ListenerPosition.y, this->m_ListenerPosition.z);
            alListenerfv(AL_ORIENTATION, m_ListenerOrientation);
        }

        void setListenerPosition(glm::vec3 pos)
        {
            m_ListenerPosition = pos;
        }

        void setListenerDirection(glm::vec3 dir, glm::vec3 up)
        {
            m_ListenerDirection = glm::normalize(dir);
            m_ListenerOrientation[0] = this->m_ListenerDirection.x;
            m_ListenerOrientation[1] = this->m_ListenerDirection.y;
            m_ListenerOrientation[2] = this->m_ListenerDirection.z;
            m_ListenerOrientation[3] = up.x;
            m_ListenerOrientation[4] = up.y;
            m_ListenerOrientation[5] = up.z;
        }

        glm::vec3 getListenerPosition() const
        {
            return m_ListenerPosition;
        }

        glm::vec3 getListenerDirection() const
        {
            return m_ListenerDirection;
        }

    private:
        glm::vec3 m_ListenerPosition;
        glm::vec3 m_ListenerDirection;
        float m_ListenerOrientation[6];
        float m_ListenerVelocity;
    };
} // namespace Epsilon