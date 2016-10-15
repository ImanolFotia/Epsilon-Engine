#pragma once

#include <iostream>
#include <GUI/Control.h>
#include <GUI/Texture.h>

namespace GUI
{

class AlertBox : public Control
{

public:
    AlertBox(float width, float height)
    {
        this->VAO = 0;
        this->WIDTH = width;
        this->HEIGHT = height;
        this->textObject = new TextGUI("assets/Fonts/arial.ttf", this->WIDTH, this->HEIGHT);
    }

public:
    virtual ~AlertBox() {}

public:
    EGUIAPI Show(float, Renderer::OpenGL3x* renderer)
    {
        //std::cout << "rendering button" << std::endl;
        this->PositionX = this->m_Events.MousePosition[0] + this->SizeX;
        this->PositionY = this->m_Events.MousePosition[1];
        if(this->VAO == 0)
            this->VAO = renderer->setupVAO(this->VAO);
        renderer->Render(this->VAO, this->textureID, this->PositionX, this->PositionY, glm::vec2(this->SizeX, this->SizeY), this->WIDTH, this->HEIGHT);
        textObject->RenderText(this->text, this->PositionX - this->SizeX/1.2, this->PositionY - this->SizeY * 0.2, this->TextSize, glm::vec3(0.9,0.9,0.9));
    }

    EGUIAPI ConfirmWidget()
    {
        eTexture tex(textureName.c_str());
        this->textureID = tex.getTextureID();
    }

private:
    GLuint VAO;

};
}
