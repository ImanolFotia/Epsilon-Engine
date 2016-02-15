///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================
#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtc/quaternion.hpp>
#include <GLFW/glfw3.h>

class Camera{

public:

    Camera(glm::vec3, glm::vec3);

    ~Camera(){}

public:

    void Update(GLFWwindow*);

    void UpdateMatrices(void);

    glm::mat4 getViewMatrix(void);

    glm::mat4 getProjectionMatrix(void);

    glm::vec3 getPosition(void);

    glm::vec3 getDirection(void);

    glm::vec3 setPosition(glm::vec3 newPosition);

    glm::vec3 setDirection(glm::vec3 newDirection);

private:

    void LockCamera(void);

    void HandleInputs(void);

    void GetExternalInputs(void);

public:

    glm::mat4 MVP;


    float MovementSpeed;
    float MouseSpeed;
    float FieldOfView;
    int winx;
    int winy;
    float MaxMovementSpeed;

private:

    glm::vec3 Rigth;
    glm::vec3 Up;
    glm::vec3 Frustrum;
    glm::vec3 newOrientation;
    glm::vec3 newPosition;
    bool OrientationhasChanged;
    bool PositionhasChanged;
    float verticalAngle;
    float horizontalAngle;
    glm::mat4 ViewMatrix;
    glm::mat4 ProjectionMatrix;
    glm::vec3 Position;
    glm::vec3 Orientation;
    glm::vec3 LastPosition;
    glm::vec3 DeltaVector, MovementVector;

    GLFWwindow* window;

};

#endif // CAMERA_H_INCLUDED
