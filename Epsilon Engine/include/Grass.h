///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#ifndef GRASS_H_INCLUDED
#define GRASS_H_INCLUDED

#include <GL/glew.h>
#include <string>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Shader.h>

class Grass
{

public:
    Grass(const char* path)
    {
        int width, height, channels;
        glGenTextures(1, &grassTexture);
        unsigned char* image = SOIL_load_image(std::string("materials/" + std::string(path)).c_str(), &width, &height, &channels, SOIL_LOAD_RGBA);
        glBindTexture(GL_TEXTURE_2D, grassTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        SOIL_free_image_data(image);
        glBindTexture(GL_TEXTURE_2D, 0);

        glGenVertexArrays(1, &VAO);

        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GrassVertices), GrassVertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GrassIndices), GrassIndices, GL_STATIC_DRAW);


        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }
    virtual ~Grass() {}

public:
    void Render(Shader* shader)
    {
        glDisable(GL_CULL_FACE);
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(shader->getProgramID(), "grassTex"), 0);
        glBindTexture(GL_TEXTURE_2D, grassTexture);

        glBindVertexArray(this->VAO);
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glEnable(GL_CULL_FACE);
    }
private:

    GLuint VBO, EBO, VAO;
    GLuint grassTexture;
    GLfloat GrassVertices[40] =
    {
         0.5f,  0.5f,  0.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,

         0.0f,  0.5f,  0.5f, 1.0f, 0.0f,
         0.0f,  0.5f, -0.5f, 0.0f, 0.0f,
         0.0f, -0.5f, -0.5f, 0.0f, 1.0f,
         0.0f, -0.5f,  0.5f, 1.0f, 1.0f
    };

    GLuint GrassIndices[12] =
    {
      0, 1, 3,
      1, 2, 3,
      4, 5, 7,
      5, 6, 7
    };

};

#endif /// GRASS_H_INCLUDED
