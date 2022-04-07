#pragma once

#include <IO/Mouse.h>
#include <IO/KeyBoard.h>

#include <Clock.hpp>

#include "CameraControllerBase.hpp"

namespace Epsilon::Renderer
{
    class StrategyController : public CameraControllerBase
    {
        const float m_cMaxSpeed = 1.0;
        const float m_cDeadZoneDistance = 0.8;

        float m_Pitch = 0.5f;
        float m_Yaw = 0.5f;
        float m_Speed = 0.0f;
        float m_VerticalSpeed = 0.0f;

        bool m_UsingWheel = false;

        glm::vec2 m_Movement = glm::vec2(0.0f);
        glm::vec2 m_LastMouse = glm::vec2(0.0f);

    public:
        StrategyController() = default;

        StrategyController(glm::vec3 position, glm::vec3 direction, float pitch, float yaw) : CameraControllerBase(position, direction),
                                                                                              m_Pitch(pitch), m_Yaw(yaw)
        {
            onCreate();
        }

        virtual void onCreate() override
        {
            Epsilon::Input::KeyBoard::KeyboardEventHandler += beacon::bind(&StrategyController::onKeyPress, this);
            Epsilon::Input::Mouse::MouseEventHandler += beacon::bind(&StrategyController::onMouseWheel, this);
            Epsilon::Input::Mouse::MouseEventHandler += beacon::bind(&StrategyController::onMouseWheelHold, this);
        }

        void onKeyPress(beacon::sender *sender, beacon::args *args)
        {
            auto obj = args->to<Input::KeyboardArgs>();
        };

        void onMouseWheel(beacon::sender *sender, beacon::args *args)
        {
            auto obj = args->to<Input::MouseArgs>();

            if (args == nullptr || !m_Enabled)
                return;

            using Input::STATE::UP, Input::STATE::DOWN;

            if (obj.Wheel().State == UP || obj.Wheel().State == DOWN)
            {
                m_Position.y -= (obj.Wheel().State == UP ? 0.2 : -0.2);
                m_Position.y = glm::clamp(m_Position.y, 0.0f, 30.0f);
                m_VerticalSpeed = (obj.Wheel().State == UP ? -0.1 : 0.1);
            }
        }

        void onMouseWheelHold(beacon::sender *sender, beacon::args *args)
        {
            auto obj = args->to<Input::MouseArgs>();

            if (args == nullptr || !m_Enabled)
                return;

            using Input::STATE::PRESSED, Input::STATE::RELEASED;

            if (obj.Middle().State == PRESSED)
            {
                m_Movement = glm::vec2(0.0);
                m_UsingWheel = true;
            }
            else if (obj.Middle().State == RELEASED)
            {
                m_UsingWheel = false;
                m_Movement = glm::vec2(0.0);
            }

            if (m_UsingWheel)
            {
                glm::vec2 current_mouse = glm::vec2(obj.X(), obj.Y());
                glm::vec2 delta_mouse = current_mouse - m_LastMouse;
                glm::vec2 Orientation = glm::vec2(
                    glm::cos(m_Pitch) * glm::sin(m_Yaw),
                    glm::cos(m_Pitch) * glm::cos(m_Yaw));

                glm::vec2 Right = glm::vec2(
                    glm::sin(m_Yaw - HPI),
                    glm::cos(m_Yaw - HPI));

                m_Direction = glm::vec3(Orientation.x, sin(m_Pitch), Orientation.y);

                m_Movement = -glm::normalize(Right) * delta_mouse.x;
                m_Movement += glm::normalize(Orientation) * delta_mouse.y;
                m_Position += glm::vec3(m_Movement.x, 0.0, m_Movement.y) * 0.1f;
            }
            m_LastMouse = glm::vec2(obj.X(), obj.Y());
        }

        virtual void onUpdate() override
        {
            pollEvents();
        }

        auto getPitch() -> float
        {
            return m_Pitch;
        }

        auto getYaw() -> float
        {
            return m_Yaw;
        }

        void setYaw(float yaw)
        {
            m_Yaw = yaw;
        }

        void setPitch(float pitch)
        {
            m_Pitch = pitch;
        }

        auto getOrientation() -> glm::vec2 {
            
            glm::vec2 Orientation = glm::vec2(
                glm::cos(m_Pitch) * glm::sin(m_Yaw),
                glm::cos(m_Pitch) * glm::cos(m_Yaw));

                return Orientation;
        }

        auto getRight() -> glm::vec2 {
            constexpr float HPI = 3.14159265359 * 0.5;
            glm::vec2 Right = glm::vec2(
                glm::sin(m_Yaw - HPI),
                glm::cos(m_Yaw - HPI));
            return Right;
        }

    private:
        void pollEvents()
        {

            if(m_Yaw < -3.14159265359) m_Yaw = 3.14159265359;
            if(m_Yaw > 3.14159265359) m_Yaw = -3.14159265359;
            constexpr float HPI = 3.14159265359 * 0.5;

            float yaw = m_Yaw;
            float pitch = m_Pitch;

            glm::vec2 Orientation = glm::vec2(
                glm::cos(m_Pitch) * glm::sin(m_Yaw),
                glm::cos(m_Pitch) * glm::cos(m_Yaw));

            glm::vec2 Right = glm::vec2(
                glm::sin(m_Yaw - HPI),
                glm::cos(m_Yaw - HPI));

            m_Direction = glm::vec3(Orientation.x, sin(m_Pitch), Orientation.y);

            float l = glm::length(m_MousePosOnViewport);
            m_MousePosOnViewport = glm::clamp(m_MousePosOnViewport, glm::vec2(-1.0f), glm::vec2(1.0f));
            float l2 = glm::length(m_MousePosOnViewport);
            float speed_mod = glm::mix(0.0, 1.0, (l2 - 0.8f) * 5.0f);

            if (!m_UsingWheel)
            {
                if (l > m_cDeadZoneDistance && m_Enabled && !m_UsingWheel)
                {
                    m_Movement = glm::normalize(Right) * m_MousePosOnViewport.x;
                    m_Movement += glm::normalize(Orientation) * m_MousePosOnViewport.y;
                    m_Speed = glm::mix(m_Speed, m_cMaxSpeed * speed_mod, Clock::DeltaSeconds() * 5.0f);
                    m_Position += glm::normalize(glm::vec3(m_Movement.x, 0.0, m_Movement.y)) * m_Speed;
                }
                else
                {
                    m_Speed = glm::mix(m_Speed, 0.0f, Clock::DeltaSeconds() * 5.0f);
                    m_Position += glm::vec3(m_Movement.x, 0.0, m_Movement.y) * m_Speed;
                }
            }

            m_VerticalSpeed = glm::mix(m_VerticalSpeed, 0.0f, Clock::DeltaSeconds() * 2.0);
            m_Position.y += 0.1 * m_VerticalSpeed;
        }
    };
}