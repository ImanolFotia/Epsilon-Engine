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
#include <Driver/API/OpenGL/VertexArrayObject.h>
#include "MaterialManager.hpp"

namespace Epsilon::Renderer
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

    struct MeshTexture
    {
        GLuint id;
        std::string type;
        std::string path;
    };

    class MaterialPBR;
    class Mesh
    {

        using MaterialPBR_ptr = std::shared_ptr<MaterialPBR>;

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
        std::vector<MeshTexture> textures;
        glm::mat4 mLocalTransform;

        bool isVisible = true;
        bool isSettedUp = false;
        glm::vec3 mPosition = glm::vec3(0.0, 0.0, 0.0);

        /**  Functions  */
        /// Constructor
        Mesh(std::vector<Renderer::t_Vertex> vertices, std::vector<GLuint> indices, std::vector<MeshTexture> textures, unsigned vOffset, unsigned iOffset, int CubeMapindex = 1);

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

                mVAO->Bind();
                glCache::glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
                mVAO->Unbind();
            }
        }

        void DrawNoTexture()
        {
            if (isVisible)
            {
                mVAO->Bind();
                glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
                mVAO->Unbind();
            }
        }

        void Destroy();

        void setMaterial(MaterialPBR_ptr);

        MaterialPBR_ptr getMaterial() {
            return MaterialManager::Get().getMaterial<MaterialPBR>(mMaterial);
        }

    public:
        MIN_MAX_POINTS MinMaxPoints;

        /**  Functions    */
        /// Initializes all the buffer objects/arrays
        void setupMesh();

    private:
        uint32_t mGIIndex;
        uint32_t mCubemapIndex;
        MeshData mData;

        MaterialManager::Material_id mMaterial;

        std::shared_ptr<API::OpenGL::VertexArrayObject> mVAO;
    };
} // namespace Epsilon