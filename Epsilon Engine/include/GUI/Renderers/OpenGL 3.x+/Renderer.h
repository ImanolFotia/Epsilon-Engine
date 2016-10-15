#pragma once

#include <GL/glew.h>
#include <Shader.h>
#include <GUI/Renderers/OpenGL 3.x+/shaders.h>
#include <glm/glm.hpp>
#include <Shader.h>

namespace Renderer{
class OpenGL3x
{
public:
    OpenGL3x() = default;
    ~OpenGL3x() = default;

public:
    GLuint createVAO(GLuint VAO)
    {
        InitializeShader();
        glGenVertexArrays(1, &VAO);
        return VAO;
    }

    void bindVAO(GLuint VAO)
    {
        glBindVertexArray(VAO);
    }

    void releaseVAO(GLuint VAO)
    {
        glBindVertexArray(0);
    }

    GLuint createVBO(GLuint VBO)
    {
        glGenBuffers(1, &VBO);
        return VBO;
    }

    GLuint InitializeShader()
    {
        Shader sh("shaders/vertex.glsl", "shaders/fragment.glsl");
        shader = sh.getProgramID();
    }

    GLuint setupVAO(GLuint VAO)
    {
        float vertices[20] =
        {
            /// Positions        /// Texture Coords
            -1.0f,  1.0f, 0.0f,  1.0f, 0.0f,    /// Top Left
            -1.0f, -1.0f, 0.0f,  1.0f, 1.0f,    /// Top Right
            1.0f,  1.0f, 0.0f,  0.0f, 0.0f,    /// Bottom Right
            1.0f, -1.0f, 0.0f,  0.0f, 1.0f     /// Bottom Left
        };
        GLuint VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(sizeof(float) * 3));

        glBindVertexArray(0);

        return VAO;
    }

    void Render(GLuint VAO, GLuint texID, float posx, float posy, glm::vec2 glyph, float viewportx, float viewporty)
    {
        glUseProgram(shader);
        glBindVertexArray(VAO);
        glUniform2f(glGetUniformLocation(shader, "position"), posx, posy);
        glUniform2f(glGetUniformLocation(shader, "glyph"), glyph.x, glyph.y);
        glUniform1i(glGetUniformLocation(shader, "viewportx"), viewportx);
        glUniform1i(glGetUniformLocation(shader, "viewporty"), viewporty);

        glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(shader, "texture1"), 0);
        glBindTexture(GL_TEXTURE_2D, texID);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
        glUseProgram(0);
    }

private:
    GLuint shader;

};
}
