#include <ParticleSystem.h>
#include <Types.h>

namespace Epsilon
{
    ParticleSystem::ParticleSystem()
    {
    }

    void ParticleSystem::Render()
    {
        for (auto PP : m_ParticleProxyContainer)
        {
            PP->Render();
        }
    }

    void ParticleSystem::addNewSystem(MINMAX_POINTS limits, PARTICLE_PROXY ProxyType, unsigned int numParts)
    {
        switch (ProxyType)
        {
        case SNOW:
            this->m_ParticleProxyContainer.push_back(
                (std::shared_ptr<SnowParticleProxy>)new SnowParticleProxy(
                    this->m_ParticleProxyContainer.size() + 1,
                    limits,
                    numParts));
            break;

        case MIST:
            this->m_ParticleProxyContainer.push_back(
                (std::shared_ptr<MistParticleProxy>)new MistParticleProxy(
                    this->m_ParticleProxyContainer.size() + 1,
                    limits,
                    numParts));
            break;

        case RAIN:
            this->m_ParticleProxyContainer.push_back(
                (std::shared_ptr<RainParticleProxy>)new RainParticleProxy(
                    this->m_ParticleProxyContainer.size() + 1,
                    limits,
                    numParts));
            break;

        case SMOKE:
            this->m_ParticleProxyContainer.push_back(
                (std::shared_ptr<SmokeParticleProxy>)new SmokeParticleProxy(
                    this->m_ParticleProxyContainer.size() + 1,
                    limits,
                    numParts));
            break;
        }
    }

    void ParticleSystem::Simulate(float deltaTime, glm::vec3 camPos)
    {
        for (auto PP : m_ParticleProxyContainer)
        {
            PP->Simulate(deltaTime, camPos);
        }
    }

} // namespace Epsilon