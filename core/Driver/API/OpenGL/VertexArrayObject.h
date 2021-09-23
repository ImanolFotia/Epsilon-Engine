#pragma once
#include <pch.hpp>
#include <Driver/API/VertexArrayObject.hpp>

#include <Driver/API/OpenGL/VertexBufferObject.h>

namespace Epsilon::API::OpenGL
{
    class VertexArrayObject : public API::VertexArrayObject
    {
    public:
        VertexArrayObject()
        {
            this->m_VertexAttributeCounter = 0;
            this->m_Handler = 0;

            glGenVertexArrays(1, &m_Handler);
            glGenBuffers(1, &EBO);
        }

        ~VertexArrayObject() {}

        void setAttribute(int index, int size, GLsizei stride, const void *pointer)
        {
            try
            {
                Bind();

                auto &tmpVBO = m_VBOArray.back();
                tmpVBO.Bind();
                glEnableVertexAttribArray(index);
                glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, pointer);
                m_VertexAttributeCounter++;

                Unbind();
            }
            catch (...)
            {
                throw;
            }
        }
        unsigned int addBuffer(GLsizeiptr size, const GLvoid *data, GLenum usage)
        {
            try
            {
                this->Bind();

                OpenGL::VertexBufferObject tmpVBO;
                tmpVBO.Fill(size, data, usage);

                m_VBOArray.push_back(tmpVBO);

                this->Unbind();

                return m_VBOArray.size() - 1;
            }
            catch (...)
            {
                throw;
            }
        }

        void IndexBuffer(const std::vector<unsigned int> &indices)
        {
            Bind();
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_DYNAMIC_DRAW);
            Unbind();
        }

        void UpdateBuffer(uint32_t bufferHandler, GLintptr offset, GLsizeiptr size, const void *data)
        {
            auto &buffer = m_VBOArray[bufferHandler];
            buffer.Update(offset, size, data);
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
        std::vector<OpenGL::VertexBufferObject> m_VBOArray;
        GLuint EBO;
        int m_VertexAttributeCounter;
        GLuint m_Handler;
    };
} // namespace Epsilon
