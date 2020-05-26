///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#ifndef SHADER_H_INCLUDED
#define SHADER_H_INCLUDED

#include <GL/glew.h>
#include <string>
#include <iostream>
#include <unordered_map>
#include <glm/glm.hpp>

#include <Types.h>
#include <OpenGL/GlCache.h>

class Shader
{
public:

    //Default Vertex/Fragment Shader constructor
    Shader(const char*, const char*);

    //Vertex/Fragment/Geometry shader constructor
    Shader(const char*, const char*, const char*) ;

    //Vertex/Fragment/Geometry/Tessellation evaluation/Tessellation Control Shader Constructor
    Shader(const char*, const char*, const char*, const char*, const char*) {}

    //Compute Shader Constructor
    Shader(const char*) {}

    virtual ~Shader(void)
    {   //std::cout << "Shader Destroyed" << std::endl;
    }

    void Use(void)
    {   glCache::glUseProgram(this->ProgramID); }

    void Free(void)
    {   glCache::glUseProgram(0); }

    GLuint generateFragmentProgram(std::string);

    GLuint generateVertexProgram(std::string);

    GLuint generateGeometryProgram(std::string);

    GLuint getProgramID()
    {   return this->ProgramID; }

    std::string getPath()
    {   return this->Path; }

    void PushUniform(std::string name, int data)
    {
        if(m_Uniforms.find(name) == m_Uniforms.end()) return;
        glUniform1i(m_Uniforms[name], data);
    }

    void PushUniform(std::string name, float data)
    {
        if(m_Uniforms.find(name) == m_Uniforms.end()) return;
        glUniform1f(m_Uniforms[name], data);
    }

    void PushUniform(std::string name, glm::vec2 data)
    {
        if(m_Uniforms.find(name) == m_Uniforms.end()) return;
        glUniform2fv(m_Uniforms[name], 1, &data[0]);
    }

    void PushUniform(std::string name, glm::vec3 data)
    {
        if(m_Uniforms.find(name) == m_Uniforms.end()) return;
        glUniform3fv(m_Uniforms[name], 1, &data[0]);
    }

    void PushUniform(std::string name, glm::vec4 data)
    {
        if(m_Uniforms.find(name) == m_Uniforms.end()) return;
        glUniform4fv(m_Uniforms[name], 1, &data[0]);
    }

    void PushUniform(std::string name, glm::mat3 data)
    {
        if(m_Uniforms.find(name) == m_Uniforms.end()) return;
        glUniformMatrix3fv(m_Uniforms[name], 1, GL_FALSE, &data[0][0]);
    }

    void PushUniform(std::string name, glm::mat4 data)
    {
        glUniformMatrix4fv(m_Uniforms[name], 1, GL_FALSE, &data[0][0]);
    }

    GLuint getUniformLocation(std::string name) {
        return m_Uniforms.at(name);
    }

private:

    GLuint ProgramID;
    std::string Path;

    std::string vPath, fPath;

    std::unordered_map<std::string, GLuint> m_Attributes;
    std::unordered_map<std::string, GLuint> m_Uniforms;

    void getUniformsLocations();

public:
    GLuint MVP_Location;
    GLuint WorldTransform_Location;
    GLuint Projection_Location;
    GLuint View_Location;
    GLuint ViewDirection_Location;
    GLuint ViewPosition_Location;
    GLuint LightSpaceMatrix_Location;
    GLuint ModelView3x3Matrix_Location;
    GLuint NormalMatrix_Location;
    GLuint LightDirection_Location;
    GLuint ClipPlane_Location;
    GLuint Time_Location;
    GLuint texture_diffuse_Location;
    GLuint texture_specular_Location;
    GLuint texture_normal_Location;
    GLuint texture_height_Location;
    GLuint skybox_Location;
    GLuint viewPos_Location;
    GLuint PrevViewPos_Location;

};


#endif /// SHADER_H_INCLUDED
