///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#ifndef SUN_H_INCLUDED
#define SUN_H_INCLUDED

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Texture.h>
#include <Shader.h>

class Sun{

public:

    Sun();
    ~Sun()
    {
        //std::cout << "Sun Destroyed" << std::endl;
    }

    void Render(Shader*&);
    void Update();

    glm::vec3 Position;
    glm::vec3 Direction;

private:

    void PrepareVAO();

    float radius;
    float height;

    GLuint TextureID;

    GLuint VAO;
    GLuint VBO, EBO;
};

#endif // SUN_H_INCLUDED
