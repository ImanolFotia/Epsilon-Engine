#pragma once
#include <glm/glm.hpp>

class Particle
{
public:
    Particle(){}
    virtual ~Particle() {
        std::cout << "Deleted Particle" << std::endl;}

    const glm::vec3 getPosition() const {
        return m_Position;
    }

    const float getSpeed() const {
        return m_Speed;
    }

    const float getLife() const {
        return m_Life;
    }

    const float getWeight() const {
        return m_Weight;
    }

private:
    glm::vec3 m_Position;
    float m_Speed;
    float m_Life;
    float m_Weight;
};

