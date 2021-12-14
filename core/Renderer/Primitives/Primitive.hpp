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


    protected:
        void createVertexBuffers()
        {
            mVertexArray = std::make_shared<API::OpenGL::VertexArrayObject>();

            mVertexArray->addBuffer(Mesh.Vertices.size() * sizeof(Vertex), &Mesh.Vertices[0], GL_STATIC_DRAW);
            mVertexArray->setAttribute(VERTEX_POINTER_INDEX::POSITION, 3, sizeof(Vertex), (GLvoid *)0);
            mVertexArray->setAttribute(VERTEX_POINTER_INDEX::UV, 2, sizeof(Vertex), (void *)offsetof(Vertex, uv));
            mVertexArray->setAttribute(VERTEX_POINTER_INDEX::NORMAL, 3, sizeof(Vertex), (void *)offsetof(Vertex, normal));
            mVertexArray->setAttribute(VERTEX_POINTER_INDEX::TANGENT, 3, sizeof(Vertex), (void *)offsetof(Vertex, tangent));
            mVertexArray->setAttribute(VERTEX_POINTER_INDEX::BITANGENT, 3, sizeof(Vertex), (void *)offsetof(Vertex, bitangent));

            mVertexArray->IndexBuffer(Mesh.Indices);
        }

    protected:
        std::shared_ptr<API::VertexArrayObject> mVertexArray;
        MaterialManager::Material_id mMaterial;
    };

} // namespace Epsilon