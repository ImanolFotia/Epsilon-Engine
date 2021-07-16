///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#ifndef SUN_H_INCLUDED
#define SUN_H_INCLUDED

#include <pch.hpp>
#include <Core.hpp>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Texture.h>
#include <Shader.h>
#include <camera.h>

namespace Epsilon
{
class Sun{

public:

    Sun();
    ~Sun()
    {
        //std::cout << "Sun Destroyed" << std::endl;
    }

    void Render(std::shared_ptr<Shader>);
    void Update();
    void SetUniforms(std::shared_ptr<Camera>, std::shared_ptr<Shader>);

    void Destroy()
    {
        glDeleteTextures(1, &this->TextureID);
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    glm::vec3 Position;
    glm::vec3 Direction;

    float height = 1.0;
    float radius;
private:

    std::shared_ptr<eTexture> tex;
    void PrepareVAO();


    GLuint TextureID;

    float mMovement;

    GLuint VAO;
    GLuint VBO, EBO;
};
}

#endif // SUN_H_INCLUDED
