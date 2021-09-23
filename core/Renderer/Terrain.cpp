///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#include <pch.hpp>
#include "Terrain.h"
#include <Physics/Physics.h>
#include <Resource/ResourceManager.h>
#include <Physics/CollisionInfo.h>
#include <Physics/TriangleMeshPhysicObject.h>
#include <Helpers.hpp>


namespace Epsilon
{
    Terrain::Terrain(const char *heightMap, const char *diffuseTexture, float scale, int gridSize /**Must be Power of two*/)
    {
        this->GL_n_texture = 0;

        this->gridSize = gridSize;

        this->heightMap = heightMap;

        this->diffuseTexture = diffuseTexture;

        this->LoadTexture(diffuseTexture);

        this->GenerateVertexBuffers();
    }

    Terrain::Terrain(const char *heightMap, const char *diffuseTexture, const char *normalTexture, const char *specularTexture, const char *metalTexture, float sc, int gridSize, glm::vec3 Position)
    {
        this->gridSize = gridSize;

        this->heightMap = heightMap;

        this->scale = sc;

        this->m_Position = Position;

        this->diffuseTexture = diffuseTexture;

        this->specularTexture = specularTexture;

        this->GenerateGrid();

        try
        {
            this->LoadTexture(diffuseTexture, normalTexture, specularTexture, metalTexture);
        }
        catch (std::exception &e)
        {
            std::cout << "Exception after LoadTexture" << e.what() << std::endl;
        }

        this->GenerateVertexBuffers();
    }

    bool Terrain::GetHeightData()
    {
        //int w, h;

        unsigned char *pixels = stbi_load(this->heightMap, &this->width, &this->height, 0, 4);

        if (this->width != this->height)
        {
            std::cout << "TERRAIN ERROR: THE TEXTURE'S SIZE PROVIDED IS NOT POWER OF TWO::ABORTING" << std::endl;
            return 1;
        }

        this->cantPixels = this->width * this->height;

        this->GenerateGrid();

        stbi_image_free(pixels);

        return true;
    }

