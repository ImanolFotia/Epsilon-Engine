#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>

#include <unordered_map>

namespace Epsilon::API
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

    struct BOUNDING_BOX
    {
        float FRONT_TOP_LEFT = 0.f;
        float FRONT_TOP_RIGHT = 0.f;
        float FRONT_BOTTOM_LEFT = 0.f;
        float FRONT_BOTTOM_RIGHT = 0.f;

        float BACK_TOP_LEFT = 0.;
        float BACK_TOP_RIGHT = 0.f;
        float BACK_BOTTOM_LEFT = 0.f;
        float BACK_BOTTOM_RIGHT = 0.f;
    };

    constexpr unsigned EPS_TEXTURE1D = 1;
    constexpr unsigned EPS_TEXTURE2D = 2;
    constexpr unsigned EPS_TEXTURE3D = 3;
    constexpr unsigned EPS_ARRAY_BUFFER = 4;
    constexpr unsigned EPS_STREAM_DRAW = 4;

    namespace GL
    {

        std::unordered_map<unsigned, int> TYPES_LUT{
            {EPS_TEXTURE1D, GL_TEXTURE_1D},
            {EPS_TEXTURE2D, GL_TEXTURE_2D},
            {EPS_TEXTURE3D, GL_TEXTURE_3D},
            {EPS_ARRAY_BUFFER, GL_ARRAY_BUFFER},
            {EPS_STREAM_DRAW, GL_STREAM_DRAW}};
    }
} // namespace Epsilon