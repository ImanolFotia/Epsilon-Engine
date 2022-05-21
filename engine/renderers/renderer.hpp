#pragma once

class framework::Window{};

namespace engine
{
    enum renderer_type
    {
        vulkan = 0,
        dx12,
        dx11,
        opengl,
        metal,
        size // if we ever want to iterate over renderer types
    };

    /**
     * @brief Renderer abstract class, do not instantiate directly
     *
     */
    class Renderer
    {
    public:
        virtual void Init(const char*, framework::Window&) = 0;
        
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