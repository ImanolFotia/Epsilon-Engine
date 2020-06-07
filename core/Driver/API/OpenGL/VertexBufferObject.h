#pragma once

#include <GL/glew.h>

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
