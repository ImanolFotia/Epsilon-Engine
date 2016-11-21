#pragma once
#include <GL/glew.h>
class SplashScreen
{
public:
    SplashScreen() {}
    ~SplashScreen() {
        /*std::cout << "Splash Destroyed" << std::endl;*/}

    void Draw(GLuint shader, GLuint texture) {

        glUseProgram(shader);
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(shader, "tex"), 0);
        glBindTexture(GL_TEXTURE_2D, texture);
        RenderQuad();
    }

private:


void RenderQuad()
{
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
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

}
    GLuint quadVAO = 0, quadVBO = 0;
};
