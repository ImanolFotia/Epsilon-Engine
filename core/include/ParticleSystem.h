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

    void addNewSystem(MINMAX_POINTS, PARTICLE_PROXY, unsigned int);

    void UpdateParticleSystem();

    void Render();

    void Simulate(float, glm::vec3);

private:


    std::vector<std::shared_ptr<ParticleProxy> > m_ParticleProxyContainer;
};

