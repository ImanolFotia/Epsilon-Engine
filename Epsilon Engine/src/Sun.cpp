///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#include <Sun.h>
#include <sys/KeyBoard.h>

Sun::Sun()
{
    eTexture tex("Sun.png");

    this->TextureID = tex.getTextureID();

    this->radius = 30.0;

    this->height = -30.0;

    PrepareVAO();
    mMovement = 1.0;
}

void Sun::Render(Shader*& shader)
{
    glDepthFunc(GL_LEQUAL);
    shader->Use();
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

    if(Input::KeyBoard::KEYS[Input::GLFW::Key::SIX])
        mMovement += 0.01;
    if(Input::KeyBoard::KEYS[Input::GLFW::Key::NINE])
        mMovement -= 0.01;
    this->Position = glm::vec3(0,0,0) + glm::vec3(this->radius * glm::cos(mMovement) /** glm::cos(glfwGetTime()/10)*/, height * glm::sin(mMovement) /** glm::sin(glfwGetTime()/10)*/, 0.0/*this->radius * glm::sin(mMovement)*/ /** glm::sin(glfwGetTime()/10)*/);
    this->Direction = glm::normalize(this->Position - glm::vec3(0,0,0));
}

void Sun::SetUniforms(std::shared_ptr<Camera> cam, Shader*& shader)
{
    glUniform3f(glGetUniformLocation(shader->getProgramID(), "CameraRight"),  cam->getViewMatrix()[0][0], cam->getViewMatrix()[1][0], cam->getViewMatrix()[2][0]);
    glUniform3f(glGetUniformLocation(shader->getProgramID(), "CameraUp"),  cam->getViewMatrix()[0][1], cam->getViewMatrix()[1][1], cam->getViewMatrix()[2][1]);
    glm::mat4 choppedView = glm::mat4(glm::mat3(cam->getViewMatrix()));
    glUniformMatrix4fv(glGetUniformLocation(shader->getProgramID(), "choppedView"), 1, GL_FALSE, &choppedView[0][0]);
    glUniformMatrix4fv(shader->Projection_Location, 1, GL_FALSE, &cam->getProjectionMatrix()[0][0]);
}

void Sun::PrepareVAO()
{
    float vertices[20] =
    {
        /// Positions        /// Texture Coords
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,    /// Top Left
        -1.0f, -1.0f, 0.0f,  1.0f, 0.0f,    /// Top Right
         1.0f,  1.0f, 0.0f,  0.0f, 0.0f,    /// Bottom Right
         1.0f, -1.0f, 0.0f,  1.0f, 1.0f     /// Bottom Left
    };

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

    glBindVertexArray(0);

    delete vertices;

}
