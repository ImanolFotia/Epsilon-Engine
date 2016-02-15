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
    aiString path;
};

class Mesh
{
public:
    /**  Mesh Data  */
    vector<Vertex> vertices;
    vector<GLuint> indices;
    vector<Texture> textures;

    /**  Functions  */
    /// Constructor
    Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        ///cout << "number of textures for this mesh: " << textures.size() << endl;

        /// Now that we have all the required data, set the vertex buffers and its attribute pointers.
        this->setupMesh();
    }

    /// Render the mesh
    void Draw(Shader* shader)
    {

        if(this->textures.size() > 0){
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(shader->getProgramID(), "texture_diffuse"), 0);
        glBindTexture(GL_TEXTURE_2D, this->textures[0].id);}


        if(this->textures.size() > 1){
        glActiveTexture(GL_TEXTURE1);
        glUniform1i(glGetUniformLocation(shader->getProgramID(), "texture_specular"), 1);
        glBindTexture(GL_TEXTURE_2D, this->textures[1].id);}

        if(this->textures.size() > 2){
        glActiveTexture(GL_TEXTURE2);
        glUniform1i(glGetUniformLocation(shader->getProgramID(), "texture_normal"), 2);
        glBindTexture(GL_TEXTURE_2D, this->textures[2].id);}

        if(this->textures.size() > 3){
        glActiveTexture(GL_TEXTURE3);
        glUniform1i(glGetUniformLocation(shader->getProgramID(), "texture_height"), 3);
        glBindTexture(GL_TEXTURE_2D, this->textures[3].id);}
/**
        glActiveTexture(GL_TEXTURE4);
        glUniform1i(glGetUniformLocation(shader->ProgramID, "skybox"), 4);
        glBindTexture(GL_TEXTURE_CUBE_MAP, this->textures[4].id);
*/
        /// Draw mesh
        glBindVertexArray(this->VAO);
        glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

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
        glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

        /// Set the vertex attribute pointers
        /// Vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);

        /// Vertex Texture Coords
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));

        /// Vertex Normals
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));

        /// Vertex Tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Tangent));
        /// Vertex Bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Bitangent));

        glBindVertexArray(0);
    }
};
