#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <vector>
#include <string>


namespace common {

    struct Vertex
    {
        Vertex() = default;

        Vertex(glm::vec3 p) : position(p){}

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

    struct AnimatedVertex
    {
        AnimatedVertex() = default;

        AnimatedVertex(glm::vec3 p) : position(p) {}

        AnimatedVertex(glm::vec3 p, glm::vec2 uv, glm::vec3 n) : position(p), texCoords(uv), normal(n) {}

        AnimatedVertex(glm::vec3 p, glm::vec2 uv, glm::vec3 n, glm::vec4 c, glm::vec3 t, glm::vec3 bt) : position(p), texCoords(uv), normal(n), color(c), tangent(t), bitangent(bt)
        {
        }

        alignas(32) glm::vec3 position = glm::vec3(0.0f);
        alignas(32) glm::vec2 texCoords = glm::vec2(0.0f);
        alignas(32) glm::vec3 normal = glm::vec3(0.0f);
        alignas(32) glm::vec4 color = glm::vec4(0.0f);
        alignas(32) glm::vec3 tangent = glm::vec3(0.0f);
        alignas(32) glm::vec3 bitangent = glm::vec3(0.0f);
        alignas(32) glm::ivec4 boneIDs = glm::ivec4(0);
        alignas(32) glm::vec4 weights = glm::vec4(0.0f);
    };

    template<typename VertexType>
    struct BaseMesh {
        using vertex_type = VertexType;
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

        std::vector<VertexType> Vertices;
        std::vector<uint32_t> Indices;
    };

    using Mesh = BaseMesh<common::Vertex>;
    using AnimatedMesh = BaseMesh<common::AnimatedVertex>;


    struct MeshMaterial {
        std::string albedo_path{};
        std::string metallic_path{};
        std::string normal_path{};
        std::string roughness_path{};

        glm::vec4 color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
        float metallic = 0.0f;
        float roughness = 0.5f;
    };

    struct MIN_MAX_POINTS {
        float MIN_X = 0.0f;
        float MIN_Y = 0.0f;
        float MIN_Z = 0.0f;

        float MAX_X = 0.0f;
        float MAX_Y = 0.0f;
        float MAX_Z = 0.0f;

        MIN_MAX_POINTS() = default;

        MIN_MAX_POINTS(glm::vec3 min, glm::vec3 max) {
            MIN_X = min.x;
            MIN_Y = min.y;
            MIN_Z = min.z;

            MAX_X = max.x;
            MAX_Y = max.y;
            MAX_Z = max.z;
            
            if (MIN_X > MAX_X) {
                float x = MAX_X;
                MAX_X = MIN_X;
                MIN_X = x;
            }

            if (MIN_Y > MAX_Y) {
                float y = MAX_Y;
                MAX_Y = MIN_Y;
                MIN_Y = y;
            }

            if (MIN_Z > MAX_Z) {
                float z = MAX_Z;
                MAX_Z = MIN_Z;
                MIN_Z = z;
            }

        }

        glm::vec3 getCenterOfMass() {
            return  glm::vec3(MAX_X + MIN_X, MAX_Y + MIN_Y, MAX_Z + MIN_Z) * 0.5f;
        }

        glm::vec3 getSize() {

            float mx = MIN_X < MAX_X ? MIN_X : MAX_X;
            float my = MIN_Y < MAX_Y ? MIN_Y : MAX_Y;
            float mz = MIN_Z < MAX_Z ? MIN_Z : MAX_Z;

            float Mx = MIN_X > MAX_X ? MIN_X : MAX_X;
            float My = MIN_Y > MAX_Y ? MIN_Y : MAX_Y;
            float Mz = MIN_Z > MAX_Z ? MIN_Z : MAX_Z;

            return glm::vec3(glm::abs(Mx - mx),
                glm::abs(My - my),
                glm::abs(Mz - mz));
        }

        glm::vec3 Min() {
            return glm::vec3(MIN_X, MIN_Y, MIN_Z);
        }

        glm::vec3 Max() {
            return glm::vec3(MAX_X, MAX_Y, MAX_Z);
        }

        void transform(glm::mat4 transform) {

            glm::vec3 scale;
            glm::quat rotation;
            glm::vec3 translation;
            glm::vec3 skew;
            glm::vec4 perspective;
            glm::decompose(transform, scale, rotation, translation, skew, perspective);

            glm::mat4 s_m = glm::scale(glm::mat4(1.0), scale);
            glm::mat4 r_m = glm::mat4(rotation);
            glm::mat4 t_m = glm::translate(glm::mat4(1.0), translation);
            glm::mat4 m_m = t_m * s_m;

            glm::vec3 min = m_m * glm::vec4(Min(), 1.0);
            glm::vec3 max = m_m * glm::vec4(Max(), 1.0);

            MIN_X = min.x;
            MIN_Y = min.y;
            MIN_Z = min.z;

            MAX_X = max.x;
            MAX_X = max.y;
            MAX_X = max.z;
        }
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