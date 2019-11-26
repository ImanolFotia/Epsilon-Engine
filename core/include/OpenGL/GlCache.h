#pragma once
#include <unordered_map>
#include <GL/glew.h>

class glCache
{
public:
    static void glUseProgram(GLuint program)
    {
        if(program != ShaderId)
        {
            ::glUseProgram(program);
            ShaderId = program;
        }
    }

    static void glCullFace(GLenum mode)
    {
        if(CullFaceMode != mode){
            ::glCullFace(mode);
            CullFaceMode = mode;
        }
    }

    static void glEnable(GLenum cap)
    {
        if(!glIsEnabled(cap))
        {
            ::glEnable(cap);
        }
    }

    static void glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid * indices)
    {
        ::glDrawElements(mode, count, type, indices);
        DrawCalls++;
    }

    static void glDrawArrays(GLenum mode, GLint first, GLsizei count)
    {
        ::glDrawArrays(mode, first, count);
        DrawCalls++;
    }


    static GLuint ShaderId;
    static GLenum CullFaceMode;
    static unsigned DrawCalls;
private:


};
