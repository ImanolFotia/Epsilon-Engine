#pragma once

#include <Entity/Component.h>

namespace Epsilon
{
    namespace Component
    {

        class MovementComponent : public Component
        {
        public:
            MovementComponent(std::shared_ptr<Physics::PhysicObject> rigidBody, glm::vec3 from, glm::vec3 to, float speed, bool loop)
            {
                mType = MOVEMENTCOMPONENT;
                RigidBodyPointer = rigidBody;
                m_FromPos = from;
                m_ToPos = to;
                m_Speed = glm::abs(speed);
                m_Loop = loop;
            }
            ~MovementComponent() {}

            COMPONENT_TYPE getType() { return mType; }

            void Update()
            {

                m_Interpolation = glm::clamp(m_Rate, 0.0f, 1.0f);

                if (m_Loop)
                {
                    if (m_Interpolation >= 1.0)
                        m_Up = false;
                    else if (m_Interpolation <= 0.0)
                        m_Up = true;
                    else if (m_Interpolation >= 0.0 && m_Interpolation <= 1.0 && !m_Up)
                    {
                        m_Up = false;
                    }
                    else if (m_Interpolation >= 0.0 && m_Interpolation <= 1.0 && m_Up)
                    {
                        m_Up = true;
                    }

                    if (m_Up)
                        m_Rate += m_Speed * ResourceManager::Get().timestep;
                    else
                        m_Rate -= m_Speed * ResourceManager::Get().timestep;
                }
                else
                {
                    m_Rate += m_Speed * ResourceManager::Get().timestep;
                }

                m_CurrentPosition = glm::mix(m_FromPos, m_ToPos, m_Interpolation);

                if (RigidBodyPointer != nullptr)
                {
                    btVector3 deltaPos = btVector3(m_CurrentPosition.x, m_CurrentPosition.y, m_CurrentPosition.z) - RigidBodyPointer->Body->getCenterOfMassPosition();
                    RigidBodyPointer->Body->translate(btVector3(deltaPos));
                }
            }

            void Render() {}

            virtual void setTransparency(bool x) {}

        private:
            COMPONENT_TYPE mType;
            glm::vec3 m_FromPos;
            glm::vec3 m_ToPos;
            glm::vec3 m_CurrentPosition;
            float m_Speed;
            float m_Interpolation = 0.0;
            float m_Rate = 0;
            bool m_Loop;
            bool m_Up = true;
            std::shared_ptr<Physics::PhysicObject> RigidBodyPointer;

        protected:
        };
    } // namespace Component
} // namespace Epsilon