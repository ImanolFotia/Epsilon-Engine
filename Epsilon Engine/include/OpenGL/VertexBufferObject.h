#pragma once

#include <GL/glew.h>

class VertexBufferObject
{
public:
    VertexBufferObject()
    {
        glGenBuffers(1, &this->m_Handler);
    }

    ~VertexBufferObject(){}

    void FillData(GLenum target, GLsizeiptr size, GLvoid* data, GLenum usage)
    {
        glBindBuffer(target, this->m_Handler);
        glBufferData(target, size, data, usage);
    }

    void Update(){}

    void Destroy()
    {
        glDeleteBuffers(1, &m_Handler);
    }

private:
    GLuint m_Handler;

};
