#pragma once

#include "primitive.hpp"

namespace engine
{
    class Sphere : public Primitive
    {
    public:
        Sphere(uint32_t subdivisions) : mNumSubdivisions(subdivisions)
        {
            m_pType = DrawableType::SPHERE;
            createGeometry(mNumSubdivisions);
        }

        Sphere() = default;


    private:
        void createGeometry(uint32_t subdivisions)
        {
            const double PI = 3.14159; //:)
            // Generate vertices
            m_pMesh.Vertices.emplace_back(glm::vec3(0.0, 1.0, 0.0), glm::vec2(1.0), glm::vec3(0.0, 1.0, 0.0));

            int parallel_count = subdivisions;
            int meridian_count = subdivisions * 2;

            for (int j = 0; j < parallel_count - 1; ++j)
            {
                double polar = PI * double(j + 1) / parallel_count;
                double sin_polar = glm::sin(polar);
                double cos_polar = glm::cos(polar);
                for (int i = 0; i < meridian_count; ++i)
                {
                    double az = PI * 2.0 * double(i) / double(meridian_count);
                    double sin_az = glm::sin(az);
                    double cos_az = glm::cos(az);
                    double x = sin_polar * cos_az;
                    double y = cos_polar;
                    double z = sin_polar * sin_az;

                    glm::vec3 p = glm::vec3(x, y, z);
                    glm::vec3 n = glm::normalize(glm::vec3(0.0f) - p);
                    glm::vec2 uv = glm::vec2( float(i) / float(meridian_count-1), n.y * 0.5 + 0.5);

                    m_pMesh.Vertices.emplace_back(p, uv, n);
                }
            }

            //0.2
            //0.0, 0.2, 0.4, 0.6, 0.8
            m_pMesh.Vertices.emplace_back(glm::vec3(0.0, -1.0, 0.0), glm::vec2(0.0), glm::vec3(0.0, -1.0, 0.0));

            // Generate triangles from vertices
            for (int i = 0; i < meridian_count; ++i)
            {
                uint32_t a = i + 1;
                uint32_t b = (i + 1) % meridian_count + 1;

                m_pMesh.addTriangle(0, b, a);
            }

            for (unsigned j = 0; j < parallel_count - 2; ++j)
            {
                uint32_t beginA = j * meridian_count + 1;
                uint32_t beginD = (j + 1) * meridian_count + 1;
                for (int i = 0; i < meridian_count; ++i)
                {
                    uint32_t a = beginA + i;
                    uint32_t b = beginA + (i + 1) % meridian_count;
                    uint32_t d = beginD + (i + 1) % meridian_count;
                    uint32_t c = beginD + i;
                    m_pMesh.addQuad(a, b, d, c);
                }
            }

            for (int i = 0; i < meridian_count; ++i)
            {
                uint32_t a = i + meridian_count * (parallel_count - 2) + 1;
                uint32_t b = (i + 1) % meridian_count + meridian_count * (parallel_count - 2) + 1;

                m_pMesh.addTriangle(m_pMesh.Vertices.size() - 1, a, b);
            }

            // Generate Normals from vertices
            for (unsigned int i = 0; i < m_pMesh.Indices.size(); i += 3)
            {
                glm::vec3 v0 = m_pMesh.Vertices[m_pMesh.Indices[i]].position;
                glm::vec3 v1 = m_pMesh.Vertices[m_pMesh.Indices[i + 1]].position;
                glm::vec3 v2 = m_pMesh.Vertices[m_pMesh.Indices[i + 2]].position;

                glm::vec3 normal = glm::cross(v2 - v0, v1 - v0);

                m_pMesh.Vertices[m_pMesh.Indices[i]].normal = glm::normalize(-normal);
                m_pMesh.Vertices[m_pMesh.Indices[i + 1]].normal = glm::normalize(-normal);
                m_pMesh.Vertices[m_pMesh.Indices[i + 2]].normal = glm::normalize(-normal);
            }

            // Generate tangent space vectors

            uint32_t num_triangles = m_pMesh.Indices.size() / 3;
            // calculate tangent/bitangent vectors of both triangles
            glm::vec3 tangent1, bitangent1;
            // std::cout << mesh.m_Tris.size() <<  std::endl;
            for (int i = 0; i < m_pMesh.Indices.size(); i += 3)
            {
                glm::vec3 v0 = m_pMesh.Vertices[m_pMesh.Indices[i]].position;
                glm::vec3 v1 = m_pMesh.Vertices[m_pMesh.Indices[i + 1]].position;
                glm::vec3 v2 = m_pMesh.Vertices[m_pMesh.Indices[i + 2]].position;

                glm::vec3 edge1 = v1 - v0;
                glm::vec3 edge2 = v2 - v0;

                glm::vec2 t0 = m_pMesh.Vertices[m_pMesh.Indices[i]].texCoords;
                glm::vec2 t1 = m_pMesh.Vertices[m_pMesh.Indices[i + 1]].texCoords;
                glm::vec2 t2 = m_pMesh.Vertices[m_pMesh.Indices[i + 2]].texCoords;

                glm::vec2 deltaUV1 = t1 - t0;
                glm::vec2 deltaUV2 = t2 - t0;

                // GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y + deltaUV2.x * deltaUV1.y);

                float f = (deltaUV2.x * deltaUV1.x - deltaUV2.y * deltaUV1.y) == 0.0f ? -1.0f : 1.0f;
                // when t1, t2, t3 in same position in UV space, just use default UV direction.
                /* if (0 == deltaUV2.x && 0 == deltaUV2.y && 0 == deltaUV1.x && 0 == deltaUV1.y)
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

                m_pMesh.Vertices[m_pMesh.Indices[i]].tangent = tangent1;
                m_pMesh.Vertices[m_pMesh.Indices[i + 1]].tangent = tangent1;
                m_pMesh.Vertices[m_pMesh.Indices[i + 2]].tangent = tangent1;

                bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
                bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
                bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
                // bitangent1 = glm::normalize(bitangent1);

                m_pMesh.Vertices[m_pMesh.Indices[i]].bitangent = bitangent1;
                m_pMesh.Vertices[m_pMesh.Indices[i + 1]].bitangent = bitangent1;
                m_pMesh.Vertices[m_pMesh.Indices[i + 2]].bitangent = bitangent1;

                m_pMesh.Vertices[m_pMesh.Indices[i]].color = glm::vec4(1.0);
                m_pMesh.Vertices[m_pMesh.Indices[i + 1]].color = glm::vec4(1.0);
                m_pMesh.Vertices[m_pMesh.Indices[i + 2]].color = glm::vec4(1.0);
            }
            /*
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
                        }*/
        }

        uint32_t mNumSubdivisions = 4;
    };
}