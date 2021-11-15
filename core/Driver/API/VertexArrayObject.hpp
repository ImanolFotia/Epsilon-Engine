#pragma once

#include <glad/glad.h>

#include "3DTypes.hpp"

namespace Epsilon::API {
    class VertexBufferObject;
    class VertexArrayObject {
        public:
        
        virtual void setAttribute(int index, int size, GLsizei  stride,  const void *pointer) = 0;

        virtual unsigned int addBuffer(GLsizeiptr size, const GLvoid *data, GLenum usage) = 0;

        virtual void UpdateBuffer(uint32_t bufferHandler, GLintptr offset, GLsizeiptr size, const void *data) = 0;

        virtual void IndexBuffer(const std::vector<unsigned int>& indices) = 0;

        virtual void Bind() = 0;

        virtual void Unbind() = 0;

        virtual void Destroy() = 0;
        
        virtual VertexBufferObject* getBuffer(unsigned int id) = 0;

        virtual void DestroyBuffer(unsigned int Id) = 0;
    };
}