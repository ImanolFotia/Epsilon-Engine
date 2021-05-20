#pragma once 

#include <glad/glad.h>

namespace Epsilon::API {
    class VertexBufferObject {
        public:
        
                virtual void FillData(GLenum target, GLsizeiptr size, GLvoid *data, GLenum usage) = 0;

                virtual void UpdateData(GLintptr offset, GLsizeiptr size, const void *data) = 0;

                virtual GLuint Get() = 0;

                virtual void Bind() = 0;

                virtual void Update() = 0;

                virtual void Destroy() = 0;
    };
}