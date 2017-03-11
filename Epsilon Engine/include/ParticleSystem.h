#pragma once

#include <vector>
#include <iostream>
#include <algorithm>
#include <Particle.h>
#include <ParticleProxy.h>

class ParticleSystem
{
public:
    ParticleSystem();

    virtual ~ParticleSystem(){
        std::cout << "Deleted ParticleSystem" << std::endl;}

    void UpdateParticleSystem();

    void Render();

    void UpdateParticles();

private:

    void m_Sort();

    void m_Simulate();

    std::vector<ParticleProxy> m_ParticleProxyContainer;
};

