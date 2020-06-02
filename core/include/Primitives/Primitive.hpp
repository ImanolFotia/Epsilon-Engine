#pragma once

#include <vector>
#include <glm/glm.hpp>

#include <OpenGL/VertexArrayObject.h>
#include <OpenGL/VertexBufferObject.h>

namespace Epsilon
{

    class Primitive
    {
    public:
        virtual void Update() = 0;

        virtual void Render() = 0;

        virtual void Destroy() = 0;

        struct Vertex
        {
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