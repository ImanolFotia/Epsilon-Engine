#pragma once
#include <Types.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <OpenGL/HelperFunctions/FullScreenQuad.h>

class ParticleProxy {
public:
    ParticleProxy() {}
    virtual ~ParticleProxy() {}
    virtual void Update() {}
    virtual void Render() {}
    double m_NumParticles;
    std::vector<glm::vec3> m_ParticlePositions;
    GLuint quadVAO, quadVBO;
private:
    std::vector<Particle> Particles;
    MINMAX_POINTS ParticlesLimits;
protected:
};

class MistParticleProxy : public ParticleProxy {
public:
    MistParticleProxy() {}
    virtual ~MistParticleProxy() {}
    std::shared_ptr<OpenGLHelpers::FullScreenQuad> m_Quad;

    virtual void Render()
    {
        m_Quad->Render();
    }
};

class RainParticleProxy : public ParticleProxy {
public:
    RainParticleProxy() {}
    virtual ~RainParticleProxy() {}
    std::shared_ptr<OpenGLHelpers::FullScreenQuad> m_Quad;

    virtual void Render()
    {
        m_Quad->Render();
    }
};

class SnowParticleProxy : public ParticleProxy {
public:
    SnowParticleProxy() {}
    virtual ~SnowParticleProxy() {}
    std::shared_ptr<OpenGLHelpers::FullScreenQuad> m_Quad;

    virtual void Render()
    {
        m_Quad->Render();
    }
};
