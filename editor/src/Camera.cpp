///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#include <iostream>

#include <include/camera.h>
#include <math.h>

float lerp(float v0, float v1, float t)
{
    return (1-t)*v0 + t*v1;
}

Camera::Camera(glm::vec3 cPosition, glm::vec3 cOrientation, float w, float h)
{
    this->Orientation = glm::vec3(cOrientation);
    this->Position = glm::vec3(cPosition);
    this->FieldOfView = 70.0f;
    this->MovementSpeed = 0.0f;
    this->MouseSpeed = 0.002f;
    this->PositionhasChanged = false;
    this->OrientationhasChanged = false;
    this->MaxMovementSpeed = 10.3;
    //vertical angle: -0.52, horizontal angle: 0.7626
    this->horizontalAngle = 0.7626;
    this->verticalAngle = -0.52;
    this->LastPosition;
    winx = w;
    winy = h;

    float Aspectratio;
    Aspectratio = (float)winy/(float)winx;
    ProjectionMatrix = glm::perspective( glm::radians(FieldOfView) , Aspectratio , 0.1f , 3000.0f );
    Up = glm::vec3(0.0f,1.0f,0.0f);
}

void Camera::Update(float x, float y, float time, ACTION action)
{
    HandleInputs(x, y, time, action);

    GetExternalInputs();
}

void Camera::GetExternalInputs(void)
{
    if(this->OrientationhasChanged)
    {
        this->Orientation = this->newOrientation;
        this->OrientationhasChanged = false;
    }
    if(this->PositionhasChanged)
    {
        this->Position = this->newPosition;
        this->PositionhasChanged = false;
    }
}

void Camera::LockCamera(void)
{
    if(verticalAngle > 1.4208)
        verticalAngle = 1.4208;

    if(verticalAngle < -1.4508)
        verticalAngle = -1.4508;

    if(horizontalAngle < -3.1416)
        horizontalAngle = 3.1414;

    if(horizontalAngle>3.1416)
        horizontalAngle = -3.1414;

}

void Camera::resize(int w, int h){

    this->setWidth(w);
    this->setHeight(h);
    float Aspectratio;
    Aspectratio = (float)w/(float)h;
    ProjectionMatrix = glm::perspective( glm::radians(FieldOfView) , Aspectratio , 0.1f , 3000.0f );
}

