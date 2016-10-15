#pragma once
#include <iostream>
#include <GUI/Control.h>
#include <GUI/Texture.h>

namespace GUI
{

class Button : public Control
{

public:
    Button(float width, float height)
    {
        this->VAO = 0;
        this->WIDTH = width;
        this->HEIGHT = height;
        this->textObject = new TextGUI("resources/arial.ttf", this->WIDTH, this->HEIGHT);
    }

public:
    virtual ~Button() {}

public:
    EGUIAPI Show(float, Renderer::OpenGL3x* renderer)
    {
        //std::cout << "rendering button" << std::endl;
        if(this->VAO == 0)
            this->VAO = renderer->setupVAO(this->VAO);

        renderer->Render(this->VAO, this->textureID, this->PositionX, this->PositionY, glm::vec2(this->SizeX, this->SizeY), this->WIDTH, this->HEIGHT); //glm::vec2(textObject->lenght, textObject->height), this->WIDTH, this->HEIGHT);
        textObject->RenderText(this->text, this->PositionX - textObject->lenght, this->PositionY - textObject->height, this->TextSize, glm::vec3(0.9,0.9,0.9));
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
