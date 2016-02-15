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
    Light(BYTE type, glm::vec3 direction, glm::vec3 position, DECIMAL radius, DECIMAL quadratic, DECIMAL linear, DECIMAL constant)
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
    BYTE getType()
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

    DECIMAL getRadius()
    {
        return this->radius;
    }

    DECIMAL getQuadratic()
    {
        return this->quadratic;
    }

    DECIMAL getLinear()
    {
        return this->linear;
    }

    DECIMAL getConstant()
    {
        return this->constant;
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

    void setRadius(DECIMAL newRadius)
    {
        this->radius = newRadius;
    }

    void setQuadratic(DECIMAL newQuadratic)
    {
        this->quadratic = newQuadratic;
    }

    void setLinear(DECIMAL newLinear)
    {
        this->linear = newLinear;
    }

    void setConstant(DECIMAL newConstant)
    {
        this->constant = newConstant;
    }

private:

    BYTE type;

    glm::vec3 direction;

    glm::vec3 position;

    DECIMAL radius;

    DECIMAL quadratic;

    DECIMAL linear;

    DECIMAL constant;


};

#endif /// LIGHT_H_INCLUDED
