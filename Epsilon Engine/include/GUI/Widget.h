#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <Shader.h>
#include <functional>
#include <exception>
#include <Types.h>

class Widget {
public:
    Widget() {}
    ~Widget() {}

    virtual void Render(std::shared_ptr<Shader>) {}

    virtual void Update(void) {}

    virtual void OnClick(void) {
        m_OnClickFunc();
    }

    virtual void OnHover(void) {
        m_OnHoverFunc();
    }

    virtual void OnEnter(void) {
        m_OnEnterFunc();
    }

    template<typename T>
    void OnHoverCallback( T func ) {
        m_OnHoverFunc = func;
    }

    template<typename T>
    void OnEntering( T func ) {
        m_OnEnteringFunc = func;
    }

    template<typename T>
    void OnLeaving( T func ) {
        m_OnLeavingFunc = func;
    }

    template<typename T>
    void OnClickCallback( T func ) {
        m_OnClickFunc = func;
    }

    template<typename T>
    void OnEnterCallback( T func ) {
        m_OnEnterFunc = func;
    }

    template<typename T>
    void OnKeyCallback( T func ) {
        m_OnKeyFunc = func;
    }

    void ChangeVisibility() {
        this->m_isHidden = !this->m_isHidden;
    }

    void AnalizeEvents(GUIEVENTS Events) {
        m_Events = Events;
        if(this->m_isHidden)
            return;

        CalculateBoundingBox();
        glm::vec4 Control = getBoundingBox();
        /** TOP LEFT*/    /** BOTTOM LEFT*/   /**TOP RIGHT*/    /**BOTTOM RIGHT*/
        if(Events.MousePosition[0] > Control.x  && Events.MousePosition[0] < Control.y && Events.MousePosition[1] < Control.z  && Events.MousePosition[1] > Control.w) {

            if(m_OnHoverFunc != nullptr && !m_isHidden)
                OnHover();
            isHover = true;
            hasLostFocus = false;
            if(!wasHover) {
                Entering = true;
                wasHover = true;
                if(m_OnEnteringFunc!= nullptr && !m_isHidden)
                    m_OnEnteringFunc();
            } else {
                Entering = false;
                wasHover = true;
            }
            try {
                if(Events.LeftClickWasPressed && m_OnClickFunc != nullptr && !m_isHidden)
                    m_OnClickFunc();
            } catch(exception e) {
                std::cout << e.what() << std::endl;
            }

        } else {
            isHover = false;
            wasHover = false;
            hasLostFocus = true;
            if(!WasFocus) {
                Leaving = false;
                WasFocus = true;
            } else {
                Leaving = true;
                WasFocus = false;
                if(m_OnLeavingFunc != nullptr && !m_isHidden)
                    m_OnLeavingFunc();
            }
            if(m_OnLostFocusFunc != nullptr && !m_isHidden)
                m_OnLostFocusFunc();
        }
    }


    bool m_isHidden = false;

private:
    glm::vec4 getBoundingBox() {
        return glm::vec4(BBL, BBR, BBT, BBB);
    }

    void CalculateBorders() {
        L = (-SizeX/2);
        R = (SizeX/2);
        T = (SizeY/2);
        B = (-SizeY/2);
    }

    void CalculateBoundingBox() {
        BBL = ( - 1 * SizeX + PositionX);
        BBR = (SizeX + PositionX);
        BBT = (SizeY + PositionY);
        BBB = ( - 1 * SizeY + PositionY);
    }


public:
    glm::vec2 m_Position;

protected:
    //void (*m_OnClickFunc)(void) = nullptr;
    std::function<void()> m_OnClickFunc = nullptr;
    std::function<void()> m_OnHoverFunc = nullptr;
    std::function<void()> m_OnLostFocusFunc = nullptr;
    std::function<void()> m_OnEnteringFunc = nullptr;
    std::function<void()> m_OnLeavingFunc = nullptr;
    std::function<void()> m_OnEnterFunc = nullptr;
    std::function<void()> m_OnKeyFunc = nullptr;

    GUIEVENTS m_Events;
    float L, R, T, B;
    float BBL, BBR, BBT, BBB;

    bool isHover = false;
    bool wasHover = false;
    bool hasLostFocus = true;
    bool WasFocus = true;

    bool Entering = false;
    bool Leaving = false;

public:
    float PositionX = 0.0;
    float PositionY = 0.0;

    std::string textureName;

    float SizeX = 0.2;
    float SizeY = 0.2;
    GUIType type;
    float TextSize;
    float textPosition;
protected:
    float WIDTH;
    float HEIGHT;

};
