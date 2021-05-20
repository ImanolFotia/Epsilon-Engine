#pragma once

#include <glad/glad.h>

#include "3DTypes.hpp"

namespace Epsilon::API {
    class VertexArrayObject {
        public:
        
        virtual void setAttribute(int size, size_t stride, void *pointer) = 0;

        virtual unsigned int addBuffer(unsigned target, GLsizeiptr size, GLvoid *data, GLenum usage) = 0;

        virtual void UpdateBuffer(uint32_t bufferHandler, GLintptr offset, GLsizeiptr size, const void *data) = 0;

        virtual void Bind() = 0;

        virtual void Unbind() = 0;

        virtual void Destroy() = 0;
    };
}