#pragma once

#include <pch.hpp>
#include "Primitive.hpp"
#include "../MaterialManager.hpp"

namespace Epsilon::Renderer
{

    class Cube : public PrimitiveBase
    {
    public:
        Cube()
        {
            using vtx = PrimitiveBase::Vertex;
            using v3 = glm::vec3;
            using v2 = glm::vec2;
            //Mesh.Vertices = {
                // back face
                Mesh.Vertices.emplace_back(vtx(v3(-1.0f, -1.0f, -1.0f), v3(0.0f, 0.0f, -1.0f), v2(0.0f, 0.0f))); // bottom-left
                Mesh.Vertices.emplace_back(vtx(v3(1.0f, 1.0f, -1.0f), v3(0.0f, 0.0f, -1.0f), v2(1.0f, 1.0f)));   // top-right
                Mesh.Vertices.emplace_back(vtx(v3(1.0f, -1.0f, -1.0f), v3(0.0f, 0.0f, -1.0f), v2(1.0f, 0.0f)));  // bottom-right
                Mesh.Vertices.emplace_back(vtx(v3(1.0f, 1.0f, -1.0f), v3(0.0f, 0.0f, -1.0f), v2(1.0f, 1.0f)));   // top-right
                Mesh.Vertices.emplace_back(vtx(v3(-1.0f, -1.0f, -1.0f), v3(0.0f, 0.0f, -1.0f), v2(0.0f, 0.0f))); // bottom-left
                Mesh.Vertices.emplace_back(vtx(v3(-1.0f, 1.0f, -1.0f), v3(0.0f, 0.0f, -1.0f), v2(0.0f, 1.0f)));  // top-left
                
                Mesh.Vertices.emplace_back(vtx(v3(-1.0f, -1.0f, 1.0f), v3(0.0f, 0.0f, 1.0f), v2(0.0f, 0.0f))); // bottom-left
                Mesh.Vertices.emplace_back(vtx(v3(1.0f, -1.0f, 1.0f), v3(0.0f, 0.0f, 1.0f), v2(1.0f, 0.0f)));  // bottom-right
                Mesh.Vertices.emplace_back(vtx(v3(1.0f, 1.0f, 1.0f), v3(0.0f, 0.0f, 1.0f), v2(1.0f, 1.0f)));   // top-right
                Mesh.Vertices.emplace_back(vtx(v3(1.0f, 1.0f, 1.0f), v3(0.0f, 0.0f, 1.0f), v2(1.0f, 1.0f)));   // top-right
                Mesh.Vertices.emplace_back(vtx(v3(-1.0f, 1.0f, 1.0f), v3(0.0f, 0.0f, 1.0f), v2(0.0f, 1.0f)));  // top-left
                Mesh.Vertices.emplace_back(vtx(v3(-1.0f, -1.0f, 1.0f), v3(0.0f, 0.0f, 1.0f), v2(0.0f, 0.0f))); // bottom-left
                
                Mesh.Vertices.emplace_back(vtx(v3(-1.0f, 1.0f, 1.0f), v3(-1.0f, 0.0f, 0.0f), v2(1.0f, 0.0f)));   // top-right
                Mesh.Vertices.emplace_back(vtx(v3(-1.0f, 1.0f, -1.0f), v3(-1.0f, 0.0f, 0.0f), v2(1.0f, 1.0f)));  // top-left
                Mesh.Vertices.emplace_back(vtx(v3(-1.0f, -1.0f, -1.0f), v3(-1.0f, 0.0f, 0.0f), v2(0.0f, 1.0f))); // bottom-left
                Mesh.Vertices.emplace_back(vtx(v3(-1.0f, -1.0f, -1.0f), v3(-1.0f, 0.0f, 0.0f), v2(0.0f, 1.0f))); // bottom-left
                Mesh.Vertices.emplace_back(vtx(v3(-1.0f, -1.0f, 1.0f), v3(-1.0f, 0.0f, 0.0f), v2(0.0f, 0.0f)));  // bottom-right
                Mesh.Vertices.emplace_back(vtx(v3(-1.0f, 1.0f, 1.0f), v3(-1.0f, 0.0f, 0.0f), v2(1.0f, 0.0f)));   // top-right
                
                Mesh.Vertices.emplace_back(vtx(v3(1.0f, 1.0f, 1.0f), v3(1.0f, 0.0f, 0.0f), v2(1.0f, 0.0f)));   // top-left
                Mesh.Vertices.emplace_back(vtx(v3(1.0f, -1.0f, -1.0f), v3(1.0f, 0.0f, 0.0f), v2(0.0f, 1.0f))); // bottom-right
                Mesh.Vertices.emplace_back(vtx(v3(1.0f, 1.0f, -1.0f), v3(1.0f, 0.0f, 0.0f), v2(1.0f, 1.0f)));  // top-right
                Mesh.Vertices.emplace_back(vtx(v3(1.0f, -1.0f, -1.0f), v3(1.0f, 0.0f, 0.0f), v2(0.0f, 1.0f))); // bottom-right
                Mesh.Vertices.emplace_back(vtx(v3(1.0f, 1.0f, 1.0f), v3(1.0f, 0.0f, 0.0f), v2(1.0f, 0.0f)));   // top-left
                Mesh.Vertices.emplace_back(vtx(v3(1.0f, -1.0f, 1.0f), v3(1.0f, 0.0f, 0.0f), v2(0.0f, 0.0f)));  // bottom-left
                
                Mesh.Vertices.emplace_back(vtx(v3(-1.0f, -1.0f, -1.0f), v3(0.0f, -1.0f, 0.0f), v2(0.0f, 1.0f))); // top-right
                Mesh.Vertices.emplace_back(vtx(v3(1.0f, -1.0f, -1.0f), v3(0.0f, -1.0f, 0.0f), v2(1.0f, 1.0f)));  // top-left
                Mesh.Vertices.emplace_back(vtx(v3(1.0f, -1.0f, 1.0f), v3(0.0f, -1.0f, 0.0f), v2(1.0f, 0.0f)));   // bottom-left
                Mesh.Vertices.emplace_back(vtx(v3(1.0f, -1.0f, 1.0f), v3(0.0f, -1.0f, 0.0f), v2(1.0f, 0.0f)));   // bottom-left
                Mesh.Vertices.emplace_back(vtx(v3(-1.0f, -1.0f, 1.0f), v3(0.0f, -1.0f, 0.0f), v2(0.0f, 0.0f)));  // bottom-right
                Mesh.Vertices.emplace_back(vtx(v3(-1.0f, -1.0f, -1.0f), v3(0.0f, -1.0f, 0.0f), v2(0.0f, 1.0f))); // top-right
                
                Mesh.Vertices.emplace_back(vtx(v3(-1.0f, 1.0f, -1.0f), v3(0.0f, 1.0f, 0.0f), v2(0.0f, 1.0f))); // top-left
                Mesh.Vertices.emplace_back(vtx(v3(1.0f, 1.0f, 1.0f), v3(0.0f, 1.0f, 0.0f), v2(1.0f, 0.0f)));   // bottom-right
                Mesh.Vertices.emplace_back(vtx(v3(1.0f, 1.0f, -1.0f), v3(0.0f, 1.0f, 0.0f), v2(1.0f, 1.0f)));  // top-right
                Mesh.Vertices.emplace_back(vtx(v3(1.0f, 1.0f, 1.0f), v3(0.0f, 1.0f, 0.0f), v2(1.0f, 0.0f)));   // bottom-right
                Mesh.Vertices.emplace_back(vtx(v3(-1.0f, 1.0f, -1.0f), v3(0.0f, 1.0f, 0.0f), v2(0.0f, 1.0f))); // top-left
                Mesh.Vertices.emplace_back(vtx(v3(-1.0f, 1.0f, 1.0f), v3(0.0f, 1.0f, 0.0f), v2(0.0f, 0.0f)));   // bottom-left
            //};
            //Generate tangent space vectors

            for(int i = 0; i < Mesh.Vertices.size(); i+=3) {
                Mesh.addTriangle(i,i+1,i+2);
            }

            uint32_t num_triangles = Mesh.Indices.size() / 3;
            // calculate tangent/bitangent vectors of both triangles
            glm::vec3 tangent1, bitangent1;
            //std::cout << mesh.m_Tris.size() <<  std::endl;
            for (int i = 0; i < Mesh.Indices.size(); i += 3)
            {
                glm::vec3 v0 = Mesh.Vertices[Mesh.Indices[i]].position;
                glm::vec3 v1 = Mesh.Vertices[Mesh.Indices[i + 1]].position;
                glm::vec3 v2 = Mesh.Vertices[Mesh.Indices[i + 2]].position;

                glm::vec3 edge1 = v2 - v0;
                glm::vec3 edge2 = v1 - v0;

                glm::vec2 t0 = Mesh.Vertices[Mesh.Indices[i]].uv;
                glm::vec2 t1 = Mesh.Vertices[Mesh.Indices[i + 1]].uv;
                glm::vec2 t2 = Mesh.Vertices[Mesh.Indices[i + 2]].uv;

                glm::vec2 deltaUV1 = t1 - t0;
                glm::vec2 deltaUV2 = t2 - t0;

                //GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y + deltaUV2.x * deltaUV1.y);

                float f = (deltaUV2.x * deltaUV1.x - deltaUV2.y * deltaUV1.y) == 0.0f ? -1.0f : 1.0f;
                // when t1, t2, t3 in same position in UV space, just use default UV direction.
                if (0 == deltaUV2.x && 0 == deltaUV2.y && 0 == deltaUV1.x && 0 == deltaUV1.y)
                {
                    deltaUV1.x = 0.0;
                    deltaUV1.y = 1.0;
                    deltaUV2.y = 1.0;
                    deltaUV2.y = 0.0;
                }

                tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
                tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
                tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
                //tangent1 = glm::normalize(tangent1);

                Mesh.Vertices[Mesh.Indices[i]].tangent = tangent1;
                Mesh.Vertices[Mesh.Indices[i + 1]].tangent = tangent1;
                Mesh.Vertices[Mesh.Indices[i + 2]].tangent = tangent1;

                bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
                bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
                bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
                //bitangent1 = glm::normalize(bitangent1);

                Mesh.Vertices[Mesh.Indices[i]].bitangent = bitangent1;
                Mesh.Vertices[Mesh.Indices[i + 1]].bitangent = bitangent1;
                Mesh.Vertices[Mesh.Indices[i + 2]].bitangent = bitangent1;
            }

            for (unsigned int i = 0; i < Mesh.Vertices.size(); ++i)
            {
                Vertex &vert = Mesh.Vertices[i];

                // re-orthogonalize T with respect to N
                vert.tangent = glm::normalize(vert.tangent - glm::dot(vert.tangent, vert.normal) * vert.normal);
                // then retrieve perpendicular vector B with the cross product of T and N
                vert.bitangent = glm::cross(vert.tangent, vert.normal);

                if (glm::dot(glm::cross(vert.normal, vert.tangent), vert.bitangent) < 0.0f)
                {
                    vert.tangent = vert.tangent * -1.0f;
                }
            }

            createVertexBuffers();

            mMaterial = MaterialManager::Get().createMaterial<MaterialPBR>();
        }

        void Render() override {

            mVertexArray->Bind();

            glDrawElements(GL_TRIANGLES, Mesh.Indices.size(), GL_UNSIGNED_INT, 0);

            mVertexArray->Unbind();
        }
        void Update() override {}
        void Destroy() override {}
        
        virtual void Draw(std::shared_ptr<Shader> shader, bool force_draw = false, std::initializer_list<unsigned int> mesh_index_list = {}) override
        {
            mVertexArray->Bind();

            glDrawElements(GL_TRIANGLES, Mesh.Indices.size(), GL_UNSIGNED_INT, 0);

            mVertexArray->Unbind();
        }
        
        virtual std::shared_ptr<MaterialPBR> getMaterial() override
        {
            return MaterialManager::Get().getMaterial<MaterialPBR>(mMaterial);
        }

        virtual MaterialManager::Material_id getMaterialId() override {
            return mMaterial;
        }

        void setMaterial(MaterialManager::Material_id mat_id) override {
            mMaterial = mat_id;
        }
    };

} // namespace Epsilon