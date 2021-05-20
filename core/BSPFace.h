///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#pragma once

#include <pch.hpp>
#include <Core.hpp>
#include <glm/glm.hpp>
#include <Physics/Physics.h>
#include <Physics/PhysicObject.h>
#include <Physics/CollisionInfo.h>
#include <Helpers.hpp>

#include <Renderer/Texture2D.hpp>
#include <Shader.h>

namespace Epsilon
{

    struct tBSPLightmap
    {
        char imageBits[128][128][3]; /// The RGB data in a 128x128 image
    };

    using namespace std;

    class BSPFace
    {
    public:
        BSPFace()
        {
            collinfo = (std::shared_ptr<Physics::CollisionInfo>)new Physics::CollisionInfo();
        }

        virtual ~BSPFace()
        {
            //std::cout << "Destroyed BSP Face" << std::endl;
        }

        int GIProbeID = -1;
        int CubemapId = -1;

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
        void RenderFace(Shader_ptr shader, bool);
        
        void setTextures(const char* a, const char* m, const char* r, const char* n) {
            albedoTexture = a;
            metallicTexture = m;
            roughnessTexture = r;
            normalTexture = n;
        }

    public:
        std::string ObjectID;

        int faceID;
        std::string getObjectID()
        {
            return ObjectID;
        }

        std::shared_ptr<btRigidBody> rigidBody;

    private:
        struct t_Vertex
        {
            glm::vec3 position;
            glm::vec2 texcoord;
            glm::vec3 normal;
            glm::vec3 tangent;
            glm::vec3 bitangent;
        };

        std::vector<t_Vertex> mVertices;

        glm::vec3 mPosition;

        bool LoadLightMapTexture()
        {
            glGenTextures(1, &LightMaptexture);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glBindTexture(GL_TEXTURE_2D, LightMaptexture);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 128, 128, 0, GL_RGB, GL_UNSIGNED_BYTE, this->LightMap.imageBits);
            glGenerateMipmap(GL_TEXTURE_2D);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glBindTexture(GL_TEXTURE_2D, 0);

            return true;
        }

        bool CalcTangentSpace()
        {

            /// calculate tangent/bitangent vectors of both triangles
            glm::vec3 tangent1, bitangent1;
            for (int i = 0; i < (int)Indices.size(); i += 3)
            {

                glm::vec3 edge1 = Vertices.at(Indices.at((i + 1))) - Vertices.at(Indices.at((i)));
                glm::vec3 edge2 = Vertices.at(Indices.at((i + 2))) - Vertices.at(Indices.at((i)));

                glm::vec2 deltaUV1 = TexCoords.at(Indices.at((i + 1))) - TexCoords.at(Indices.at((i)));
                glm::vec2 deltaUV2 = TexCoords.at(Indices.at((i + 2))) - TexCoords.at(Indices.at((i)));

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

            glBindVertexArray(this->VAO);

            /// Load data into vertex buffers
            glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
            /// A great thing about structs is that their memory layout is sequential for all its items.
            /// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
            /// again translates to 3/2 floats which translates to a byte array.
            glBufferData(GL_ARRAY_BUFFER, this->mVertices.size() * sizeof(t_Vertex), &this->mVertices[0], GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->Indices.size() * sizeof(GLuint), &this->Indices[0], GL_STATIC_DRAW);

            /// Set the vertex attribute pointers
            /// Vertex Positions
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(t_Vertex), (GLvoid *)0);

            /// Vertex Texture Coords
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(t_Vertex), (GLvoid *)offsetof(t_Vertex, texcoord));

            /// Vertex Normals
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(t_Vertex), (GLvoid *)offsetof(t_Vertex, normal));

            /// Vertex Tangent
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(t_Vertex), (GLvoid *)offsetof(t_Vertex, tangent));

            /// Vertex Bitangent
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(t_Vertex), (GLvoid *)offsetof(t_Vertex, bitangent));

            glBindVertexArray(0);
            return true;
        }


    private:
        GLuint VAO;

        GLuint texture;
        string imagePath;
        GLuint VBO, EBO;
        GLuint GITexture;
        bool GISet = false;
        tBSPLightmap LightMap;
        GLuint LightMaptexture;
        std::shared_ptr<Physics::PhysicObject> CollisionObject;
        std::vector<glm::vec3> Vertices;
        std::vector<glm::vec3> Normals;
        std::vector<glm::vec2> TexCoords;
        std::vector<glm::vec2> LMTexCoords;
        std::vector<glm::vec3> Tangents;
        std::vector<glm::vec3> Bitangents;
        std::vector<unsigned int> Indices;

        const char* albedoTexture = nullptr;
        const char* metallicTexture = nullptr;
        const char* roughnessTexture = nullptr;
        const char* normalTexture = nullptr;

        std::shared_ptr<Physics::CollisionInfo> collinfo;
    };

} // namespace Epsilon