///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#ifndef BSPFACE_H_INCLUDED
#define BSPFACE_H_INCLUDED

#include <glm/glm.hpp>
#include <GL/glad.h>
#include <iostream>
#include <string>
#include <vector>
#include <include/texture.hpp>

struct tBSPLightmap {
    char imageBits[128][128][3];   /// The RGB data in a 128x128 image
};
using namespace std;


class BSPFace
{
public:
    BSPFace()
    {

    }

    virtual ~BSPFace()
    {
        //std::cout << "Destroyed BSP Face" << std::endl;
    }
public:
    bool BuildFace(std::vector<glm::vec3> Vertices,
                   std::vector<glm::vec3> Normals,
                   std::vector<glm::vec2> TexCoords,
                   std::vector<glm::vec2> LMTexCoords,
                   std::vector<unsigned int> Indices,
                   int ID,
                   string imagePath,
                   tBSPLightmap LightMap);


public:

    void RenderFace(GLuint shader, OpenGL::Texture* TextureID,OpenGL::Texture* normalID, OpenGL::Texture* specularID, OpenGL::Texture* metallicID)
    {

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, TextureID->getTextureId());
        glUniform1i(glGetUniformLocation(shader, "texture_diffuse"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularID->getTextureId());
        glUniform1i(glGetUniformLocation(shader, "texture_specular"), 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, normalID->getTextureId());
        glUniform1i(glGetUniformLocation(shader, "texture_normal"), 2);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, metallicID->getTextureId());
        glUniform1i(glGetUniformLocation(shader, "texture_height"), 3);

        glBindVertexArray(this->VAO);
        glDrawElements(GL_TRIANGLES, this->Indices.size(), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);

    }

    void Destroy()
    {
        //std::cout << "Destroyed face" << std::endl;
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteBuffers(1, &UVB);
        glDeleteBuffers(1, &LMUVB);
        glDeleteBuffers(1, &NBO);
        glDeleteBuffers(1, &TBO);
        glDeleteBuffers(1, &BTBO);

        glDeleteTextures(1, &LightMaptexture);

        glDeleteTextures(1, &texture);

        Vertices.clear();
        std::vector<glm::vec3>().swap(Vertices);

        Normals.clear();
        std::vector<glm::vec3>().swap(Normals);

        TexCoords.clear();
        std::vector<glm::vec2>().swap(TexCoords);

        LMTexCoords.clear();
        std::vector<glm::vec2>().swap(LMTexCoords);


        Tangents.clear();
        std::vector<glm::vec3>().swap(Tangents);

        Bitangents.clear();
        std::vector<glm::vec3>().swap(Bitangents);

        Indices.clear();
        std::vector<unsigned int>().swap(Indices);

        VBO = EBO = UVB = LMUVB = NBO = TBO = BTBO = 0;
    }

private:

    bool CalcTangentSpace()
    {

        /// calculate tangent/bitangent vectors of both triangles
        glm::vec3 tangent1, bitangent1;
        for(int i = 0 ; i < (int)Indices.size() ; i+=3)
        {

            glm::vec3 edge1 = Vertices.at(Indices.at((i+1))) - Vertices.at(Indices.at((i)));
            glm::vec3 edge2 = Vertices.at(Indices.at((i+2))) - Vertices.at(Indices.at((i)));

            glm::vec2 deltaUV1 = TexCoords.at(Indices.at((i+1))) - TexCoords.at(Indices.at((i)));
            glm::vec2 deltaUV2 = TexCoords.at(Indices.at((i+2))) - TexCoords.at(Indices.at((i)));

            GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

            tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
            tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
            tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
            /// tangent1 = glm::normalize(tangent1);

            //for(int j = 0 ; j < Tangents.size() ; j++)
            //   tangent1 = tangent1 + Tangents[j];

            tangent1 = glm::normalize(tangent1);
            Tangents.push_back(tangent1);
            Tangents.push_back(tangent1);
            Tangents.push_back(tangent1);
            bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
            bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
            bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
            /// bitangent1 = glm::normalize(bitangent1);

            //for(int j = 0 ; j < Bitangents.size() ; j++)
            //    bitangent1 = bitangent1 + Bitangents[j];

            bitangent1 = glm::normalize(bitangent1);
            Bitangents.push_back(bitangent1);
            Bitangents.push_back(bitangent1);
            Bitangents.push_back(bitangent1);
        }
        return true;
    }

    bool prepareVAO()
    {

        glGenVertexArrays(1, &this->VAO);

        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glGenBuffers(1, &UVB);
        glGenBuffers(1, &LMUVB);
        glGenBuffers(1, &NBO);
        glGenBuffers(1, &TBO);
        glGenBuffers(1, &BTBO);

        glBindVertexArray(this->VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * Vertices.size(), &Vertices[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, UVB);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * TexCoords.size(), &TexCoords[0], GL_STATIC_DRAW);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (GLvoid*)0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, NBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * Normals.size(), &Normals[0], GL_STATIC_DRAW);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, TBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * Tangents.size(), &Tangents[0], GL_STATIC_DRAW);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
        glEnableVertexAttribArray(3);

        glBindBuffer(GL_ARRAY_BUFFER, BTBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * Bitangents.size(), &Bitangents[0], GL_STATIC_DRAW);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
        glEnableVertexAttribArray(4);
        /**
        cout << "SizeOf indices: " << sizeof(unsigned int) * Indices.size() << endl;
        */
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * Indices.size(), &Indices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        return true;
    }


private:

    GLuint VAO;

    GLuint VBO, EBO, UVB, LMUVB, NBO, TBO, BTBO;
    int faceID;
    GLuint texture;
    string imagePath;
    GLuint LightMaptexture;

    std::vector<glm::vec3> Vertices;
    std::vector<glm::vec3> Normals;
    std::vector<glm::vec2> TexCoords;
    std::vector<glm::vec2> LMTexCoords;
    std::vector<glm::vec3> Tangents;
    std::vector<glm::vec3> Bitangents;
    std::vector<unsigned int> Indices;

};

#endif /// BSPFACE_H_INCLUDED
