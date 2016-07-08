///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#ifndef TERRAIN_H_INCLUDED
#define TERRAIN_H_INCLUDED

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Shader.h>
#include <Texture.h>

struct TVertex
{
    /// Position
    glm::vec3 Position;
    /// TexCoords
    glm::vec2 TexCoords;
};

struct Face
{
    glm::vec3 vertex0;
    glm::vec3 vertex1;
    glm::vec3 vertex2;
};

class Terrain
{
public:

    Terrain(const char*, const char*, float, int /**Must be Power of two*/);
    Terrain(const char*, const char*, const char*, float, int/**Must be Power of two*/);
    Terrain(const char*, const char*, const char*, const char*, float, int/**Must be Power of two*/);

    virtual ~Terrain(){

        std::cout <<"Deleted terrain" << std::endl;
        //delete diffuseTexture;
        //delete specularTexture;
        //delete heightMap;
    }

public:

    void RenderTerrain(Shader*);
    std::vector<TVertex> vertices;

private:

    bool GenerateSmoothNormals();
    bool GenerateVertexBuffers();
    bool GetHeightData();
    bool GenerateGrid(unsigned char* pixels);
    bool LoadTexture();
    bool LoadTexture(const char*);
    bool LoadTexture(const char*, const char*, const char*);
    bool calculateTangentSpace();

private:
    GLuint GL_d_texture;
    GLuint GL_n_texture;
    GLuint GL_s_texture;
    const char* diffuseTexture;
    const char* specularTexture;
    const char* heightMap;
    int width;
    int height;
    int channels;
    int gridSize;
    int cantPixels;

private:
    GLuint VAO;
    GLuint EBO;
    GLuint VBO;
    GLuint NVB;
    GLuint TVB;
    GLuint BVB;

private:
    std::vector<Face> vface;
    std::vector<GLuint> indices;
    std::vector<glm::vec3> Normals;
    std::vector<glm::vec3> Tangents;
    std::vector<glm::vec3> Bitangents;
};

#endif // TERRAIN_H_INCLUDED
