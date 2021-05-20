#pragma once

#include <glad/glad.h>

#include "../BufferObject.hpp"

namespace Epsilon::API::OpenGL
{
    class UniformBuffer : public BufferObject
    {
    public:
        UniformBuffer() = default;
        UniformBuffer(size_t size, unsigned int binding_point, size_t offset = 0, unsigned int usage = GL_STATIC_DRAW)
        {
            mSize = size;
            mOffset = offset;
            mUsage = usage;

            glGenBuffers(1, &mHandle);
            glBindBuffer(GL_UNIFORM_BUFFER, mHandle);
            glBufferData(GL_UNIFORM_BUFFER, mSize, nullptr, mUsage);
            glBindBufferRange(GL_UNIFORM_BUFFER, binding_point, mHandle, offset, mSize);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }

        void Bind() override
        {
            glBindBuffer(GL_UNIFORM_BUFFER, mHandle);
        }

        void Unbind() override
        {
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }

        void Init(const void *data) override
        {
            if (mHandle == 0)
            {
                std::cout << "Error: buffer not initialized" << std::endl;
                return;
            }

            glBindBuffer(GL_UNIFORM_BUFFER, mHandle);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, mSize, data);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }

        void Update(size_t size, unsigned int offset, const void *data) override
        {

            if (mHandle == 0)
            {
                std::cout << "Error: buffer not initialized" << std::endl;
                return;
            }

            glBindBuffer(GL_UNIFORM_BUFFER, mHandle);
            glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }
    };
} // namespace Epsilon