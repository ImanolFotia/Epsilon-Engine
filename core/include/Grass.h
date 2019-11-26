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
    Grass(const char* path, std::vector<glm::vec3> grassPos)
    {
        int width, height, channels;
        amount  = grassPos.size();
        glGenTextures(1, &grassTexture);
        unsigned char* image = SOIL_load_image(std::string("materials/" + std::string(path)).c_str(), &width, &height, &channels, SOIL_LOAD_RGBA);
        glBindTexture(GL_TEXTURE_2D, grassTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
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


        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);

        glm::mat4* modelMatrices = new glm::mat4[grassPos.size()];
        glm::mat4 model;
        for(unsigned int i = 0; i < grassPos.size(); ++i)
        {
            model = glm::mat4(1);
            model = glm::translate(glm::mat4(1), grassPos[i]);
            model = glm::scale(model, glm::vec3(7.5f));
            modelMatrices[i] = model;
        }

        GLuint buffer;
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, grassPos.size() * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
        // Set attribute pointers for matrix (4 times vec4)
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);

        delete[] modelMatrices;


    }
    virtual ~Grass()
    {
        //std::cout << "Deleted Grass" << std::endl;
    }

public:
    void Render(Shader* shader)
    {
        glDisable(GL_CULL_FACE);
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(shader->getProgramID(), "grassTex"), 0);
        glBindTexture(GL_TEXTURE_2D, grassTexture);

        glBindVertexArray(this->VAO);
        glDrawElementsInstanced(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0, amount);
        glBindVertexArray(0);
        glEnable(GL_CULL_FACE);
    }
private:

    GLuint VBO, EBO, VAO;
    GLuint grassTexture;
    std::vector<glm::mat4> Positions;
    int amount;
    GLfloat GrassVertices[64] =
    {
        0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0, 1.0, 0.0,
        0.5f, -0.5f,  0.0f, 0.0f, 1.0f, 0.0, 1.0, 0.0,
        -0.5f, -0.5f,  0.0f, 1.0f, 1.0f, 0.0, 1.0, 0.0,
        -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0, 1.0, 0.0,

        0.0f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0, 1.0, 0.0,
        0.0f,  0.5f, -0.5f, 0.0f, 0.0f, 0.0, 1.0, 0.0,
        0.0f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0, 1.0, 0.0,
        0.0f, -0.5f,  0.5f, 1.0f, 1.0f, 0.0, 1.0, 0.0
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
