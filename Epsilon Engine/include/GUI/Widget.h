#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <Shader.h>

class Widget{
public:
    Widget(){}
    ~Widget(){}

    virtual void Render();

    virtual void Update();

    void OnClick(){m_OnClick();}

    void OnHover(){m_OnHover();}

    void OnHoverCallback(std::function func){m_OnHover = func;}

    void OnClickCallback(std::function func){m_OnClick = func;}

private:
    glm::vec2 m_Position;

private:
    std::function m_OnClick;
    std::function m_OnHover;
};
