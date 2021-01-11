#pragma once

#include <Core.hpp>

namespace Epsilon
{
    namespace API
    {
        namespace OpenGL
        {
            class VertexBufferObject
            {
            public:
                VertexBufferObject()
                {
                    glGenBuffers(1, &this->m_Handler);
                }

                ~VertexBufferObject() {}

                void FillData(GLenum target, GLsizeiptr size, GLvoid *data, GLenum usage)
                {
                    m_Target = target;
                    glBindBuffer(m_Target, this->m_Handler);
                    glBufferData(m_Target, size, data, usage);
                }

                void UpdateData(GLintptr offset, GLsizeiptr size, const void *data) {
                    glBindBuffer(m_Target, this->m_Handler);
                    glBufferSubData(m_Target, offset, size, data);
                }

                GLuint Get()
                {
                    return m_Handler;
                }

                void Bind()
                {
                    glBindBuffer(m_Target, this->m_Handler);
                }

                void Update() {}

                void Destroy()
                {
                    glDeleteBuffers(1, &m_Handler);
                }

            private:
                GLuint m_Handler;
                GLenum m_Target;
            };
        } // namespace OpenGL
    } // namespace API
} // namespace Epsilon
