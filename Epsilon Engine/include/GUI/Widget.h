#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <Shader.h>
#include <functional>
#include <Types.h>

class Widget {
public:
    Widget() {}
    ~Widget() {}

    virtual void Render(std::shared_ptr<Shader>) {}

    virtual void Update(void) {}

    void OnClick(void) {
        m_OnClickFunc();
    }

    void OnHover(void) {
        m_OnHoverFunc();
    }

    void OnHoverCallback(void(*func)()) {
        m_OnHoverFunc = func;
    }

    void OnClickCallback( void(*func)()) {
        m_OnClickFunc = func;
    }

    void AnalizeEvents(GUIEVENTS Events)
    {
        m_Events = Events;
        if(this->Hidden)
            return;

        CalculateBoundingBox();
        glm::vec4 Control = getBoundingBox();
        /** TOP LEFT*/    /** BOTTOM LEFT*/   /**TOP RIGHT*/    /**BOTTOM RIGHT*/
        if(Events.MousePosition[0] > Control.x  && Events.MousePosition[0] > Control.x    && Events.MousePosition[0] < Control.y  &&  Events.MousePosition[0] < Control.y)
        {
            if(Events.MousePosition[1] < Control.z  && Events.MousePosition[1] > Control.w   && Events.MousePosition[1] < Control.z  &&  Events.MousePosition[1] > Control.w)
            {
                if(m_OnHoverFunc != nullptr)
                    OnHover();
                if(Events.LeftClickWasPressed && m_OnClickFunc != nullptr)
                    m_OnClickFunc();
            }
            else
            {
                if(m_OnLostFocusFunc != nullptr)
                    m_OnLostFocusFunc();
            }
        }
        else
        {
            if(m_OnLostFocusFunc != nullptr)
                m_OnLostFocusFunc();
        }
    }

bool Hidden = false;
bool m_isHidden = false;

private:
    glm::vec4 getBoundingBox()
    {
        return glm::vec4(BBL, BBR, BBT, BBB);
    }

private:
    void CalculateBorders()
    {
        L = (-SizeX/2);
        R = (SizeX/2);
        T = (SizeY/2);
        B = (-SizeY/2);
    }

private:
    void CalculateBoundingBox()
    {
        BBL = ( - 1 * SizeX + PositionX);
        BBR = (SizeX + PositionX);
        BBT = (SizeY + PositionY);
        BBB = ( - 1 * SizeY + PositionY);
    }

private:
    glm::vec2 m_Position;

private:
    void (*m_OnClickFunc)(void) = nullptr;
    void (*m_OnHoverFunc)(void) = nullptr;
    void (*m_OnLostFocusFunc)(void) = nullptr;

    GUIEVENTS m_Events;
    float L, R, T, B;
    float BBL, BBR, BBT, BBB;

public:
    float PositionX = 0.0;
public:
    float PositionY = 0.0;

public:
    std::string textureName;

public:
    float SizeX = 0.2;
public:
    float SizeY = 0.2;

public:
    float TextSize;
public:
    float textPosition;
protected:
    float WIDTH;
protected:
    float HEIGHT;
};
