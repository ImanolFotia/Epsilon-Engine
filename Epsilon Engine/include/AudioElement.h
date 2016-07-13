#pragma once
#include <Types.h>

class AudioElement
{
    AudioElement(){}
    ~AudioElement(){}

    unsigned int m_AudioID;
    float m_Volume;
    int m_Channels;

    AUDIO_TYPE m_Type;

    glm::vec3 m_Position;
    glm::vec3 m_Direction;
    glm::vec3 m_Velocity;
    float m_Radius;
};
