///========= Copyright Imanol Fotia, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#include "camera.hpp"
#include <math.h>
#include <core/framework/IO/IO.hpp>
#include "core/framework/def.hpp"
#include <core/framework/clock.hpp>

namespace utils
{
    Camera::Camera(glm::vec3 cPosition, glm::vec3 cOrientation)
    {
        this->Orientation = glm::normalize(glm::vec3(cOrientation));
        this->Position = glm::vec3(cPosition);
        LastPosition = glm::vec3(cPosition);
        DeltaVector = glm::vec3(0.0f);
        this->Up = glm::vec3(0.0f, 1.0f, 0.0f);
        this->Rigth = glm::cross(Orientation, Up);
        this->FieldOfView = 65.0f;
        this->MovementSpeed = 0.0f;
        this->MouseSpeed = 0.002f;
        this->JoystickSensibility = 0.05f;
        this->PositionhasChanged = false;
        this->OrientationhasChanged = false;
        this->MaxMovementSpeed = 5.3;

        this->horizontalAngle = glm::atan(this->Orientation.x, this->Orientation.z) ;
        this->verticalAngle = glm::asin(this->Orientation.y);
        near_plane = 0.01;
        far_plane = 512.0f;
    }

    void Camera::onMouseWheelCallback(beacon::sender *sender, beacon::args *args)
    {
        if (args == nullptr)
            return;

        using framework::Input::STATE::UP, framework::Input::STATE::DOWN;
        auto obj = args->to<framework::Input::MouseArgs>();

        if (obj.Wheel().State == UP)
        {
            Position = Position + Orientation * (float)obj.Wheel().yOffset;
        }
        else if (obj.Wheel().State == DOWN)
        {
            Position = Position - Orientation * (float)obj.Wheel().yOffset;
        }
    }

    void Camera::Update(framework::Window::windowType *win)
    {
        HandleInputs(win);

        GetExternalInputs();
    }

    void Camera::GetExternalInputs(void)
    {
        if (this->OrientationhasChanged)
        {
            this->Orientation = this->newOrientation;
            this->OrientationhasChanged = false;
        }
        if (this->PositionhasChanged)
        {
            this->Position = this->newPosition;
            this->PositionhasChanged = false;
        }
    }

    void Camera::LockCamera(void)
    {
        auto k = getMinimum<double>();
        auto bias = 0.05;

        verticalAngle = glm::clamp(verticalAngle, -HPI + bias, HPI - bias);
        horizontalAngle = clamp(horizontalAngle, -PI, PI, -k, -1.0);
    }

