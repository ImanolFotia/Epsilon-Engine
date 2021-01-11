#pragma once

#include <glm/glm.hpp>

namespace Epsilon
{
    namespace API
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
    } // namespace API
} // namespace Epsilon