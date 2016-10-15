#pragma once

#include <iostream>
#include <GUI/Control.h>
#include <GUI/Texture.h>

namespace GUI
{

class Label : public Control
{

public:
    Label(float width, float height)
    {
        this->VAO = 0;
        this->WIDTH = width;
        this->HEIGHT = height;
        this->textObject = new TextGUI("assets/Fonts/arial.ttf", this->WIDTH, this->HEIGHT);
    }

public:
    virtual ~Label() {}

public:
    EGUIAPI Show(float, Renderer::OpenGL3x* renderer)
    {
        //std::cout << "rendering button" << std::endl;
        if(this->VAO == 0)
            this->VAO = renderer->setupVAO(this->VAO);

        textObject->RenderText(this->text, this->PositionX - textObject->lenght, this->PositionY - textObject->height, this->TextSize, glm::vec3(0.9,0.9,0.9));
    }

private:
    GLuint VAO;

};
}