    void Camera::HandleInputs(framework::Window::windowType *&window)
    {

        if (glm::isnan(this->Position.x))
            this->Position.x = 0;
        if (glm::isnan(this->Position.y))
            this->Position.y = 0;
        if (glm::isnan(this->Position.z))
            this->Position.z = 0;

        static double LastTime = framework::Clock::TimeSeconds();

        double currentTime = framework::Clock::TimeSeconds();

        float DeltaTime = float(currentTime - LastTime);

        DeltaTime = glm::clamp(DeltaTime, 0.0f, 0.1f);

        // auto winSize =

        // glfwGetWindowSize(window, &winx, &winy);

        auto _Joystick = framework::Input::Joystick::JoystickManager::PrimaryJoystick();

        float horizontal = 0.0f, vertical = 0.0f;

        if (resetDeltas) {
            lastX = framework::Input::Mouse::XPOS;
            lastY = framework::Input::Mouse::YPOS;
            resetDeltas = false;

            this->horizontalAngle = glm::atan(this->Orientation.x, this->Orientation.z);
            this->verticalAngle = glm::asin(this->Orientation.y);
            return;
        }

        if (!_Joystick->getJoystickIsPresent())
        {
            horizontalAngle += MouseSpeed * float(lastX - framework::Input::Mouse::XPOS);
            verticalAngle += MouseSpeed * float(lastY - framework::Input::Mouse::YPOS);

            lastX = framework::Input::Mouse::XPOS;
            lastY = framework::Input::Mouse::YPOS;
        }
        else
        {
            if (!inRange(_Joystick->R3().x, -0.1f, 0.1f))
                horizontalAngle -= MouseSpeed + JoystickSensibility * float(_Joystick->R3().x);

            if (!inRange(_Joystick->R3().y, -0.1f, 0.1f))
                verticalAngle -= MouseSpeed + JoystickSensibility * float(_Joystick->R3().y);

            lastX = framework::Input::Mouse::XPOS;
            lastY = framework::Input::Mouse::YPOS;
        }


        LockCamera();

        Orientation = glm::vec3(
            cos(verticalAngle) * sin(horizontalAngle),
            sin(verticalAngle),
            cos(verticalAngle) * cos(horizontalAngle));

        Rigth = glm::vec3(
            sin(horizontalAngle - HPI),
            0,
            cos(horizontalAngle - HPI));

        Orientation = glm::normalize(Orientation);

        Up = glm::cross(Rigth, Orientation);

        /** Keyboard Camera input**/
#if USE_GLFW
        if (!_Joystick->getJoystickIsPresent())
        {
            if (framework::Input::KeyBoard::KEYS[framework::Input::GLFW::Key::W])
            {
                MovementSpeed = glm::mix(MovementSpeed, this->MaxMovementSpeed, 2.0f * DeltaTime);
                Position += Orientation * MovementSpeed * DeltaTime;
            }

            if (framework::Input::KeyBoard::KEYS[framework::Input::GLFW::Key::S])
            {
                MovementSpeed = glm::mix(MovementSpeed, this->MaxMovementSpeed, 2.0f * DeltaTime);
                Position -= Orientation * MovementSpeed * DeltaTime;
            }

            if (framework::Input::KeyBoard::KEYS[framework::Input::GLFW::Key::D])
            {
                MovementSpeed = glm::mix(MovementSpeed, this->MaxMovementSpeed, 2.0f * DeltaTime);
                Position += Rigth * MovementSpeed * DeltaTime;
            }

            if (framework::Input::KeyBoard::KEYS[framework::Input::GLFW::Key::A])
            {
                MovementSpeed = glm::mix(MovementSpeed, this->MaxMovementSpeed, 2.0f * DeltaTime);
                Position -= Rigth * MovementSpeed * DeltaTime;
            }

            if (!framework::Input::KeyBoard::KEYS[framework::Input::GLFW::Key::A] &&
                !framework::Input::KeyBoard::KEYS[framework::Input::GLFW::Key::W] &&
                !framework::Input::KeyBoard::KEYS[framework::Input::GLFW::Key::S] &&
                !framework::Input::KeyBoard::KEYS[framework::Input::GLFW::Key::D])
            {
                MovementSpeed = glm::mix(MovementSpeed, 0.0f, 2.0f * DeltaTime);
            }
        } else
        /**---------------------------------------------------------------**/

        /** Joystick Camera input**/
        if (_Joystick->getJoystickIsPresent())
        {
            if (framework::Input::KeyBoard::getKey(framework::Input::GLFW::Key::W) || _Joystick->L3().y < -0.1)
            {
                MovementSpeed = glm::mix(MovementSpeed, this->MaxMovementSpeed, 2.0f * DeltaTime);
                Position += Orientation * MovementSpeed * DeltaTime * (_Joystick->L3().y < -0.1 ? glm::abs(_Joystick->L3().y * 2.0f) : 2.0f);
            }

            if (framework::Input::KeyBoard::KEYS[framework::Input::GLFW::Key::S] || _Joystick->L3().y > 0.1)
            {
                MovementSpeed = glm::mix(MovementSpeed, this->MaxMovementSpeed, 2.0f * DeltaTime);
                Position -= Orientation * MovementSpeed * DeltaTime * (_Joystick->L3().y > 0.1 ? _Joystick->L3().y * 2.0f : 1.0f);
            }

            if (framework::Input::KeyBoard::KEYS[framework::Input::GLFW::Key::D] || _Joystick->L3().x > 0.1)
            {
                MovementSpeed = glm::mix(MovementSpeed, this->MaxMovementSpeed, 2.0f * DeltaTime);
                Position += Rigth * MovementSpeed * DeltaTime * (_Joystick->L3().x > 0.1 ? glm::abs(_Joystick->L3().x * 2.0f) : 1.0f);
            }

            if (framework::Input::KeyBoard::KEYS[framework::Input::GLFW::Key::A] || _Joystick->L3().x < -0.1)
            {
                MovementSpeed = glm::mix(MovementSpeed, this->MaxMovementSpeed, 2.0f * DeltaTime);
                Position -= Rigth * MovementSpeed * DeltaTime * (_Joystick->L3().x < -0.1 ? glm::abs(_Joystick->L3().x * 2.0f) : 1.0f);
            }

            if (
                (!framework::Input::KeyBoard::getKey(framework::Input::GLFW::Key::W) && _Joystick->L3().x > -0.1) &&
                (!framework::Input::KeyBoard::KEYS[framework::Input::GLFW::Key::S] && _Joystick->L3().x < 0.1) &&
                (!framework::Input::KeyBoard::KEYS[framework::Input::GLFW::Key::D] && _Joystick->L3().y > -0.1) &&
                (!framework::Input::KeyBoard::KEYS[framework::Input::GLFW::Key::A] && _Joystick->L3().y < 0.1))
            {
                MovementSpeed = glm::mix(MovementSpeed, 0.0f, 2.0f * DeltaTime);
            }
        }

#endif
        /** ------------------------------------------------------------------*/

        if (MovementSpeed < 0.01f)
            MovementSpeed = 0.0f;
        if (LastPosition != Position)
            DeltaVector = Position - LastPosition;

        DeltaVector.x = glm::isnan(DeltaVector.x) ? 0.0 : DeltaVector.x;
        DeltaVector.y = glm::isnan(DeltaVector.y) ? 0.0 : DeltaVector.y;
        DeltaVector.z = glm::isnan(DeltaVector.z) ? 0.0 : DeltaVector.z;
        Position += DeltaVector * MovementSpeed * DeltaTime;

        LastTime = currentTime;

        if (!externallymodified)
        {
            mIsMoving = false;

            if (LastPosition != Position)
            {
                mIsMoving = true;
            }
            if (LastOrientation != Orientation)
                mIsMoving = true;
        }
        else
        {
            externallymodified = false;
        }
    }

