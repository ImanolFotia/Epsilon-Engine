#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>

class Particle
{
public:
    Particle(glm::vec3 pos, float speed, float life) : m_Position(pos), m_Speed(speed), m_InitialLife(life) {
        m_Life = m_InitialLife;
        m_InitialPosition = pos;
        }
    virtual ~Particle() {
    }

    const glm::vec3 getPosition() const {
        return m_Position;
    }

    void resetPosition(){
        m_Position = m_InitialPosition;
    }

    const glm::vec3 getSpeed() const {
        return m_Speed;
    }

    void setSpeed(glm::vec3 sp) {
        m_Speed = sp;
    }

    const float getLife() const {
        return m_Life;
    }

    void setLife(float life) {
        m_Life = life;
    }

    void resetLife() {
        m_Life = m_InitialLife;
    }

    const float getWeight() const {
        return m_Weight;
    }

    void setPosition(const glm::vec3 pos) {
        m_Position = pos;
    }

    void setDistancetoCamera(float dist){
        this->m_DistancetoCamera = dist;
    }

    const float getDistancetoCamera() const {
        return this->m_DistancetoCamera;
    }

    bool operator<(const Particle& that) const {
        // Sort in reverse order : far particles drawn first.
        return this->m_DistancetoCamera > that.m_DistancetoCamera;
    }

    glm::vec3 m_Position;
private:
    glm::vec3 m_Speed;
    float m_Life;
    glm::vec3 m_InitialPosition;
    float m_InitialLife;
    float m_Weight;
    float m_DistancetoCamera;

};



