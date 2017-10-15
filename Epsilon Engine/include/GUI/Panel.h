#pragma once

#include <GUI/Widget.h>
#include <OpenGL/HelperFunctions/Quad.h>
#include <iostream>
#include <glm/glm.hpp>

class Panel : public Widget
{
public:
    Panel(const int winWidth, const int winHeight) : m_winWidth(winWidth), m_winHeight(winHeight) {
        m_Quad = (std::shared_ptr<OpenGLHelpers::Quad>) new OpenGLHelpers::Quad(400, 1080);

        projection = glm::ortho(0.0f, static_cast<GLfloat>(m_winWidth), 0.0f, static_cast<GLfloat>(m_winHeight));
    }

    ~Panel() {}

    void Render(std::shared_ptr<Shader> shader) {
        shader->Use();
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glUniform2f(glGetUniformLocation(shader->getProgramID(), "Position"), 200.0, 360);
        glUniformMatrix4fv(glGetUniformLocation(shader->getProgramID(), "projection"), 1, GL_FALSE, &projection[0][0]);
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
