#pragma once
#include <string>
#include <GUI/Renderers/OpenGL 3.x+/Renderer.h>
#include <GUI/TextGUI.h>
#include <GUI/Types.h>
#include <functional>
namespace GUI
{

class Control
{
public:
    Control() {};
public:
    virtual ~Control() = default;

public:
    virtual EGUIAPI Show(float, Renderer::OpenGL3x*) = 0;


public:
    float PositionX;
public:
    float PositionY;

public:
    std::string textureName;

public:
    float SizeX;
public:
    float SizeY;

public:
    float TextSize;
public:
    float textPosition;
public:
    bool Hidden;
protected:
    float WIDTH;
protected:
    float HEIGHT;

protected:
    TextGUI* textObject;
public:
    std::string text;
protected:
    GLuint textureID;

public:
    void onClick(std::function<void(void)> functionCallback)
    {
        std::cout << "click" << std::endl;
        ClickCallback = functionCallback;
    }
public:
    void onHover(void(*functionCallback)())
    {
        HoverCallback = functionCallback;
    }
public:
    void onFocus(void(*functionCallback)())
    {
        FocusCallback = functionCallback;
    }

public:
    void onLostFocus(void(*functionCallback)())
    {
        LostFocusCallback = functionCallback;
    }
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

public:
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
                if(HoverCallback != nullptr)
                    HoverCallback();
                if(Events.LeftClickWasPressed && ClickCallback != nullptr)
                    ClickCallback();
            }
            else
            {
                if(LostFocusCallback != nullptr)
                    LostFocusCallback();
            }
        }
        else
        {
            if(LostFocusCallback != nullptr)
                LostFocusCallback();
        }
    }

private:
    //void (*ClickCallback)(void) = nullptr;
    std::function<void(void)>ClickCallback;
private:
    void (*HoverCallback)(void) = nullptr;
private:
    void (*FocusCallback)(void) = nullptr;
private:
    void (*LostFocusCallback)(void) = nullptr;


    float L, R, T, B;
    float BBL, BBR, BBT, BBB;
    protected:
    GUIEVENTS m_Events;
};

}
