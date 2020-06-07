#pragma once
#include <GL/glew.h>

template <typename T>
class ShaderStorageBuffer {
public:
    ShaderStorageBuffer(T* data, int sData) {
        glGenBuffers(1, &SSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sData, data, GL_DYNAMIC_COPY);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, SSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    ~ShaderStorageBuffer() {}

    void Bind() {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, SSBO);
    }

    void Unbind() {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
    }

private:
    GLuint SSBO;
};

