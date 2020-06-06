///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#ifndef LIGHT_H_INCLUDED
#define LIGHT_H_INCLUDED

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Types.h>

class Light{

public:
    Light(short type, glm::vec3 direction, glm::vec3 position, float radius, float quadratic, float linear, float constant)
    {
        this->type = type;
        this->direction = direction;
        this->position = position;
        this->radius = radius;
        this->quadratic = quadratic;
        this->linear = linear;
        this->constant = constant;
    }

    virtual ~Light(){}

    /** Getters */
public:
    short getType()
    {
        return this->type;
    }

    glm::vec3 getDirection()
    {
        return this->direction;
    }

    glm::vec3 getPosition()
    {
        return this->position;
    }

    float getRadius()
    {
        return this->radius;
    }

    float getQuadratic()
    {
        return this->quadratic;
    }

    float getLinear()
    {
        return this->linear;
    }

    float getConstant()
    {
        return this->constant;
    }

    t_light toStruct()
    {
        return t_light();
    }

        /** Setters */
public:
    void setType(BYTE newType)
    {
        this->type = newType;
    }

    void setDirection(glm::vec3 newDirection)
    {
        this->direction = newDirection;
    }

    void setPosition(glm::vec3 newPosition)
    {
        this->position = newPosition;
    }

    void setRadius(float newRadius)
    {
        this->radius = newRadius;
    }

    void setQuadratic(float newQuadratic)
    {
        this->quadratic = newQuadratic;
    }

    void setLinear(float newLinear)
    {
        this->linear = newLinear;
    }

    void setConstant(float newConstant)
    {
        this->constant = newConstant;
    }

private:

    short type;

    glm::vec3 direction;

    glm::vec3 position;

    float radius;

    float quadratic;

    float linear;

    float constant;


};

#endif /// LIGHT_H_INCLUDED