    bool Terrain::GenerateGrid()
    {
        try
        {
            TVertex vert;

            //float texU = float(this->gridSize);
            //float texV = float(this->gridSize);
            int counter = 0;

            std::vector<std::vector<float>> Grid;
            std::vector<float> row;

            for (int i = 0; i < this->gridSize; i++)
            {
                for (int j = 0; j < this->gridSize; j++)
                {
                    vert.Position.x = ((float)i + m_Position.x) * scale;
                    vert.Position.y = (float)glm::pow(FBM(glm::vec2(i, j) * 0.04f), 3.0) * 15.0f + m_Position.y;
                    vert.Position.z = ((float)j + m_Position.z) * scale;
                    row.push_back(vert.Position.y);
                    float fScaleC = float(j) / float(this->gridSize - 1);
                    float fScaleR = float(i) / float(this->gridSize - 1);
                    vert.TexCoords.s = this->gridSize * fScaleC / this->gridSize * 10;
                    vert.TexCoords.t = this->gridSize * fScaleR / this->gridSize * 10;
                    vertices.push_back(vert);
                    counter++;
                }
                Grid.push_back(row);
                row.clear();
            }

            int mod = 0;
            counter = 0;
            for (int i = 0; i < this->gridSize; i++)
            {
                for (int j = 0; j < this->gridSize; j++)
                {
                    if (i > 0 && j > 0 && i < Grid.size() - 1 && j < Grid.size() - 1)
                    {
                        float HL = Grid.at(i - 1).at(j);
                        float HR = Grid.at(i + 1).at(j);
                        float HD = Grid.at(i).at(j - 1);
                        float HU = Grid.at(i).at(j + 1);

                        glm::vec3 norm = glm::normalize(glm::vec3(HL - HR, 1.0f, HD - HU));

                        Normals.push_back(norm);
                    }
                    else
                    {
                        Normals.push_back(glm::vec3(0, 1, 0));
                    }
                }
            }
            for (unsigned int i = 0; i < vertices.size(); i++)
            {
                vertices.at(i).Normal = Normals.at(i);
            }

            counter = 0;

            for (int i = 0; i < glm::pow(this->gridSize, 2.0f) - this->gridSize; i++)
            {
                if (counter == this->gridSize - 1)
                {
                    counter = 0;
                    mod++;
                    continue;
                }
                Face face;

                face.vertex0 = vertices[i + 1].Position;
                indices.push_back(i + 1);
                face.vertex1 = vertices[i + this->gridSize].Position;
                indices.push_back(i + this->gridSize);
                face.vertex2 = vertices[i + this->gridSize - this->gridSize].Position;
                indices.push_back(i + this->gridSize - this->gridSize);
                vface.push_back(face);
                face.vertex0 = vertices[i + 1].Position;
                indices.push_back(i + 1);
                face.vertex1 = vertices[i + this->gridSize + 1].Position;
                indices.push_back(i + this->gridSize + 1);
                face.vertex2 = vertices[i + this->gridSize].Position;
                indices.push_back(i + this->gridSize);

                vface.push_back(face);

                counter++;
            }

            std::vector<glm::vec3> tmp_vertices;

            std::cout << indices.size() << std::endl;

            for (unsigned int i = 0; i < vertices.size(); ++i)
                tmp_vertices.push_back(vertices[i].Position);

            std::shared_ptr<Physics::TriangleMeshPhysicObject> TerrainPhysicsMesh = (std::shared_ptr<Physics::TriangleMeshPhysicObject>)new Physics::TriangleMeshPhysicObject();

            this->rigidBody = nullptr;

            this->rigidBody = TerrainPhysicsMesh->addObject(tmp_vertices, this->indices, 1.0f);

            this->collinfo = (std::shared_ptr<Physics::CollisionInfo>)new Physics::CollisionInfo();

            this->collinfo->setName("Terrain");

            TerrainPhysicsMesh->Body->setUserPointer(collinfo.get());

            ResourceManager::Get().getPhysicsWorld()->getSoftDynamicsWorld()->addRigidBody(rigidBody.get());

            this->CollisionObject = TerrainPhysicsMesh;
        }
        catch (std::exception& e)
        {
            std::cout << "Exception caught at Terrain::GenerateGrid()"
                      << "\nInfo: " << e.what() << std::endl;
        }

        calculateTangentSpace();

        return true;
    }

    bool Terrain::calculateTangentSpace()
    {
        /// calculate tangent/bitangent vectors of both triangles
        glm::vec3 tangent1, bitangent1;
        for (int i = 0; i < (int)indices.size(); i += 3)
        {

            glm::vec3 edge1 = vertices.at(indices.at((i + 1))).Position - vertices.at(indices.at((i))).Position;
            glm::vec3 edge2 = vertices.at(indices.at((i + 2))).Position - vertices.at(indices.at((i))).Position;

            glm::vec2 deltaUV1 = vertices.at(indices.at((i + 1))).TexCoords - vertices.at(indices.at((i))).TexCoords;
            glm::vec2 deltaUV2 = vertices.at(indices.at((i + 2))).TexCoords - vertices.at(indices.at((i))).TexCoords;

            GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

            tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
            tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
            tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
            /// tangent1 = glm::normalize(tangent1);

            tangent1 = glm::normalize(tangent1);
            vertices.at(indices.at((i + 0))).Tangent = tangent1;
            vertices.at(indices.at((i + 1))).Tangent = tangent1;
            vertices.at(indices.at((i + 2))).Tangent = tangent1;
            bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
            bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
            bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
            /// bitangent1 = glm::normalize(bitangent1);

            bitangent1 = glm::normalize(bitangent1);
            vertices.at(indices.at((i + 0))).Binormal = bitangent1;
            vertices.at(indices.at((i + 1))).Binormal = bitangent1;
            vertices.at(indices.at((i + 2))).Binormal = bitangent1;
        }

        return true;
    }

