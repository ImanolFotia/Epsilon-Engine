#pragma once

#include <Widget.h>

class Panel : public Widget
{
public:
    Panel() {
        m_Shader = (std::shared_ptr<Shader>) new Shader("", "");
    }
    ~Panel() {}

    virtual Render(void) {
        if (quadVAO == 0)
        {
            GLfloat quadVertices[] =
            {
                // Positions         //Texture Coords
                -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 0.0f, 1.0f, 0.0f
            };
            // Setup plane VAO
            glGenVertexArrays(1, &quadVAO);
            glGenBuffers(1, &quadVBO);
            glBindVertexArray(quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        }
        m_Shader->Use();
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
        m_Shader->Free();
    }

    virtual Update(void) {}

private:
    float m_Opacity;
    float m_Width;
    float m_Height;

    GLuint quadVAO;
    GLuint quadVBO;

    std::shared_ptr<> m_Shader;
};