void Camera::HandleInputs(float x, float y, float time, ACTION action)
{

    if(glm::isnan(this->Position.x))
        this->Position.x = 0;
    if(glm::isnan(this->Position.y))
        this->Position.y = 0;
    if(glm::isnan(this->Position.z))
        this->Position.z = 0;

    static double LastTime = time;

    double currentTime = time;

    float DeltaTime = float( currentTime - LastTime );

    //double xpos, ypos;
    static double lastX, lastY;

    //glfwGetWindowSize(window , &winx , &winy );
    //glfwGetCursorPos(window, &xpos , &ypos );
    //glfwSetCursorPos(window, winx/2.0, winy/2.0);

    firstx = x;
    firsty = y;
    dX = lastX - firstx;
    dY = lastY - firsty;


    if(action == ROTATE){

        //std::cout << "position: " << Position.x << " " << Position.y << " " << Position.z << std::endl;

        horizontalAngle += MouseSpeed * float( lastX - x ) ;
        verticalAngle   += MouseSpeed * float( lastY - y ) ;
        std::cout << "vertical angle: " << verticalAngle << std::endl;
        glm::vec3 rot = glm::vec3( radius * cos(horizontalAngle), (sin(verticalAngle)) * radius*1.1, radius * sin(horizontalAngle) );
        Position = objectCenter + rot;

        Orientation = glm::normalize(objectCenter - Position);

        //Position += Orientation* glm::length(rot);

        /*Orientation = glm::vec3(
                          cos( verticalAngle )      *       sin( horizontalAngle ),
                          sin( verticalAngle ),
                          cos( verticalAngle )      *       cos( horizontalAngle )
                      );*/

        Rigth = glm::vec3(
                    sin(horizontalAngle     -       3.14f/2.0f),
                    0,
                    cos(horizontalAngle     -       3.14f/2.0f)
                );

        Rigth = glm::cross(Orientation, glm::vec3(0.0, 1.0, 0.0));
        Up = glm::cross( Rigth , Orientation );
        LockCamera();
    }

    lastX = x;
    lastY = y;


    if ( action == ZOOM )
    {
        //std::cout << "position: " << Position.x << " " << Position.y << " " << Position.z << std::endl;
        MovementSpeed = glm::mix(MovementSpeed, this->MaxMovementSpeed, DeltaTime);
        Position += Orientation   * this->ZoomDelta*1.0f *  0.0016f;
       // std::cout << "ZOOM" << std::endl;
        objectCenter = Position +  Orientation * radius;
    }

    if ( 0 )
    {
        MovementSpeed = glm::mix(MovementSpeed, this->MaxMovementSpeed, DeltaTime);
        Position -= Orientation      *    0.0016f;
    }

    if ( action == MOVE )
    {
        //std::cout << "position: " << Position.x << " " << Position.y << " " << Position.z << std::endl;
        MovementSpeed = glm::mix(MovementSpeed, this->MaxMovementSpeed, DeltaTime);
        Position -= Rigth* dX *  0.0016f * MovementSpeed;//Rigth       *     MovementSpeed      *   DeltaTime;
        Position += Up       * dY *   0.0016f * MovementSpeed;
        objectCenter = Position +  Orientation * radius;
    }

/*
    if (!Input::KeyBoard::KEYS[Input::GLFW::Key::A] &&
        !Input::KeyBoard::KEYS[Input::GLFW::Key::W] &&
        !Input::KeyBoard::KEYS[Input::GLFW::Key::S] &&
        !Input::KeyBoard::KEYS[Input::GLFW::Key::D] ){
        MovementSpeed = glm::mix(MovementSpeed, 0.0f, 2.0f * DeltaTime);

    }
*/

    //std::cout  << Orientation.x << Orientation.y << Orientation.z << std::endl;
    LastTime = currentTime;
/*
    Position += (MovementVector * MovementSpeed * DeltaTime);
    if(LastPosition != Position)
    {
        DeltaVector = (Position - LastPosition);
        MovementVector = glm::normalize(DeltaVector / glm::vec3( glm::sqrt(glm::pow(DeltaVector.x, 2.0)  + glm::pow(DeltaVector.y, 2.0) + glm::pow(DeltaVector.z , 2.0) ) ));
    }
*/
    LastPosition = Position;

    Frustrum = Position+Orientation;
}

void Camera::UpdateMatrices(void)
{


    ViewMatrix       = glm::lookAt(
                           Position,
                           Position+Orientation,
                           glm::vec3(0,1,0)
                       );

}

glm::mat4 Camera::getViewMatrix(void)
{
    return this->ViewMatrix;
}

glm::mat4 Camera::getProjectionMatrix(void)
{
    return this->ProjectionMatrix;
}

glm::vec3 Camera::getPosition(void)
{
    return this->Position;
}

glm::vec3 Camera::getDirection(void)
{
    return glm::normalize(this->Orientation);
}

glm::vec3 Camera::getUp(void)
{
    return this->Up;
}
glm::vec3 Camera::getRight(void)
{
    return this->Rigth;
}

glm::vec3 Camera::setPosition(glm::vec3 newPosition)
{
    this->Position = newPosition;
    return newPosition;
}

glm::vec3 Camera::setDirection(glm::vec3 newDirection)
{
    this->Orientation = newDirection;
    return newDirection;
}


void Camera::setOrthogonalProjection()
{
    ProjectionMatrix = glm::ortho(-(winx / 2.0f), winx / 2.0f,
                                  winy / 2.0f, -(winy / 2.0f),
                                1000.0f, -3000.0f);
}

void Camera::setPerspectiveProjection()
{
    float Aspectratio;
    Aspectratio = (float)winx/(float)winy;
    ProjectionMatrix = glm::perspective( glm::radians(FieldOfView) , Aspectratio , 0.1f , 3000.0f );
}
