#pragma once
#include <pch.hpp>
#include <Driver/API/OpenGL/VertexBufferObject.h>

namespace Epsilon
{
    namespace API
    {
        namespace OpenGL
        {
            class VertexArrayObject
            {
            public:
                VertexArrayObject()
                {
                    this->m_VertexAttributeCounter = 0;
                    this->m_Handler = 0;
                    glGenVertexArrays(1, &this->m_Handler);
                }

                ~VertexArrayObject() {}

                void setAttribute(int size, size_t stride, void *pointer)
                {
                    try
                    {
                        this->Bind();

                        glEnableVertexAttribArray(m_VertexAttributeCounter);
                        glVertexAttribPointer(m_VertexAttributeCounter, size, GL_FLOAT, GL_FALSE, stride, pointer);
                        this->m_VertexAttributeCounter++;

                        this->Unbind();
                    }
                    catch (...)
                    {
                        throw;
                    }
                }
                unsigned int addBuffer(GLenum target, GLsizeiptr size, GLvoid *data, GLenum usage)
                {
                    try
                    {
                        this->Bind();

                        VertexBufferObject tmpVBO;

                        tmpVBO.FillData(target, size, data, usage);

                        m_VBOArray.push_back(tmpVBO);

                        this->Unbind();

                        return m_VBOArray.size() - 1;
                    }
                    catch (...)
                    {
                        throw;
                    }
                }

                void UpdateBuffer(uint32_t bufferHandler, GLintptr offset, GLsizeiptr size, const void *data) {
                    auto & buffer = m_VBOArray[bufferHandler];
                    buffer.UpdateData(offset, size, data);
                }

                void Bind()
                {
                    glBindVertexArray(this->m_Handler);
                }

                void Unbind()
                {
                    glBindVertexArray(0);
                }

                void Destroy()
                {
                    glDeleteVertexArrays(1, &this->m_Handler);
                }

            private:
                std::vector<VertexBufferObject> m_VBOArray;
                int m_VertexAttributeCounter;
                GLuint m_Handler;
            };
        } // namespace OpenGL
    } // namespace API
} // namespace Epsilon
