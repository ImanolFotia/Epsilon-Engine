///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#include <Sun.h>
#include <IO/KeyBoard.h>

namespace Epsilon
{
    Sun::Sun()
    {
        try
        {
            tex = std::make_shared<eTexture>("Sun.png");
            TextureID = tex->getTextureID();

            radius = 20.0;

            height = 10.0;

            Position = glm::vec3(0.0, -50.0, 3.0);

            PrepareVAO();
            std::cout << "Llega" << std::endl;
            std::cout << "Sun texture ID " << TextureID << std::endl;
            //mMovement = 2.6;
            mMovement = 1.0;
        }
        catch (std::exception &e)
        {
            std::cout << e.what() << std::endl;
        }
    }

    void Sun::Render(std::shared_ptr<Shader> shader)
    {
        if (mMovement > 0.0)
        {
            glDepthFunc(GL_LEQUAL);
            shader->Use();
            glBindVertexArray(VAO);
            glActiveTexture(GL_TEXTURE0);
            glUniform1i(glGetUniformLocation(shader->getProgramID(), "sSampler"), 0);
            glBindTexture(GL_TEXTURE_2D, TextureID);

            glUniform3f(glGetUniformLocation(shader->getProgramID(), "sunPos"), Position.x + radius, Position.y + radius, Position.z + radius);
            glCache::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glBindVertexArray(0);
            glCache::glUseProgram(0);
            glDepthFunc(GL_LESS);
        }
    }

    void Sun::Update()
    {

        if (Input::KeyBoard::KEYS[Input::GLFW::Key::SIX])
            mMovement += 0.01;
        if (Input::KeyBoard::KEYS[Input::GLFW::Key::NINE])
            mMovement -= 0.01;
        Direction = glm::normalize(Position + glm::vec3(radius));
    }

    void Sun::SetUniforms(std::shared_ptr<Camera> cam, std::shared_ptr<Shader> shader)
    {
        glUniform3f(glGetUniformLocation(shader->getProgramID(), "CameraRight"), cam->getViewMatrix()[0][0], cam->getViewMatrix()[1][0], cam->getViewMatrix()[2][0]);
        glUniform3f(glGetUniformLocation(shader->getProgramID(), "CameraUp"), cam->getViewMatrix()[0][1], cam->getViewMatrix()[1][1], cam->getViewMatrix()[2][1]);
        glm::mat4 choppedView = glm::mat4(glm::mat3(cam->getViewMatrix()));
        glUniformMatrix4fv(glGetUniformLocation(shader->getProgramID(), "choppedView"), 1, GL_FALSE, &choppedView[0][0]);
        glUniformMatrix4fv(shader->Projection_Location, 1, GL_FALSE, &cam->getProjectionMatrix()[0][0]);
    }

    void Sun::PrepareVAO()
    {
        float vertices[20] =
            {
                /// Positions        /// Texture Coords
                -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  /// Top Left
                -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, /// Top Right
                1.0f, 1.0f, 0.0f, 0.0f, 0.0f,   /// Bottom Right
                1.0f, -1.0f, 0.0f, 1.0f, 1.0f   /// Bottom Left
            };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)0);

        glBindVertexArray(0);
    }
} // namespace Epsilon