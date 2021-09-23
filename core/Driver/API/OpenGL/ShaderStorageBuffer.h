#pragma once
#include <Core.hpp>

#include "../ShaderStorage.hpp"

namespace Epsilon::API::OpenGL
{
    class ShaderStorageBuffer : public ShaderStorage
    {
    public:
        ShaderStorageBuffer(size_t size, unsigned int binding_point, size_t offset = 0, unsigned int usage = GL_DYNAMIC_DRAW)
        {
            mSize = size;
            mBindingPoint = binding_point;
            mOffset = offset;
            mUsage = usage;

            glGenBuffers(1, &mHandle);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, mHandle);
            glBufferData(GL_SHADER_STORAGE_BUFFER, size, nullptr, mUsage);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding_point, mHandle);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        }

        ~ShaderStorageBuffer() {}

        void Bind()
        {
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, mHandle);
        }

        void Unbind()
        {
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        }

        virtual void Init(const void *data) override
        {

            if (mHandle == 0)
            {
                std::cout << "Error: Shader Storage not initialized" << std::endl;
                return;
            }
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, mHandle);
            glBufferData(GL_SHADER_STORAGE_BUFFER, mSize, data, mUsage);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        }

        virtual void Update(size_t size, unsigned int offset, const void *data) override
        {
            if (mHandle == 0)
            {
                std::cout << "Error: Shader Storage not initialized" << std::endl;
                return;
            }

            glBindBuffer(GL_SHADER_STORAGE_BUFFER, mHandle);
            glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        }

        virtual void Get(void *data) override
        {
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, mHandle);
            glGetNamedBufferSubData(mHandle, 0, mSize, data);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        }

        virtual void GetSubData(size_t size, unsigned int offset, void *data) override
        {
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, mHandle);
            glGetNamedBufferSubData(mHandle, offset, size, data);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        }

    private:
        GLuint mHandle;
    };
} // namespace Epsilon
