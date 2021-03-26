///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#pragma once
/// Std. Includes
#include <pch.hpp>
/// GL Includes
#include <Core.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Shader.h>
#include <Renderer/EML/eml1_0.h>

namespace Epsilon
{
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
        std::string type;
        std::string path;
    };

    class Mesh
    {

        class MeshData
        {
            unsigned numVertices;
            unsigned numIndices;
            unsigned vertexOffset;
            unsigned indexOffset;

            public:
                MeshData() = default;
                MeshData(auto a, auto b, auto c, auto d)
                        : numVertices(a), numIndices(b), vertexOffset(c), indexOffset(d) {}
        };

    public:
        /**  Mesh Data  */
        std::vector<Renderer::t_Vertex> vertices;
        int CubeMapIndex = 1;
        std::vector<Vertex> ivertices;
        std::vector<GLuint> indices;
        std::vector<Texture> textures;
        glm::mat4 mLocalTransform;

        bool isVisible = true;
        glm::vec3 mPosition = glm::vec3(0.0, 0.0, 0.0);

        /**  Functions  */
        /// Constructor
        Mesh(std::vector<Renderer::t_Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures, unsigned vOffset, unsigned iOffset, int CubeMapindex = 1)
        {
            this->vertices = vertices;
            this->indices = indices;
            this->textures = textures;

            mData = { vertices.size(), indices.size(), 0, 0 };


            CubeMapIndex = CubeMapindex;
            mGIIndex = 0;
            mCubemapIndex = 1;
            finalCubemaps = false;
            ///cout << "number of textures for this mesh: " << textures.size() << endl;

            /// Now that we have all the required data, set the vertex buffers and its attribute pointers.
            //this->setupMesh();
        }

        ~Mesh()
        {
            mGIIndex = 0;
            mCubemapIndex = 1;
            //std::cout << "Deleted Mesh" << std::endl;
        }

        bool updateCubemaps = false;

        bool finalCubemaps = false;

        /// Render the mesh
        void Draw(std::shared_ptr<Shader> shader, glm::vec3 pos);

        /// Render the mesh
        void DrawWithAlpha()
        {
            if (isVisible)
            {
                glBindTexture(GL_TEXTURE_2D, this->textures[0].id);

                glBindVertexArray(this->VAO);
                glCache::glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);
            }
        }

        void DrawNoTexture()
        {
            if (isVisible)
            {
                glBindVertexArray(this->VAO);
                glCache::glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);
            }
        }

        void Destroy()
        {
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
            glDeleteVertexArrays(1, &VAO);
        }

    public:
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
            glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Renderer::t_Vertex), &this->vertices[0], GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

            /// Set the vertex attribute pointers
            /// Vertex Positions
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Renderer::t_Vertex), (GLvoid *)0);

            /// Vertex Texture Coords
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Renderer::t_Vertex), (GLvoid *)offsetof(Renderer::t_Vertex, texcoord));

            /// Vertex Normals
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Renderer::t_Vertex), (GLvoid *)offsetof(Renderer::t_Vertex, normal));

            /// Vertex Tangent
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Renderer::t_Vertex), (GLvoid *)offsetof(Renderer::t_Vertex, tangent));

            /// Vertex Bitangent
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Renderer::t_Vertex), (GLvoid *)offsetof(Renderer::t_Vertex, bitangent));

            glBindVertexArray(0);
        }

    private:
        uint32_t mGIIndex;
        uint32_t mCubemapIndex;
        MeshData mData;
    };
} // namespace Epsilon