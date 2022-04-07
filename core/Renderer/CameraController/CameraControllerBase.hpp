#pragma once

#include <glm/glm.hpp>

namespace Epsilon::Renderer
{
    class CameraControllerBase
    {

    protected:
        bool m_Enabled = false;
        glm::vec2 m_MousePosOnViewport;

        glm::vec3 m_Position;
        glm::vec3 m_Angle;

        glm::vec3 m_Direction;

    public:
        CameraControllerBase(glm::vec3 position, glm::vec3 direction) : m_Position(position), m_Direction(direction) {
            
        }

        virtual void onCreate() = 0;
        virtual void onUpdate() = 0;

        // Normalized [-1,1] mouse position on viewport, might be the entire window or not
        void setMousePosOnViewport(glm::vec2 mousePosView)
        {
            m_MousePosOnViewport = mousePosView;
        }

        auto getPosition() -> glm::vec3 {
            return m_Position;
        }
        
        auto getDirection() -> glm::vec3 {
            return m_Direction;
        }
        
        auto setPosition(glm::vec3 position) -> void {
            m_Position = position;
        }
        
        auto setDirection(glm::vec3 direction) -> void {
            m_Direction = direction;
        }

        auto Enable() -> void {
            m_Enabled = true;
        }
        
        auto Disable()-> void {
            m_Enabled = false;
        }

        auto State() -> bool {
            return m_Enabled;
        }
    };
}