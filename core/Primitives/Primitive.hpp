#pragma once

#include <pch.hpp>
#include <glm/glm.hpp>

#include <Driver/API/OpenGL/VertexArrayObject.h>
#include <Driver/API/OpenGL/VertexBufferObject.h>

namespace Epsilon
{

    class PrimitiveBase
    {
    public:
        virtual void Update() = 0;

        virtual void Render() = 0;

        virtual void Destroy() = 0;

        struct Vertex
        {
            Vertex(glm::vec3 p, glm::vec3 n, glm::vec2 u) {
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

    protected:
        std::vector<Vertex> mVertices;

        API::OpenGL::VertexArrayObject mVertexArray;
        API::OpenGL::VertexBufferObject mVertexBuffer;
    };

} // namespace Epsilon