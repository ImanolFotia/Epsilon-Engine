#pragma once

#include <Driver/API/3DTypes.hpp>
#include <Renderer/EML/eml1_0.h>

#include <vector>

namespace Epsilon::API::OpenGL
{
    class Mesh
    {

        std::vector<Vertex> mVertices;

    public:
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

    public:
        Mesh() = default;

        void Init(std::vector<t_Vertex> vertices, std::vector<GLuint> indices)
        {
        }
    };
} // namespace Epsilon