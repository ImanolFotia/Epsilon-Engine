//========= Copyright Survtech, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================
#ifndef SHADER_H_INCLUDED
#define SHADER_H_INCLUDED

#include <GL/glew.h>
#include <string>

class Shader{
public:

   Shader(const char*, const char*);
   virtual ~Shader(void){}

    GLuint ProgramID;

    void Use(void){glUseProgram(this->ProgramID);}
};


#endif // SHADER_H_INCLUDED
