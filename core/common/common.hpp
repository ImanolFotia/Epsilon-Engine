#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

#include <vector>


namespace common {

    struct Vertex
    {
        Vertex() = default;

        Vertex(glm::vec3 p, glm::vec2 uv, glm::vec3 n) : position(p), texCoords(uv), normal(n) {}

        Vertex(glm::vec3 p, glm::vec2 uv, glm::vec3 n, glm::vec4 c, glm::vec3 t, glm::vec3 bt) : position(p), texCoords(uv), normal(n), color(c), tangent(t), bitangent(bt)
        {
        }

        alignas(32) glm::vec3 position = glm::vec3(0.0f);
        alignas(32) glm::vec2 texCoords = glm::vec2(0.0f);
        alignas(32) glm::vec3 normal = glm::vec3(0.0f);
        alignas(32) glm::vec4 color = glm::vec4(0.0f);
        alignas(32) glm::vec3 tangent = glm::vec3(0.0f);
        alignas(32) glm::vec3 bitangent = glm::vec3(0.0f);
    };


    struct Mesh
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

        std::vector<common::Vertex> Vertices;
        std::vector<uint32_t> Indices;
    };

    struct MIN_MAX_POINTS {
        float MIN_X = 0.0f;
        float MIN_Y = 0.0f;
        float MIN_Z = 0.0f;

        float MAX_X = 0.0f;
        float MAX_Y = 0.0f;
        float MAX_Z = 0.0f;
    };

    struct BoundingBox
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
}