#pragma once
#include <Types.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ParticleRenderer.h>
#include <random>
#include <Types.h>
#include <chrono>
class ParticleProxy {
public:
    ParticleProxy(unsigned int id, MINMAX_POINTS limits, double numparts) : ID(id), ParticlesLimits(limits), m_NumParticles(numparts) {}

    virtual ~ParticleProxy() {}

    virtual void Simulate(float, glm::vec3) {
    }

    virtual void Render() {}

    double m_NumParticles;
    std::vector<glm::mat4> m_ParticlePositions;
    GLuint quadVAO, quadVBO;
    std::shared_ptr<ParticleRenderer> m_ParticleRenderer;

private:
        std::random_device rd;
        std::default_random_engine generator;
protected:

    MINMAX_POINTS ParticlesLimits;
    unsigned int ID;
    std::vector<std::shared_ptr<Particle> > Particles;

protected:

    void calculateDistancetoCamera(glm::vec3 camPosition)
    {
        for(auto p: Particles)
        {
            p->setDistancetoCamera(glm::length(p->getPosition() - camPosition));
        }
    }

    void sortParticles() {
        std::sort(Particles.begin(), Particles.end());
    }

    glm::vec3 GenerateRandomParticle() {

        std::uniform_real_distribution<GLfloat> HorizonalLimit(ParticlesLimits.MIN_X, ParticlesLimits.MAX_X);
        std::uniform_real_distribution<GLfloat> VerticalLimit(ParticlesLimits.MIN_Y, ParticlesLimits.MAX_Y);
        std::uniform_real_distribution<GLfloat> ZLimit(ParticlesLimits.MIN_Z, ParticlesLimits.MAX_Z);
        generator.seed(std::chrono::system_clock::now().time_since_epoch().count());

        return glm::vec3(
                   HorizonalLimit  (generator),
                   VerticalLimit   (generator),
                   ZLimit          (generator)
               );
    }

    void InitializeProxy() {

        std::uniform_real_distribution<GLfloat> HorizonalLimit(ParticlesLimits.MIN_X, ParticlesLimits.MAX_X);
        std::uniform_real_distribution<GLfloat> VerticalLimit(ParticlesLimits.MIN_Y, ParticlesLimits.MAX_Y);
        std::uniform_real_distribution<GLfloat> ZLimit(ParticlesLimits.MIN_Z, ParticlesLimits.MAX_Z);
        generator.seed(std::chrono::system_clock::now().time_since_epoch().count());

        for(int i = 0; i < m_NumParticles; i++) {

            glm::vec3 CurrentParticlePosition(
                HorizonalLimit  (generator),
                VerticalLimit   (generator),
                ZLimit          (generator)
            );

            Particles.push_back((std::shared_ptr<Particle>) new Particle(CurrentParticlePosition, 1.0, 50.0));
            std::cout << "Particle #" << i << ": X: " << Particles[i]->getPosition().x << " Y: " << Particles[i]->getPosition().y << " Z: " << Particles[i]->getPosition().z << std::endl;
        }

        for(auto p: Particles) {
            glm::mat4 modelPos = glm::mat4();
            modelPos = translate(modelPos, p->getPosition());
            m_ParticlePositions.push_back(modelPos);
        }
    }
};

class MistParticleProxy : public ParticleProxy {

public:
    MistParticleProxy(unsigned int id, MINMAX_POINTS limits, double numparts) : ParticleProxy(id, limits, numparts) {

        ID = id;
        ParticlesLimits = limits;
        m_NumParticles = numparts;

        InitializeProxy();

        m_ParticleRenderer = (std::shared_ptr<ParticleRenderer>) new ParticleRenderer(ID, m_NumParticles, m_ParticlePositions);
    }

    virtual ~MistParticleProxy() {}

    void Simulate() {}

    virtual void Render()
    {
        m_ParticleRenderer->UpdateParticleRenderer(m_ParticlePositions);

        m_ParticleRenderer->Render();
    }
};

class RainParticleProxy : public ParticleProxy {
public:
    RainParticleProxy(unsigned int id, MINMAX_POINTS limits, double numparts) : ParticleProxy(id, limits, numparts) {

        ID = id;
        ParticlesLimits = limits;
        m_NumParticles = numparts;

        InitializeProxy();

        m_ParticleRenderer = (std::shared_ptr<ParticleRenderer>) new ParticleRenderer(ID, m_NumParticles, m_ParticlePositions);
    }

    virtual ~RainParticleProxy() {}

    void Simulate(float deltaTime, glm::vec3 camPos) {

        this->calculateDistancetoCamera(camPos);

        this->sortParticles();

        for(int i = 0; i < Particles.size(); i++) {

            if(Particles[i]->m_Position.y < ParticlesLimits.MIN_Y){
                Particles[i]->setLife(-1.0);
                //std::cout << "Reset Particle: Limit" << std::endl;
            }

            if(Particles[i]->getLife() < 0.0) {
                //Particles[i]->setPosition(GenerateRandomParticle());
                Particles[i]->resetPosition();
                Particles[i]->resetLife();
                //std::cout << "Reset Particle" << std::endl;
            }



            Particles[i]->setSpeed(glm::vec3(0.0, -9.81, 0.0));
            Particles[i]->m_Position.y = Particles[i]->m_Position.y + -9.81 * deltaTime * 3.0;
            //p.setPosition( p.getPosition() + p.getSpeed() *deltaTime);

            Particles[i]->setLife(Particles[i]->getLife() - deltaTime);

        }

        for(int i = 0; i < m_NumParticles; i++) {
            glm::mat4 modelPos = glm::mat4();
            modelPos = translate(modelPos, Particles[i]->getPosition());
            m_ParticlePositions[i] = modelPos;
        }
    }

    virtual void Render()
    {
        m_ParticleRenderer->UpdateParticleRenderer(m_ParticlePositions);

        m_ParticleRenderer->Render();
    }
};

class SnowParticleProxy : public ParticleProxy {
public:
    SnowParticleProxy(unsigned int id, MINMAX_POINTS limits, double numparts) : ParticleProxy(id, limits, numparts) {

        ID = id;
        ParticlesLimits = limits;
        m_NumParticles = numparts;

        InitializeProxy();

        m_ParticleRenderer = (std::shared_ptr<ParticleRenderer>) new ParticleRenderer(ID, m_NumParticles, m_ParticlePositions);
    }

    virtual ~SnowParticleProxy() {}

    void Simulate() {}

    virtual void Render()
    {
        m_ParticleRenderer->UpdateParticleRenderer(m_ParticlePositions);

        m_ParticleRenderer->Render();
    }
};
