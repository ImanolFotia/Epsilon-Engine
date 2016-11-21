#pragma once

#include <vector>
#include <algorithm>
#include <Particle.h>

class ParticleSystem()
{
public:
    ParticleSystem();

    virtual ~ParticleSystem(){
        //std::cout << "Deleted ParticleSystem" << std::endl;}

    void UpdateParticleSystem();

private:

    void m_Sort();

    void m_Simulate();

    std::vector<Particle> m_ParticleContainer;
};

