///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#include <iostream>

#include <camera.h>
#include <math.h>
#include <sys/KeyBoard.h>
#include <sys/Mouse.h>
#include <sys/Joystick.h>

float lerp(float v0, float v1, float t)
{
    return (1-t)*v0 + t*v1;
}

Camera::Camera(glm::vec3 cPosition, glm::vec3 cOrientation)
{
    this->Orientation = glm::vec3(cOrientation);
    this->Position = glm::vec3(cPosition);
    this->FieldOfView = 65.0f;
    this->MovementSpeed = 0.0f;
    this->MouseSpeed = 0.002f;
    this->JoystickSensibility = 0.05f;
    this->PositionhasChanged = false;
    this->OrientationhasChanged = false;
    this->MaxMovementSpeed = 3.3;
    this->horizontalAngle = 0.0;
    this->verticalAngle = 0.0;
}

void Camera::Update(GLFWwindow*& win)
{
    HandleInputs(win);

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

    if(verticalAngle >= 1.5707)
        verticalAngle = 1.5707;

    if(verticalAngle <= -1.5707)
        verticalAngle = -1.5707;

    if(horizontalAngle <= -3.1416)
        horizontalAngle = 3.1416;

    if(horizontalAngle >= 3.1416)
        horizontalAngle = -3.1416;

}

