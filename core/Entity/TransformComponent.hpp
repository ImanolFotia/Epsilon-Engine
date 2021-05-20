#pragma once

#include <Entity/Component.h>

namespace Epsilon::Component
{
    class TransformComponent : public Component
    {
    public:
        TransformComponent(glm::vec3 a, glm::vec3 b, glm::quat c) : m_Position(a), m_Scale(b), m_Rotation(c), m_PrevPosition(a), m_PrevScale(b), m_PrevRotation(c) {}

        COMPONENT_TYPE getType()
        {
            return TRANSFORMCOMPONENT;
        }

        auto Position() { return m_Position; }
        auto Scale() {  return m_Scale; }
        auto Rotation() { return m_Rotation; }
        auto PrevPosition() { return m_PrevPosition; }
        auto PrevScale() { return m_PrevScale; }
        auto PrevRotation() { return m_PrevRotation; }

        
        void Position(auto x) { m_Position = x; }
        void Scale(auto x) {  m_Scale = x; }
        void Rotation(auto x) { m_Rotation = x; }
        void PrevPosition(auto x) { m_PrevPosition = x; }
        void PrevScale(auto x) { m_PrevScale = x; }
        void PrevRotation(auto x) { m_PrevRotation = x; }

        
            void Update() override {}

            void Render() override {}

    private:
        glm::vec3 m_Position;
        glm::vec3 m_Scale;
        glm::quat m_Rotation;
        glm::vec3 m_PrevPosition;
        glm::vec3 m_PrevScale;
        glm::quat m_PrevRotation;
    };

    using TransformComponent_ptr = std::shared_ptr<TransformComponent>;
}