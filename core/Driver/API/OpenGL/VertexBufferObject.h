#pragma once

#include <Core.hpp>

#include <Driver/API/VertexBufferObject.hpp>

namespace Epsilon::API::OpenGL
        {
            class VertexBufferObject : public API::VertexBufferObject
            {
            public:
                VertexBufferObject()
                {
                    glGenBuffers(1, &this->m_Handler);
                }

                ~VertexBufferObject() {}

                void Reserve(std::size_t size) override {
                    glBindBuffer(GL_ARRAY_BUFFER, this->m_Handler);
                    glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
                }

                void Fill(GLsizeiptr size, const GLvoid *data, GLenum usage) override
                {
                    glBindBuffer(GL_ARRAY_BUFFER, this->m_Handler);
                    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
                }

                void Update(GLintptr offset, GLsizeiptr size, const void *data) override {
                    glBindBuffer(GL_ARRAY_BUFFER, this->m_Handler);
                    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
                }

                GLuint Get() override
                {
                    return m_Handler;
                }

                void Bind() override
                {
                    glBindBuffer(GL_ARRAY_BUFFER, this->m_Handler);
                }

                void Destroy() override
                {
                    glDeleteBuffers(1, &m_Handler);
                }

            private:
                GLuint m_Handler;
            };
} // namespace Epsilon
