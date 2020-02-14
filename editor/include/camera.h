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
#include <iostream>
#include <include/Types.h>

class Camera{

public:

    Camera(glm::vec3, glm::vec3, float w, float h);

    ~Camera(){
    }

public:

    void Update(float x, float y, float time, ACTION action);

    void UpdateMatrices(void);

    glm::mat4 getViewMatrix(void);

    glm::mat4 getProjectionMatrix(void);

    glm::vec3 getPosition(void);

    glm::vec3 getDirection(void);

    glm::vec3 getUp(void);

    glm::vec3 getRight(void);

    glm::vec3 setPosition(glm::vec3 newPosition);

    glm::vec3 setDirection(glm::vec3 newDirection);

    void resize(int w, int h);

    void setOrthogonalProjection();

    void setPerspectiveProjection();

    void setWidth(int w){this->winx = w;}
    void setHeight(int h){this->winy = h;}

private:

    void LockCamera(void);

    void HandleInputs(float x, float y, float time, ACTION action);

    void GetExternalInputs(void);

public:

    glm::mat4 MVP;


    float MovementSpeed;
    float MouseSpeed;
    float FieldOfView;
    float ZoomDelta;
    int winx;
    int winy;
    float MaxMovementSpeed;

private:

    float radius = 5.0f;
    glm::vec3 objectCenter;
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
    float firstx, firsty = 0.0;
    float dX = 0.0, dY = 0.0;
    double lastX = 0.0, lastY = 0.0;

};

#endif // CAMERA_H_INCLUDED
