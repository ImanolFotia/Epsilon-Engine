#pragma once
#include <unordered_map>
#include <Core.hpp>

class glCache
{
public:
    inline static void glUseProgram(GLuint program)
    {
        if(program != ShaderId)
        {
            ::glUseProgram(program);
            ShaderId = program;
        }
    }

    inline static void glCullFace(GLenum mode)
    {
        if(CullFaceMode != mode){
            ::glCullFace(mode);
            CullFaceMode = mode;
        }
    }

    inline static void glEnable(GLenum cap)
    {
        if(!glIsEnabled(cap))
        {
            ::glEnable(cap);
        }
    }

    inline static void glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid * indices)
    {
        ::glDrawElements(mode, count, type, indices);
        DrawCalls++;
    }

    inline static void glDrawArrays(GLenum mode, GLint first, GLsizei count)
    {
        ::glDrawArrays(mode, first, count);
        DrawCalls++;
    }


    static GLuint ShaderId;
    static GLenum CullFaceMode;
    static unsigned DrawCalls;
private:


};
