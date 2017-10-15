///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#pragma once
/// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;
/// GL Includes
#include <GL/glew.h> /// Contains all the necessery OpenGL includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Shader.h>
#include <emlFormat.h>
#include <memory>

struct Vertex
{
    /// Position
    glm::vec3 Position;
    /// Normal
    glm::vec3 Normal;
    /// TexCoords
    glm::vec2 TexCoords;
    ///Tangents
    glm::vec3 Tangent;
    ///Bitangents
    glm::vec3 Bitangent;
};

struct Texture
{
    GLuint id;
    string type;
    string path;
};

class ResourceManager;

class Mesh
{
public:
    /**  Mesh Data  */
    vector<t_Vertex> vertices;
    int CubeMapIndex = 1;
    vector<Vertex> ivertices;
    vector<GLuint> indices;
    vector<Texture> textures;

    /**  Functions  */
    /// Constructor
    Mesh(vector<t_Vertex> vertices, vector<GLuint> indices, vector<Texture> textures, int CubeMapindex = 1)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        CubeMapIndex = CubeMapindex;
        ///cout << "number of textures for this mesh: " << textures.size() << endl;

        /// Now that we have all the required data, set the vertex buffers and its attribute pointers.
        this->setupMesh();
    }

    ~Mesh()
    {
        //std::cout << "Deleted Mesh" << std::endl;
    }

    /// Render the mesh
    void Draw(Shader*& shader, std::shared_ptr<ResourceManager> resm, glm::vec3 pos);

    /// Render the mesh
    void Draw(GLuint shader, std::shared_ptr<ResourceManager> resm, glm::vec3);

/// Render the mesh
    void DrawWithAlpha()
    {
        glBindTexture(GL_TEXTURE_2D, this->textures[0].id);

        glBindVertexArray(this->VAO);
        glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void DrawNoTexture()
    {
        glBindVertexArray(this->VAO);
        glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void Destroy()
    {
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteVertexArrays(1, &VAO);
    }

private:
    /**  Render data  */
    GLuint VAO, VBO, EBO;

    /**  Functions    */
/// Initializes all the buffer objects/arrays
    void setupMesh()
    {
        /// Create buffers/arrays
        glGenVertexArrays(1, &this->VAO);
        glGenBuffers(1, &this->VBO);
        glGenBuffers(1, &this->EBO);

        glBindVertexArray(this->VAO);
        /// Load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        /// A great thing about structs is that their memory layout is sequential for all its items.
        /// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        /// again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(t_Vertex), &this->vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

        /// Set the vertex attribute pointers
        /// Vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(t_Vertex), (GLvoid*)0);

        /// Vertex Texture Coords
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(t_Vertex), (GLvoid*)offsetof(t_Vertex, texcoord));

        /// Vertex Normals
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(t_Vertex), (GLvoid*)offsetof(t_Vertex, normal));

        /// Vertex Tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(t_Vertex), (GLvoid*)offsetof(t_Vertex, tangent));

        /// Vertex Bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(t_Vertex), (GLvoid*)offsetof(t_Vertex, bitangent));

        glBindVertexArray(0);
    }
};
