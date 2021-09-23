#pragma once

namespace Epsilon::Audio
{
    class AudioSource
    {
    public:
        AudioSource(const char *, AUDIO_TYPE, glm::vec3, glm::vec3) {}

        virtual void Destroy() = 0;

        virtual void Play() = 0;
        virtual void Pause() = 0;
        virtual void Stop() = 0;

        virtual void setPosition(glm::vec3 position) = 0;
        virtual glm::vec3 getPosition() = 0;
        
        virtual void setDirection(glm::vec3 dir) = 0;
        virtual glm::vec3 getDirection() = 0;

    };
}