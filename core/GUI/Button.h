#pragma once

#include <GUI/Widget.h>
#include <Text.h>
#include <string>

namespace Epsilon {
class Button : public Widget
{
public:
    Button(float width, float height, int winWidth, int winHeight, std::string text)
        : m_Width(width), m_Height(height), m_winWidth(winWidth), m_winHeight(winHeight), m_TextString(text) {

        m_Quad = (std::shared_ptr<OpenGLHelpers::Quad>) new OpenGLHelpers::Quad((m_Width*m_winWidth), (m_Height*m_winHeight));


        projection = glm::ortho(0.0f, static_cast<GLfloat>(m_winWidth), 0.0f, static_cast<GLfloat>(m_winHeight));

        this->WIDTH = m_winWidth;
        this->HEIGHT = m_winHeight;
        //type = GUIType::Button;

    }

    ~Button() {
    }

    void Render(std::shared_ptr<Shader> shader) {
        if(m_textHeight <= 0.0)
        {
            m_textWidth = m_TextRendererInstance->getHorizontalLength(m_TextString, 0.8);
            m_textHeight = m_TextRendererInstance->getVerticalLength()*0.8;

            std::cout << m_TextString << " height: " << m_textHeight << std::endl;
        }
        shader->Use();
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glUniform2f(glGetUniformLocation(shader->getProgramID(), "Position"), (this->PositionX*0.5+0.5)*m_winWidth, (this->PositionY*0.5+0.5)*m_winHeight);
        glUniformMatrix4fv(glGetUniformLocation(shader->getProgramID(), "projection"), 1, GL_FALSE, &projection[0][0]);
        if(this->hasLostFocus){
            shader->PushUniform("color", glm::vec4(glm::vec3(0.0f, 0.0f, 0.0f), 0.5f) * 0.5f);
        }
        else if(this->isHover){
            shader->PushUniform("color", glm::vec4(glm::vec3(0.0f, 0.0f, 0.0f), 1.0f));
        }
        else{}
        m_Quad->Render();
        shader->Free();
        this->m_TextRendererInstance->RenderText(m_TextString,/**/(this->PositionX*0.5+0.5)-(m_textWidth*0.5), (this->PositionY*0.5+0.5) - (m_textHeight), 0.8, glm::vec3(1,1,1));
    }

    void Update() {
    }
    std::shared_ptr<Text> m_TextRendererInstance;
private:

    float m_Opacity;
    float m_Width;
    float m_Height;
    float m_winWidth;
    float m_winHeight;
    float m_textWidth = 0.0;
    float m_textHeight = 0.0;
    glm::mat4 projection;
    std::shared_ptr<OpenGLHelpers::Quad> m_Quad;
    std::string m_TextString;
};
}