void Camera::HandleInputs(GLFWwindow*& window)
{

    if(glm::isnan(this->Position.x))
        this->Position.x = 0;
    if(glm::isnan(this->Position.y))
        this->Position.y = 0;
    if(glm::isnan(this->Position.z))
        this->Position.z = 0;

    static double LastTime = glfwGetTime();

    double currentTime = glfwGetTime();

    float DeltaTime = float( currentTime - LastTime );

    static double lastX, lastY;

    glfwGetWindowSize(window , &winx , &winy );

    if(!Input::Joystick::JoystickIsPresent) {
        horizontalAngle += MouseSpeed * float( lastX - Input::Mouse::XPOS ) ;
        verticalAngle   += MouseSpeed * float( lastY - Input::Mouse::YPOS ) ;

        lastX = Input::Mouse::XPOS;
        lastY = Input::Mouse::YPOS;

    }
    else
    {

        horizontalAngle -= JoystickSensibility * float( Input::Joystick::JoystickAxes[2] ) ;
        verticalAngle   += JoystickSensibility * float( Input::Joystick::JoystickAxes[3] ) ;
    }

    LockCamera();

    Orientation = glm::vec3(
                      cos( verticalAngle )      *       sin( horizontalAngle ),
                      sin( verticalAngle ),
                      cos( verticalAngle )      *       cos( horizontalAngle )
                  );

    Rigth = glm::vec3(
                sin(horizontalAngle     -       3.14f/2.0f),
                0,
                cos(horizontalAngle     -       3.14f/2.0f)
            );


    Orientation = glm::normalize(Orientation);

    Up = glm::cross( Rigth , Orientation );

    /** Keyboard Camera input**/
    if(!Input::Joystick::JoystickIsPresent) {
        if ( Input::KeyBoard::KEYS[Input::GLFW::Key::W])
        {
            MovementSpeed = glm::mix(MovementSpeed, this->MaxMovementSpeed, 2.0f * DeltaTime);
            Position += Orientation   *     MovementSpeed      *   DeltaTime;
        }

        if ( Input::KeyBoard::KEYS[Input::GLFW::Key::S])
        {
            MovementSpeed = glm::mix(MovementSpeed, this->MaxMovementSpeed, 2.0f * DeltaTime);
            Position -= Orientation   *     MovementSpeed      *   DeltaTime;
        }

        if ( Input::KeyBoard::KEYS[Input::GLFW::Key::D])
        {
            MovementSpeed = glm::mix(MovementSpeed, this->MaxMovementSpeed, 2.0f * DeltaTime);
            Position += Rigth       *     MovementSpeed      *   DeltaTime;
        }

        if ( Input::KeyBoard::KEYS[Input::GLFW::Key::A])
        {
            MovementSpeed = glm::mix(MovementSpeed, this->MaxMovementSpeed, 2.0f * DeltaTime);
            Position -= Rigth       *     MovementSpeed      *   DeltaTime;
        }

        if (!Input::KeyBoard::KEYS[Input::GLFW::Key::A] &&
                !Input::KeyBoard::KEYS[Input::GLFW::Key::W] &&
                !Input::KeyBoard::KEYS[Input::GLFW::Key::S] &&
                !Input::KeyBoard::KEYS[Input::GLFW::Key::D] ) {
            MovementSpeed = glm::mix(MovementSpeed, 0.0f, 2.0f * DeltaTime);

        }
    }
    /**---------------------------------------------------------------**/

    /** Joystick Camera input**/
    if(Input::Joystick::JoystickIsPresent) {
        if ( Input::KeyBoard::KEYS[Input::GLFW::Key::W] || Input::Joystick::JoystickAxes[1] > 0.0)
        {
            MovementSpeed = glm::mix(MovementSpeed, this->MaxMovementSpeed, 2.0f * DeltaTime);
            Position += Orientation   *     MovementSpeed      *  DeltaTime *    (glm::abs(Input::Joystick::JoystickAxes[1] * 2));
        }

        if ( Input::KeyBoard::KEYS[Input::GLFW::Key::S] || Input::Joystick::JoystickAxes[1] < 0.0 )
        {
            MovementSpeed = glm::mix(MovementSpeed, this->MaxMovementSpeed, 2.0f * DeltaTime);
            Position -= Orientation   *     MovementSpeed      *   DeltaTime * (glm::abs(Input::Joystick::JoystickAxes[1] * 2));
        }

        if ( Input::KeyBoard::KEYS[Input::GLFW::Key::D]  || Input::Joystick::JoystickAxes[0] > 0.0)
        {
            MovementSpeed = glm::mix(MovementSpeed, this->MaxMovementSpeed, 2.0f * DeltaTime);
            Position += Rigth       *     MovementSpeed      *   DeltaTime * (glm::abs(Input::Joystick::JoystickAxes[0] * 2));
        }

        if ( Input::KeyBoard::KEYS[Input::GLFW::Key::A]  || Input::Joystick::JoystickAxes[0] < 0.0)
        {
            MovementSpeed = glm::mix(MovementSpeed, this->MaxMovementSpeed, 2.0f * DeltaTime);
            Position -= Rigth       *     MovementSpeed      *   DeltaTime * (glm::abs(Input::Joystick::JoystickAxes[0] * 2));
        }

        if (!Input::KeyBoard::KEYS[Input::GLFW::Key::A] &&
                !Input::KeyBoard::KEYS[Input::GLFW::Key::W] &&
                !Input::KeyBoard::KEYS[Input::GLFW::Key::S] &&
                !Input::KeyBoard::KEYS[Input::GLFW::Key::D] ) {
            MovementSpeed = glm::mix(MovementSpeed, 0.0f, 2.0f * DeltaTime);

        }
    }
    /** ------------------------------------------------------------------*/

    LastTime = currentTime;

    Position += (MovementVector * MovementSpeed * DeltaTime);
    mIsMoving = false;
    if(LastPosition != Position)
    {
        DeltaVector = (Position - LastPosition);
        MovementVector = glm::normalize(DeltaVector / glm::vec3( glm::sqrt(glm::pow(DeltaVector.x, 2.0)  + glm::pow(DeltaVector.y, 2.0) + glm::pow(DeltaVector.z , 2.0) ) ));
        mIsMoving = true;
    }

    LastPosition = Position;

    Frustrum = Position+Orientation;
}

void Camera::UpdateMatrices(void)
{


    float Aspectratio;
    Aspectratio = (float)winx/(float)winy;

    ProjectionMatrix = glm::perspective( glm::radians(FieldOfView) , glm::max(Aspectratio, 1.0f) , 0.1f , 3000.0f );

    PrevView = ViewMatrix;

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

bool Camera::isMoving()
{
    return mIsMoving;
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
    this->ProjectionMatrix = glm::perspective( glm::radians(FOV) , AR , NEAR , FAR );
}

void Camera::setProjection(glm::mat4 newProj)
{
    this->ProjectionMatrix = newProj;
}

void Camera::setViewMatrix(glm::mat4 newView)
{
    this->ViewMatrix = newView;
}
