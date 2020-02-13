#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <GL/glad.h>
#include <iostream>
#include <thread>
#include "helpers.h"
#include <include/texture.hpp>

class Terrain{
public:
    Terrain(int, double, double, glm::vec3);
    ~Terrain(){
        std::cout << "destroyed terrain" << std::endl;

    }

    void Destroy();

    void setTexture(std::string tex)
    {
        texture->Destroy();
        delete texture;
        texture = nullptr;

        texture = new OpenGL::Texture();
        texture->Load(tex);

    }
    void setTextureRepeat(int value)
    {
        textureRepeat = value;
    }

    void Render(GLuint);

private:

    void genVertexData();
    void genVertexArrayObject();

private:

    struct TVertex
    {
        /// Position
        glm::vec3 Position;
        /// TexCoords
        glm::vec2 TexCoords;

        glm::vec3 Normal;
    };

    struct Face
    {
        glm::vec3 vertex0;
        glm::vec3 vertex1;
        glm::vec3 vertex2;
    };

    std::vector<unsigned int> m_Indices;
    std::vector<Face> m_Faces;
    std::vector<TVertex> m_Vertices;
    std::vector<glm::vec3> m_Normals;
    glm::vec3 m_Position;

    int m_Size;
    double m_Roughness;
    double m_Height;
    GLuint m_VAO;
    GLuint m_VBO;
    GLuint m_EBO;
    int textureRepeat = 1;
    bool generated = false;
    std::thread th;
    OpenGL::Texture* texture;

};

