///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#include <Sun.h>

Sun::Sun()
{
    eTexture tex("Sun.png");

    this->TextureID = tex.texture;

    this->radius = 8.0;

    this->height = 10.0;

    PrepareVAO();
}

void Sun::Render(Shader* shader)
{
    glDepthFunc(GL_LEQUAL);
    glBindVertexArray(this->VAO);
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(shader->getProgramID(), "sSampler"), 0);
    glBindTexture(GL_TEXTURE_2D, this->TextureID);

    glUniform3f(glGetUniformLocation(shader->getProgramID(), "sunPos"), this->Position.x, this->Position.y, this->Position.z);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindVertexArray(0);
    glUseProgram(0);
    glDepthFunc(GL_LESS);
}

void Sun::Update()
{
    this->Position = glm::vec3(0,0,0) + glm::vec3(this->radius /** glm::cos(glfwGetTime()/10)*/, height /** glm::sin(glfwGetTime()/10)*/, this->radius /** glm::sin(glfwGetTime()/10)*/);
    this->Direction = glm::normalize(this->Position - glm::vec3(0,0,0));
}

void Sun::PrepareVAO()
{
    static float vertices[] =
    {
        /// Positions        /// Texture Coords
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,    /// Top Left
        -1.0f, -1.0f, 0.0f,  1.0f, 0.0f,    /// Top Right
         1.0f,  1.0f, 0.0f,  0.0f, 0.0f,    /// Bottom Right
         1.0f, -1.0f, 0.0f,  1.0f, 1.0f     /// Bottom Left

    };

    GLuint VBO, EBO;
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);

    glBindBuffer(GL_VERTEX_ARRAY, 0);
    glBindVertexArray(0);

}
