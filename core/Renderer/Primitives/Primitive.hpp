#pragma once

#include <pch.hpp>
#include <Types.h>
#include <glm/glm.hpp>

#include <Driver/API/OpenGL/VertexArrayObject.h>
#include <Driver/API/VertexBufferObject.hpp>

#include "../Drawable.hpp"
#include "../MaterialPBR.hpp"
#include "../MaterialManager.hpp"

#include "Helpers.hpp"

namespace Epsilon::Renderer
{

    class PrimitiveBase : public Drawable
    {
    public:
        virtual void Update() = 0;

        virtual void Render() = 0;

        virtual void Destroy() = 0;
        
        virtual void Draw(std::shared_ptr<Shader> shader, bool force_draw = false, std::initializer_list<unsigned int> mesh_index_list = {}) = 0;

        virtual std::shared_ptr<MaterialPBR> getMaterial() = 0;

        virtual MaterialManager::Material_id getMaterialId() = 0;

        virtual void setMaterial(MaterialManager::Material_id mat_id) = 0;

        struct Vertex
        {
            Vertex(glm::vec3 p)
            {
                position = p;
            }

            Vertex(glm::vec3 p, glm::vec3 n, glm::vec2 u)
            {
                position = p;
                normal = n;
                uv = u;
            }
            glm::vec3 position;
            glm::vec2 uv;
            glm::vec3 normal;
            glm::vec3 bitangent;
            glm::vec3 tangent;
        };

        struct Mesh_t
        {

            void addTriangle(uint32_t a, uint32_t b, uint32_t c)
            {
                Indices.push_back(a);
                Indices.push_back(b);
                Indices.push_back(c);
            }

            void addQuad(uint32_t a, uint32_t b, uint32_t c, uint32_t d)
            {
                Indices.push_back(a);
                Indices.push_back(b);
                Indices.push_back(d);
                Indices.push_back(b);
                Indices.push_back(c);
                Indices.push_back(d);
            }

            std::vector<Vertex> Vertices;
            std::vector<uint32_t> Indices;
        } Mesh;

        
        auto getGeometry() -> const Mesh_t& {
            return Mesh;
        }


    protected:
        void createVertexBuffers()
        {
            mVertexArray = std::make_shared<API::OpenGL::VertexArrayObject>();

            mVertexBufferHandle = mVertexArray->addBuffer(Mesh.Vertices.size() * sizeof(Vertex), &Mesh.Vertices[0], GL_STATIC_DRAW);
            mVertexArray->setAttribute(VERTEX_POINTER_INDEX::POSITION, 3, sizeof(Vertex), (GLvoid *)0);
            mVertexArray->setAttribute(VERTEX_POINTER_INDEX::UV, 2, sizeof(Vertex), (void *)offsetof(Vertex, uv));
            mVertexArray->setAttribute(VERTEX_POINTER_INDEX::NORMAL, 3, sizeof(Vertex), (void *)offsetof(Vertex, normal));
            mVertexArray->setAttribute(VERTEX_POINTER_INDEX::TANGENT, 3, sizeof(Vertex), (void *)offsetof(Vertex, tangent));
            mVertexArray->setAttribute(VERTEX_POINTER_INDEX::BITANGENT, 3, sizeof(Vertex), (void *)offsetof(Vertex, bitangent));

            mVertexArray->IndexBuffer(Mesh.Indices);
        }
        
        void updateVertexBuffers()
        {
            mVertexArray->UpdateBuffer(mVertexBufferHandle, 0, Mesh.Vertices.size() * sizeof(Vertex), &Mesh.Vertices[0]);
        }

        void generateTangentSpaceVectors()
        {

            uint32_t num_triangles = Mesh.Indices.size() / 3;
            // calculate tangent/bitangent vectors of both triangles
            glm::vec3 tangent1, bitangent1;
            // std::cout << mesh.m_Tris.size() <<  std::endl;
            for (int i = 0; i < Mesh.Indices.size(); i += 3)
            {
                glm::vec3 v0 = Mesh.Vertices[Mesh.Indices[i]].position;
                glm::vec3 v1 = Mesh.Vertices[Mesh.Indices[i + 1]].position;
                glm::vec3 v2 = Mesh.Vertices[Mesh.Indices[i + 2]].position;

                glm::vec3 edge1 = v1 - v0;
                glm::vec3 edge2 = v2 - v0;

                glm::vec2 t0 = Mesh.Vertices[Mesh.Indices[i]].uv;
                glm::vec2 t1 = Mesh.Vertices[Mesh.Indices[i + 1]].uv;
                glm::vec2 t2 = Mesh.Vertices[Mesh.Indices[i + 2]].uv;

                glm::vec2 deltaUV1 = t1 - t0;
                glm::vec2 deltaUV2 = t2 - t0;

                // GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y + deltaUV2.x * deltaUV1.y);

                float f = (deltaUV2.x * deltaUV1.x - deltaUV2.y * deltaUV1.y) == 0.0f ? -1.0f : 1.0f;
                // when t1, t2, t3 in same position in UV space, just use default UV direction.
                /*if (0 == deltaUV2.x && 0 == deltaUV2.y && 0 == deltaUV1.x && 0 == deltaUV1.y)
                {
                    deltaUV1.x = 0.0;
                    deltaUV1.y = 1.0;
                    deltaUV2.y = 1.0;
                    deltaUV2.y = 0.0;
                }*/

                tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
                tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
                tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
                // tangent1 = glm::normalize(tangent1);

                Mesh.Vertices[Mesh.Indices[i]].tangent = tangent1;
                Mesh.Vertices[Mesh.Indices[i + 1]].tangent = tangent1;
                Mesh.Vertices[Mesh.Indices[i + 2]].tangent = tangent1;

                bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
                bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
                bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
                // bitangent1 = glm::normalize(bitangent1);

                Mesh.Vertices[Mesh.Indices[i]].bitangent = bitangent1;
                Mesh.Vertices[Mesh.Indices[i + 1]].bitangent = bitangent1;
                Mesh.Vertices[Mesh.Indices[i + 2]].bitangent = bitangent1;
            }
        }

    protected:
        std::shared_ptr<API::VertexArrayObject> mVertexArray;
        MaterialManager::Material_id mMaterial;
        unsigned int mVertexBufferHandle = 0;
    };

} // namespace Epsilon