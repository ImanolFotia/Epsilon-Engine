///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#include <pch.hpp>

#include <Helpers.hpp>

#include <camera.h>
#include <math.h>
#include <IO/KeyBoard.h>
#include <IO/Mouse.h>
#include <IO/Joystick/Joystick.h>

#include <Types.h>

#include <Clock.hpp>

namespace Epsilon
{
    Camera::Camera(glm::vec3 cPosition, glm::vec3 cOrientation)
    {
        this->Orientation = glm::vec3(cOrientation);
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
        this->horizontalAngle = 0.0;
        this->verticalAngle = 0.0;
    }

    void Camera::Update(GLFWwindow *win)
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
        auto k = Helpers::getMinimum<double>();
        auto bias = 0.05;

        verticalAngle = Helpers::clamp(verticalAngle, -HPI + bias, HPI - bias);
        horizontalAngle = Helpers::clamp(horizontalAngle, -PI, PI, -k, -1.0);
    }

    void Camera::HandleInputs(GLFWwindow *&window)
    {

        if (glm::isnan(this->Position.x))
            this->Position.x = 0;
        if (glm::isnan(this->Position.y))
            this->Position.y = 0;
        if (glm::isnan(this->Position.z))
            this->Position.z = 0;

        static double LastTime = Clock::TimeSeconds();

        double currentTime = Clock::TimeSeconds();

        float DeltaTime = float(currentTime - LastTime);

        static double lastX = 0.0, lastY = 0.0;

        glfwGetWindowSize(window, &winx, &winy);

        auto _Joystick = Input::Joystick::JoystickManager::PrimaryJoystick();

        float horizontal = 0.0f, vertical = 0.0f;

        if (!_Joystick->getJoystickIsPresent())
        {
            horizontalAngle += MouseSpeed * float(lastX - Input::Mouse::XPOS);
            verticalAngle += MouseSpeed * float(lastY - Input::Mouse::YPOS);

            lastX = Input::Mouse::XPOS;
            lastY = Input::Mouse::YPOS;
        }
        else
        {
            if (!Helpers::inRange(_Joystick->R3().x, -0.1f, 0.1f))
                horizontalAngle -= MouseSpeed + JoystickSensibility * float(_Joystick->R3().x);

            if (!Helpers::inRange(_Joystick->R3().y, -0.1f, 0.1f))
                verticalAngle -= MouseSpeed + JoystickSensibility * float(_Joystick->R3().y);

            lastX = Input::Mouse::XPOS;
            lastY = Input::Mouse::YPOS;
        }

        //horizontalAngle += ()

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
        if (!_Joystick->getJoystickIsPresent())
        {
            if (Input::KeyBoard::KEYS[Input::GLFW::Key::W])
            {
                MovementSpeed = glm::mix(MovementSpeed, this->MaxMovementSpeed, 2.0f * DeltaTime);
                //MovementVector += Orientation * MovementSpeed * DeltaTime;
                Position += Orientation * MovementSpeed * DeltaTime;
            }

            if (Input::KeyBoard::KEYS[Input::GLFW::Key::S])
            {
                MovementSpeed = glm::mix(MovementSpeed, this->MaxMovementSpeed, 2.0f * DeltaTime);
                //MovementVector -= Orientation * MovementSpeed * DeltaTime;
                Position -= Orientation * MovementSpeed * DeltaTime;
            }

            if (Input::KeyBoard::KEYS[Input::GLFW::Key::D])
            {
                MovementSpeed = glm::mix(MovementSpeed, this->MaxMovementSpeed, 2.0f * DeltaTime);
                //MovementVector += Rigth * MovementSpeed * DeltaTime;
                Position += Rigth * MovementSpeed * DeltaTime;
            }

            if (Input::KeyBoard::KEYS[Input::GLFW::Key::A])
            {
                MovementSpeed = glm::mix(MovementSpeed, this->MaxMovementSpeed, 2.0f * DeltaTime);
                //MovementVector -= Rigth * MovementSpeed * DeltaTime;
                Position -= Rigth * MovementSpeed * DeltaTime;
            }

            if (!Input::KeyBoard::KEYS[Input::GLFW::Key::A] &&
                !Input::KeyBoard::KEYS[Input::GLFW::Key::W] &&
                !Input::KeyBoard::KEYS[Input::GLFW::Key::S] &&
                !Input::KeyBoard::KEYS[Input::GLFW::Key::D])
            {
                MovementSpeed = glm::mix(MovementSpeed, 0.0f, 2.0f * DeltaTime);
            }
        }
        /**---------------------------------------------------------------**/

        /** Joystick Camera input**/
        if (_Joystick->getJoystickIsPresent())
        {
            if (Input::KeyBoard::getKey(Input::GLFW::Key::W) || _Joystick->L3().y < -0.1)
            {
                MovementSpeed = glm::mix(MovementSpeed, this->MaxMovementSpeed, 2.0f * DeltaTime);
                Position += Orientation * MovementSpeed * DeltaTime * (_Joystick->L3().y < -0.1 ? glm::abs(_Joystick->L3().y * 2.0f) : 1.0f);
                std::cout << (_Joystick->L3().y < -0.1 ? glm::abs(_Joystick->L3().y * 2.0f) : 1.0f) << std::endl;
                std::cout << Position.x << " " << Position.z << std::endl;
            }

            if (Input::KeyBoard::KEYS[Input::GLFW::Key::S] || _Joystick->L3().y > 0.1)
            {
                MovementSpeed = glm::mix(MovementSpeed, this->MaxMovementSpeed, 2.0f * DeltaTime);
                Position -= Orientation * MovementSpeed * DeltaTime * (_Joystick->L3().y > 0.1 ? _Joystick->L3().y * 2.0f : 1.0f);
            }

            if (Input::KeyBoard::KEYS[Input::GLFW::Key::D] || _Joystick->L3().x > 0.1)
            {
                MovementSpeed = glm::mix(MovementSpeed, this->MaxMovementSpeed, 2.0f * DeltaTime);
                Position += Rigth * MovementSpeed * DeltaTime * (_Joystick->L3().x > 0.1 ? glm::abs(_Joystick->L3().x * 2.0f) : 1.0f);
            }

            if (Input::KeyBoard::KEYS[Input::GLFW::Key::A] || _Joystick->L3().x < -0.1)
            {
                MovementSpeed = glm::mix(MovementSpeed, this->MaxMovementSpeed, 2.0f * DeltaTime);
                Position -= Rigth * MovementSpeed * DeltaTime * (_Joystick->L3().x < -0.1 ? glm::abs(_Joystick->L3().x * 2.0f) : 1.0f);
            }

            if (
                (!Input::KeyBoard::getKey(Input::GLFW::Key::W) || _Joystick->L3().x > -0.1) &&
                (!Input::KeyBoard::KEYS[Input::GLFW::Key::S] || _Joystick->L3().x < 0.1) &&
                (!Input::KeyBoard::KEYS[Input::GLFW::Key::D] || _Joystick->L3().y > -0.1) &&
                (!Input::KeyBoard::KEYS[Input::GLFW::Key::A] || _Joystick->L3().y < 0.1))
            {
                //MovementSpeed = 0.0f; //glm::mix(MovementSpeed, 0.0f, 2.0f * DeltaTime);
                MovementSpeed = glm::mix(MovementSpeed, 0.0f, 2.0f * DeltaTime);
            }
        }
        /** ------------------------------------------------------------------*/

        //MovementSpeed = glm::mix(MovementSpeed, 0.0f, 2.0f * DeltaTime);
        if (MovementSpeed < 0.01)
            MovementSpeed = 0.0f;
        if (LastPosition != Position)
        DeltaVector = Position - LastPosition;
        
        DeltaVector.x = glm::isnan(DeltaVector.x) ? 0.0 : DeltaVector.x;
        DeltaVector.y = glm::isnan(DeltaVector.y) ? 0.0 : DeltaVector.y;
        DeltaVector.z = glm::isnan(DeltaVector.z) ? 0.0 : DeltaVector.z;
        Position += DeltaVector * MovementSpeed * DeltaTime;

        LastTime = currentTime;

        //Position += (MovementVector * MovementSpeed * DeltaTime);
        //Position += MovementVector;
        if(!externallymodified){
            mIsMoving = false;
            //DeltaVector = (Position - LastPosition);
            if (LastPosition != Position)
            {
                //DeltaVector = (Position - LastPosition);
                //MovementVector = glm::normalize(DeltaVector / glm::vec3(glm::sqrt(glm::pow(DeltaVector.x, 2.0) + glm::pow(DeltaVector.y, 2.0) + glm::pow(DeltaVector.z, 2.0))));
                mIsMoving = true;
            }
            if (LastOrientation != Orientation)
                mIsMoving = true;
        } else {
            externallymodified = false;
        }

        LastPosition = Position;
        LastOrientation = Orientation;

        Frustrum = Position + Orientation;
    }

    void Camera::UpdateMatrices(int FrameNumber)
    {
        float Aspectratio;
        Aspectratio = (float)winx / (float)winy;
        static const glm::vec2 Res = glm::vec2((float)winx, (float)winy);
        static const glm::vec2 iRes = 1.0f / glm::vec2((float)winx, (float)winy);

        glm::vec2 Jitter = Helpers::halton(FrameNumber % 16 + 1) * iRes * glm::radians(FieldOfView) * 2.0f;
        

        ProjectionMatrix = glm::perspective(glm::radians(FieldOfView), glm::clamp(Aspectratio, 0.0f, 10.0f), 0.1f, 3000.0f);

        //std::cout << "Jitter.x: " << Jitter.x << "  Jitter.y: " << Jitter.y << "\n";

        ProjectionMatrix[2][0] += Jitter.x;
        ProjectionMatrix[2][1] += Jitter.y;

        PrevView = ViewMatrix;

        ViewMatrix = glm::lookAt(
            Position,
            Position + Orientation,
            glm::vec3(0, 1, 0));
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
        return mIsMoving; //return true to cancel temporal SSR denoiser
    }

    void Camera::isMoving(bool x) {
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

    void Camera::setProjection(float FOV, float AR, float NEAR, float FAR)
    {
        this->ProjectionMatrix = glm::perspective(glm::radians(FOV), AR, NEAR, FAR);
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
