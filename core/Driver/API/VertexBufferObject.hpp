#pragma once 

#include <glad/glad.h>

namespace Epsilon::API {
    class VertexBufferObject {
        public:
        
                virtual void Fill(GLsizeiptr size, const GLvoid *data, GLenum usage) = 0;

                virtual void Update(GLintptr offset, GLsizeiptr size, const void *data) = 0;
                
                virtual void Reserve(std::size_t size) = 0;

                virtual GLuint Get() = 0;

                virtual void Bind() = 0;

                virtual void Destroy() = 0;
    };
}