    glm::vec2 Camera::UpdateMatrices(int FrameNumber, int frame_w, int frame_h, bool jitter)
    {
        if (frame_w == 0 or frame_h == 0) {
            return glm::vec2(0.0f);
        }
        LastPosition = Position;
        LastOrientation = Orientation;

        Frustrum = Position + Orientation;
        winx = frame_w;
        winy = frame_h;
        float Aspectratio = (float)frame_w /(float)frame_h;
        Aspectratio = (float)winx / (float)winy;
        const glm::vec2 Res = glm::vec2((float)winx, (float)winy);
        const glm::vec2 iRes = 1.0f / glm::vec2((float)winx, (float)winy);

        if(m_pProjType == Perspective)
            ProjectionMatrix = glm::perspective(glm::radians(FieldOfView), glm::clamp(Aspectratio, -10.0f, 10.0f), near_plane, far_plane);
        else
            ProjectionMatrix = glm::ortho(-(float)winx/60.0f, (float)winx / 60.0f, -(float)winy /60.0f, (float)winy / 60.0f, -500.0f, 500.0f);

        glm::vec2 Jitter = glm::vec2(0.0f);;
        if (jitter)
        {
            glm::vec2 Sample = halton(FrameNumber % 16 + 1);
            glm::vec2 Jitter = ((Sample * 2.0f - 1.0f)) / Res;

            Jitter *= jitter_multiplier;
            ProjectionMatrix[2][0] += Jitter.x;
            ProjectionMatrix[2][1] += Jitter.y;
        }
        PrevView = ViewMatrix;

        ViewMatrix = glm::lookAt(
            Position,
            Position + Orientation,
            glm::vec3(0.0f, 1.0f, 0.0f));

        return Jitter;
    }

    glm::mat4 Camera::getViewMatrix(void)
    {
        return this->ViewMatrix;
    }

    glm::mat4 Camera::getProjectionMatrix(void)
    {
        return this->ProjectionMatrix;
    }

    bool Camera::isMoving()
    {
        return true; // mIsMoving; //return true to cancel temporal SSR denoiser
    }

    void Camera::isMoving(bool x)
    {
        externallymodified = true;
        mIsMoving = x;
    }

    glm::vec3 Camera::getPosition(void)
    {
        return this->Position;
    }

    glm::vec3 Camera::getDirection(void)
    {
        return this->Orientation;
    }

    glm::vec3 Camera::getUp(void)
    {
        return this->Up;
    }
    glm::vec3 Camera::getRight(void)
    {
        return this->Rigth;
    }

    void Camera::setPosition(glm::vec3 newPosition)
    {
        this->Position = newPosition;
    }

    void Camera::setDirection(glm::vec3 newDirection)
    {
        this->Orientation = newDirection;
    }

    glm::mat4 Camera::getPrevViewMatrix()
    {
        return PrevView;
    }

    void Camera::setFoV(float FoV)
    {
        this->FieldOfView = FoV;
    }

    float Camera::getFoV()
    {
        return this->FieldOfView;
    }

    void Camera::setProjection(float FOV, float AR, float near_plane, float far_plane)
    {
        this->ProjectionMatrix = glm::perspective(glm::radians(FOV), AR, near_plane, far_plane);
    }

    void Camera::setProjection(glm::mat4 newProj)
    {
        this->ProjectionMatrix = newProj;
    }

    void Camera::setViewMatrix(glm::mat4 newView)
    {
        this->ViewMatrix = newView;
    }
} // namespace Epsilon
