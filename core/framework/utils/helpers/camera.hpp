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

        enum ProyectionType {
            Perspective,
            Orthographic
        };

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
        
        float getNearPlane() { 
            return near_plane;
        }
        float getFarPlane() { return far_plane; }

        void ResetDeltas() {
            resetDeltas = true;
        }

        ProyectionType ProjType() {
            return m_pProjType;
        }

        void ProjType(ProyectionType proj_type) {
            m_pProjType = proj_type;
        }
    private:
        void LockCamera(void);

        void HandleInputs(framework::Window::windowType *&);

        void GetExternalInputs(void);



    public:
        glm::mat4 MVP = glm::mat4(1.0);

        float MovementSpeed{};
        float MouseSpeed{};
        float JoystickSensibility{};
        float FieldOfView{};
        int winx{};
        int winy{};
        float MaxMovementSpeed{};
        bool mIsMoving = false;
        bool externallymodified = true;
        float jitter_multiplier = 1.0;
        ProyectionType m_pProjType = Perspective;

    private:
        glm::vec3 Rigth{};
        glm::vec3 Up{};
        glm::vec3 Frustrum{};
        glm::vec3 newOrientation{};
        glm::vec3 newPosition{};
        glm::mat4 PrevView{};
        bool OrientationhasChanged = false;
        bool PositionhasChanged = false;
        double verticalAngle = 0.0;
        double horizontalAngle = 0.0;
        float near_plane = 0.001;
        float far_plane = 3000.0;
        glm::mat4 ViewMatrix = glm::mat4(1.0f);
        glm::mat4 ProjectionMatrix = glm::mat4(1.0f);
        glm::vec3 Position{};
        glm::vec3 Orientation{};
        glm::vec3 LastOrientation{};
        glm::vec3 LastPosition{};
        glm::vec3 DeltaVector{}, MovementVector{};

        double lastX = 0.0, lastY = 0.0;

        bool resetDeltas = false;
    };
} // namespace Epsilon
#endif // CAMERA_H_INCLUDED
