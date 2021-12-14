#pragma once

#include <Entity/Component.h>

namespace Epsilon::Component
{
    class TransformComponent : public Component
    {
    public:
        TransformComponent(glm::vec3 a, glm::vec3 b, glm::quat c) : m_Position(a), m_Scale(b), m_Rotation(c), m_PrevPosition(a), m_PrevScale(b), m_PrevRotation(c) {}

        COMPONENT_TYPE getType() override
        {
            return TRANSFORMCOMPONENT;
        }

        auto Position() { return m_Position; }
        auto Scale() { return m_Scale; }
        auto Rotation() { return m_Rotation; }
        auto PrevPosition() { return m_PrevPosition; }
        auto PrevScale() { return m_PrevScale; }
        auto PrevRotation() { return m_PrevRotation; }

        void Position(auto x, bool ext = false)
        {
            externally_modified_position = ext;
            m_PrevPosition = m_Position;
            m_Position = x;
        }
        void Scale(auto x, bool ext = false)
        {
            externally_modified_scale = ext;
            m_PrevScale = m_Scale;
            m_Scale = x;
        }
        void Rotation(auto x, bool ext = false)
        {
            externally_modified_rotation = ext;
            m_PrevRotation = m_Rotation;
            m_Rotation = x;
        }
        void PrevPosition(auto x) { m_PrevPosition = x; }
        void PrevScale(auto x) { m_PrevScale = x; }
        void PrevRotation(auto x) { m_PrevRotation = x; }

        void Update() override {
            if(!externally_modified_position) {
                m_PrevPosition = m_Position;
            }

            if(!externally_modified_scale) {
                m_PrevScale = m_Scale;
            }
            
            if(!externally_modified_rotation) {
                m_PrevRotation = m_Rotation;
            }
        }

        void Render() override {}

    private:
        glm::vec3 m_Position;
        glm::vec3 m_Scale;
        glm::quat m_Rotation;
        glm::vec3 m_PrevPosition;
        glm::vec3 m_PrevScale;
        glm::quat m_PrevRotation;

        bool externally_modified_position = false;
        bool externally_modified_scale = false;
        bool externally_modified_rotation = false;

        friend EntityBase;
    };

    using TransformComponent_ptr = std::shared_ptr<TransformComponent>;
}