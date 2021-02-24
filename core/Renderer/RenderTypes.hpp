#pragma once

#include <Engine.hpp>

#ifdef COMPILE_FOR_GL
#include <GL/glad.h>
#endif

namespace Epsilon {
    constexpr inline uint32_t DynamicDraw() {
        auto & context = Engine::Get().ContextType();
        
        switch(context) {
            case GL:
                return GL_DYNAMIC_DRAW;
            break;
            case D3D: break;
            case VULKAN: break;
            case METAL: break;
        }
    }

    constexpr inline uint32_t StaticDraw() {
        auto & context = Engine::Get().ContextType();
        
        switch(context) {
            case GL:
                return GL_STATIC_DRAW;
            break;
            case D3D: break;
            case VULKAN: break;
            case METAL: break;
        }
    }

    constexpr inline uint32_t StreamDraw() {
        auto & context = Engine::Get().ContextType();
        
        switch(context) {
            case GL:
                return GL_STREAM_DRAW;
            break;
            case D3D: break;
            case VULKAN: break;
            case METAL: break;
        }
    }
}