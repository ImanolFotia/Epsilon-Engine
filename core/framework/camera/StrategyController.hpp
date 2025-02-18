#pragma once

#include <core/framework/IO/Mouse.hpp>
#include <core/framework/IO/KeyBoard.hpp>

#include <core/framework/clock.hpp>

#include "CameraControllerBase.hpp"

namespace framework
{
    class StrategyController : public CameraControllerBase
    {
        const float m_cMaxSpeed = 0.01;
        const float m_cMaxDragSpeed = 0.05;
        const float m_cDeadZoneDistance = 0.99;

        float m_Pitch = 0.5f;
        float m_Yaw = 0.5f;
        float m_Speed = 0.0f;
        float m_VerticalSpeed = 0.0f;

        bool m_UsingWheel = false;

        glm::vec2 m_Movement = glm::vec2(0.0f);
        glm::vec2 m_LastMouse = glm::vec2(0.0f);

    public:
        StrategyController() : CameraControllerBase(glm::vec3(0.f), glm::vec3(0.f)) {}

        void Init(glm::vec3 position, glm::vec3 direction, float pitch, float yaw) 
        {
            m_Position = position;
            m_Direction = direction;
            m_Pitch = pitch;
            m_Yaw = yaw;

            onCreate();
        }

        virtual void onCreate() override
        {
            framework::Input::KeyBoard::KeyboardEventHandler += beacon::bind(&StrategyController::onKeyPress, this);
            framework::Input::Mouse::MouseEventHandler += beacon::bind(&StrategyController::onMouseWheel, this);
            framework::Input::Mouse::MouseEventHandler += beacon::bind(&StrategyController::onMouseWheelHold, this);
        }

        void onKeyPress(beacon::sender *sender, beacon::args *args)
        {
            auto obj = args->to<framework::Input::KeyboardArgs>();
        };

        void onMouseWheel(beacon::sender *sender, beacon::args *args)
        {
            auto obj = args->to<framework::Input::MouseArgs>();

            if (args == nullptr || !m_Enabled)
                return;


            if (obj.Wheel().State == framework::Input::UP || obj.Wheel().State == framework::Input::DOWN)
            {
                if(m_Position.y > 12.0f || m_Position.y < 28.0f)
                    m_Position.y -= (obj.Wheel().State == framework::Input::UP ? 0.1 : -0.1);
                m_Position.y = glm::clamp(m_Position.y, 11.0f, 30.0f);
                m_VerticalSpeed = (obj.Wheel().State == framework::Input::UP ? -0.05 : 0.05);
            }
        }

        void onMouseWheelHold(beacon::sender *sender, beacon::args *args)
        {
            auto obj = args->to<framework::Input::MouseArgs>();

            if (args == nullptr || !m_Enabled)
                return;


            if (obj.Middle().State == framework::Input::PRESSED)
            {
                m_Movement = glm::vec2(0.0);
                m_UsingWheel = true;
            }
            else if (obj.Middle().State == framework::Input::RELEASED)
            {
                m_UsingWheel = false;
                m_Movement = glm::vec2(0.0);
            }

            if (m_UsingWheel)
            {
                glm::vec2 current_mouse = glm::vec2(framework::Input::Mouse::XPOS, framework::Input::Mouse::YPOS)*0.1f;
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
                m_Position += glm::vec3(m_Movement.x, 0.0, m_Movement.y) * m_cMaxDragSpeed;
            }
            m_LastMouse = glm::vec2(obj.X(), obj.Y())*0.1f;
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

        StrategyController operator=(const StrategyController&) {
            return *this;
        }

        bool UsingWheel() {
            return m_UsingWheel;
        }
    private:
        void pollEvents()
        {
            if (!m_Enabled) return; 

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

            auto tempMouse = m_MousePosOnViewport;

            float l = glm::length(tempMouse);
            m_MousePosOnViewport = glm::clamp(m_MousePosOnViewport, glm::vec2(-1.0f), glm::vec2(1.0f));
            float l2 = glm::length(tempMouse);
            float speed_mod = glm::mix(0.0, 1.0, (l2 - 0.8f) * 5.0f);

            tempMouse = glm::normalize(tempMouse);

            if (!m_UsingWheel)
            {
                if ((glm::abs(m_MousePosOnViewport).x > m_cDeadZoneDistance || glm::abs(m_MousePosOnViewport).y > m_cDeadZoneDistance) && m_Enabled && !m_UsingWheel)
                {
                    m_Movement = glm::normalize(Right) * tempMouse.x;
                    m_Movement += glm::normalize(Orientation) * tempMouse.y;
                    m_Speed = glm::mix(m_Speed, m_cMaxSpeed * speed_mod, framework::Clock::DeltaSeconds() * 5.0f);
                    m_Position += glm::normalize(glm::vec3(m_Movement.x, 0.0, m_Movement.y)) * m_Speed;
                }
                else
                {
                    m_Speed = glm::mix(m_Speed, 0.0f, framework::Clock::DeltaSeconds() * 5.0f);
                    m_Position += glm::vec3(m_Movement.x, 0.0, m_Movement.y) * m_Speed;
                }
            }
            m_VerticalSpeed = glm::mix(m_VerticalSpeed, 0.0f, framework::Clock::DeltaSeconds() * 2.0);
            m_Position.y += 0.1 * m_VerticalSpeed;
            m_Position.y = glm::clamp(m_Position.y, 11.0f, 30.0f);

        }


    };
}