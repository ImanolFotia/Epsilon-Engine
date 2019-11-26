#pragma once

#include <GUI/Widget.h>
#include <OpenGL/HelperFunctions/Quad.h>
#include <iostream>
#include <glm/glm.hpp>

class Panel : public Widget
{
public:
    Panel(const int winWidth, const int winHeight, const int width, const int height, const glm::vec2 pos)
    : m_winWidth(winWidth), m_winHeight(winHeight), m_Width(width), m_Height(height) {
        m_Quad = (std::shared_ptr<OpenGLHelpers::Quad>) new OpenGLHelpers::Quad(width, height);
        m_Position = pos;
        projection = glm::ortho(0.0f, static_cast<GLfloat>(m_winWidth), 0.0f, static_cast<GLfloat>(m_winHeight));
       // type = GUIType::Panel;
    }

    ~Panel() {}

    void Render(std::shared_ptr<Shader> shader) {
        shader->Use();
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glUniform2f(glGetUniformLocation(shader->getProgramID(), "Position"), (m_Position.x*0.5+0.5)*m_winWidth, (m_Position.y*0.5+0.5)*m_winHeight);
        glUniformMatrix4fv(glGetUniformLocation(shader->getProgramID(), "projection"), 1, GL_FALSE, &projection[0][0]);
        shader->PushUniform("color", glm::vec4(glm::vec3(0.0f, 0.0f, 0.0f), 0.5f) * 0.5f);
        m_Quad->Render();
        shader->Free();
    }

    void Update(void) {}

private:
    float m_Opacity;
    float m_Width;
    float m_Height;
    float m_winWidth;
    float m_winHeight;

    std::shared_ptr<OpenGLHelpers::Quad> m_Quad;

    glm::mat4 projection;
};
