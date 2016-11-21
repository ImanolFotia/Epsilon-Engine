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
#include <memory>
#include <Physics.h>

class ResourceManager;

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

    Terrain(const char*, const char*, float, int, std::shared_ptr<ResourceManager> rm /**Must be Power of two*/);
    Terrain(const char*, const char*, const char*, float, int/**Must be Power of two*/);
    Terrain(const char*, const char*, const char*, const char*, float, int, glm::vec3,std::shared_ptr<ResourceManager> rm/**Must be Power of two*/);

    virtual ~Terrain(){

        //std::cout <<"Deleted terrain" << std::endl;
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
    GLuint GL_decal_texture;
    const char* diffuseTexture;
    const char* specularTexture;
    const char* heightMap;
    int width;
    int height;
    int channels;
    int gridSize;
    int cantPixels;
    float scale;
    glm::vec3 m_Position;

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

private:
    std::shared_ptr<ResourceManager> rM;
    std::shared_ptr<btRigidBody> rigidBody;
    std::shared_ptr<Physics::CollisionInfo> collinfo;
    std::shared_ptr<Physics::PhysicObject> CollisionObject;
};

#endif // TERRAIN_H_INCLUDED
