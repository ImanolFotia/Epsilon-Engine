#pragma once

#include <glad/glad.h>

#include "../DrawCommandType.hpp"

namespace Epsilon::API::OpenGL
{
    struct DrawCommandData_t : DrawCommandDataBase_t {
        GLuint a;
        GLuint b;
        GLuint c;
        GLuint d;
        GLuint e;
    };
    struct MultiDrawCommandDataIndirect_t : DrawCommandData_t
    {
        GLuint vertexCount;
        GLuint instanceCount;
        GLuint firstIndex;
        GLuint baseVertex;
        GLuint baseInstance;
    };
    
    struct DrawCommandDataIndirect_t : DrawCommandData_t
    {
        GLuint vertexCount;
        GLuint instanceCount;
        GLuint firstIndex;
        GLuint baseVertex;
        const GLuint reservedMustBeZero = 0;
    };

}