    bool Terrain::LoadTexture(const char *diff)
    {
        int texwidth = 0, texheight = 0, texchannels;

        eTexture dTex(diff);

        GL_d_texture = dTex.getTextureID();

        texwidth = dTex.getWidth();

        texheight = dTex.getWidth();

        return true;
    }

    bool Terrain::LoadTexture(const char *diff, const char *normal, const char *specular, const char *metallic)
    {
        try
        {
            int texwidth, texheight, texchannels;

            eTexture dTex(diff);

            eTexture nTex(normal);

            eTexture sTex(specular);

            eTexture mTex(metallic);

            eTexture Decal("decal.png", GL_CLAMP_TO_BORDER);

            GL_d_texture = dTex.getTextureID();

            GL_n_texture = nTex.getTextureID();

            GL_s_texture = sTex.getTextureID();

            GL_m_texture = mTex.getTextureID();

            GL_decal_texture = Decal.getTextureID();

            texwidth = dTex.getWidth();

            texheight = dTex.getWidth();

            return true;
        }
        catch (std::exception &e)
        {
            std::cout << "Exception after LoadTexture" << e.what() << std::endl;
        }

        return false;
    }

    bool Terrain::GenerateVertexBuffers()
    {
        glGenVertexArrays(1, &this->VAO);
        glGenBuffers(1, &this->VBO);
        glGenBuffers(1, &this->EBO);

        glBindVertexArray(this->VAO);

        /** Vertex And UV buffer Attachments*/

        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(TVertex), &this->vertices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TVertex), (GLvoid *)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TVertex), (GLvoid *)offsetof(TVertex, TexCoords));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(TVertex), (GLvoid *)offsetof(TVertex, Normal));

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(TVertex), (GLvoid *)offsetof(TVertex, Tangent));

        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(TVertex), (GLvoid *)offsetof(TVertex, Binormal));

        /** Element buffer Attachments*/

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

        glBindVertexArray(0);

        return true;
    }

    float Terrain::FBM(glm::vec2 p)
    {

        auto hash11 = [&](float n) -> float {
            return glm::fract(glm::sin(n) * 43758.5453123);
        };

        auto noise12 = [&](glm::vec2 x) -> float {
            glm::vec2 q = glm::floor(x);
            glm::vec2 f = glm::fract(x);

            f = f * f * (3.0f - 2.0f * f);

            float n = q.x + q.y * 57.0f;

            return glm::mix(glm::mix(hash11(n + 0.0f), hash11(n + 1.0f), f.x), glm::mix(hash11(n + 57.0f), hash11(n + 58.0f), f.x), f.y);
        };

        float f = 0.0;
        f += 0.5 * noise12(p);
        p *= 2.02;
        f += 0.25 * noise12(p);
        p *= 2.03;
        f += 0.125 * noise12(p);
        p *= 2.04;
        f += 0.0625 * noise12(p);
        p *= 2.05;
        return f / 0.9375;
    }

    void Terrain::RenderTerrain(Shader *shader)
    {

        glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(shader->getProgramID(), "texture_diffuse"), 0);
        glBindTexture(GL_TEXTURE_2D, GL_d_texture);

        glActiveTexture(GL_TEXTURE1);
        glUniform1i(glGetUniformLocation(shader->getProgramID(), "texture_specular"), 1);
        glBindTexture(GL_TEXTURE_2D, GL_s_texture);

        glActiveTexture(GL_TEXTURE2);
        glUniform1i(glGetUniformLocation(shader->getProgramID(), "texture_normal"), 2);
        glBindTexture(GL_TEXTURE_2D, GL_n_texture);

        glActiveTexture(GL_TEXTURE3);
        glUniform1i(glGetUniformLocation(shader->getProgramID(), "texture_height"), 3);
        glBindTexture(GL_TEXTURE_2D, GL_m_texture);

        // Draw mesh
        glBindVertexArray(this->VAO);
        glCache::glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glCache::glUseProgram(0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
} // namespace Epsilon