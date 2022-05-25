#pragma once

#include <glm/glm.hpp>

#include <vector>

namespace engine
{
    const uint32_t MAX_VERTICES_PER_BUFFER = 10000;
    enum renderer_type
    {
        vulkan = 0,
        dx12,
        dx11,
        opengl,
        metal,
        size // if we ever want to iterate over renderer types
    };

    struct Vertex {
        Vertex(glm::vec3 p, glm::vec2 uv, glm::vec3 n, glm::vec4 c, glm::vec3 t, glm::vec3 bt) :
            position(p), texCoords(uv), normal(n), color(c), tangent(t), bitangent(bt) 
        {}
        
        glm::vec3 position;
        glm::vec2 texCoords;
        glm::vec3 normal;
        glm::vec4 color;
        glm::vec3 tangent;
        glm::vec3 bitangent;
    };

    struct MaterialInfo {};

    /**
     * @brief Renderer abstract class, do not instantiate directly
     *
     */
    class Renderer
    {
    public:
        virtual void Init(const char*, framework::Window&) = 0;

        virtual uint32_t Submit(const std::vector<Vertex>&, const MaterialInfo&) = 0;
        
        virtual void Push(uint32_t) = 0;
        
        virtual void Begin() = 0;

        virtual void End() = 0;

        virtual void Flush() = 0;

        virtual void Cleanup() = 0;

    public:
        /**
         * @brief Construct a new Renderer object
         *
         * @param type the underlying api this renderer will be using
         */
        Renderer(renderer_type type) : m_pType{type} {};
        Renderer() = default;

        Renderer(Renderer&&) = delete;
        Renderer(const Renderer&) = delete;

        virtual ~Renderer() {}

    protected:
        renderer_type m_pType;
    };
}