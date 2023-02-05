///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================
#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "glm/gtc/quaternion.hpp"

#if USE_GLFW
#include "GLFW/glfw3.h"
#endif

#ifndef BUILD_ANDROID
#include "core/framework/IO/KeyBoard.hpp"
#include "core/framework/IO/Mouse.hpp"
#include "core/framework/IO/Joystick/Joystick.hpp"
#endif

#include "../../window.hpp"

#include "beacon/beacon.hpp"
#include <iostream>

namespace utils
{
    class Camera
    {
    public:

    public:
        Camera(glm::vec3, glm::vec3);

        ~Camera()
        {
            std::cout << "Deleted Camera" << std::endl;
        }

    public:
        void Update(framework::Window::windowType *);

        glm::vec2 UpdateMatrices(int, int, int, bool jitter = true);

        glm::mat4 getViewMatrix(void);

        glm::mat4 getPrevViewMatrix(void);

        glm::mat4 getProjectionMatrix(void);

        glm::vec3 getPosition(void);

        glm::vec3 getDirection(void);

        glm::vec3 getUp(void);

        glm::vec3 getRight(void);

        void setPosition(glm::vec3 newPosition);

        void setDirection(glm::vec3 newDirection);

        void setFoV(float FoV);

        float getFoV();

        void setProjection(float FOV, float AR, float NEAR, float FAR);

        void setProjection(glm::mat4);

        void setViewMatrix(glm::mat4);

        bool isMoving();

        void isMoving(bool);

        void onMouseMoveCallback(beacon::sender *sender, beacon::args *args)
        {
        }

        void onMouseWheelCallback(beacon::sender *sender, beacon::args *args);
        
        float getNearPlane() { return near_plane; }
        float getFarPlane() { return far_plane; }

    private:
        void LockCamera(void);

        void HandleInputs(framework::Window::windowType *&);

        void GetExternalInputs(void);


    public:
        glm::mat4 MVP;

        float MovementSpeed;
        float MouseSpeed;
        float JoystickSensibility;
        float FieldOfView;
        int winx;
        int winy;
        float MaxMovementSpeed;
        bool mIsMoving = false;
        bool externallymodified = true;
        float jitter_multiplier = 1.0;

    private:
        glm::vec3 Rigth;
        glm::vec3 Up;
        glm::vec3 Frustrum;
        glm::vec3 newOrientation;
        glm::vec3 newPosition;
        glm::mat4 PrevView;
        bool OrientationhasChanged;
        bool PositionhasChanged;
        double verticalAngle;
        double horizontalAngle;
        float near_plane;
        float far_plane;
        glm::mat4 ViewMatrix;
        glm::mat4 ProjectionMatrix;
        glm::vec3 Position;
        glm::vec3 Orientation;
        glm::vec3 LastOrientation;
        glm::vec3 LastPosition;
        glm::vec3 DeltaVector, MovementVector;
    };
} // namespace Epsilon
#endif // CAMERA_H_INCLUDED
