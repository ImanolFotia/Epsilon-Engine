///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#ifndef SHADER_H_INCLUDED
#define SHADER_H_INCLUDED

#include <pch.hpp>
#include <Core.hpp>
#include <glm/glm.hpp>

#include <Types.h>
#include <Driver/API/OpenGL/GlCache.h>

namespace Epsilon {
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
    {   /*glCache::*/glUseProgram(this->ProgramID); }

    void Free(void)
    {   /*glCache::*/glUseProgram(0); }

    bool Reload();

    void Destroy() {
        glUseProgram(0);
	    glDeleteProgram(this->ProgramID);
    }

    GLuint generateFragmentProgram(std::string);

    GLuint generateVertexProgram(std::string);

    GLuint generateGeometryProgram(std::string);

    GLuint getProgramID()
    {   return this->ProgramID; }

    std::string getPath()
    {   return this->Path; }

    void PushUniform(const std::string& name, int data)
    {
        if(m_Uniforms.find(name) == m_Uniforms.end()) return;
        glUniform1i(m_Uniforms[name], data);
    }

    void PushUniform(const std::string& name, float data)
    {
        if(m_Uniforms.find(name) == m_Uniforms.end()) return;
        glUniform1f(m_Uniforms[name], data);
    }

    void PushUniform(const std::string& name, glm::vec2 data)
    {
        if(m_Uniforms.find(name) == m_Uniforms.end()) return;
        glUniform2fv(m_Uniforms[name], 1, &data[0]);
    }

    void PushUniform(const std::string& name, glm::vec3 data)
    {
        if(m_Uniforms.find(name) == m_Uniforms.end()) return;
        glUniform3fv(m_Uniforms[name], 1, &data[0]);
    }
    
    void PushUniform(const std::string& name, glm::ivec3 data)
    {
        if(m_Uniforms.find(name) == m_Uniforms.end()) return;
        glUniform3iv(m_Uniforms[name], 1, &data[0]);
    } 

    void PushUniform(const std::string& name, glm::vec4 data)
    {
        if(m_Uniforms.find(name) == m_Uniforms.end()) return;
        glUniform4fv(m_Uniforms[name], 1, &data[0]);
    }

    void PushUniform(const std::string& name, glm::mat3 data)
    {
        if(m_Uniforms.find(name) == m_Uniforms.end()) return;
        glUniformMatrix3fv(m_Uniforms[name], 1, GL_FALSE, &data[0][0]);
    }

    void PushUniform(const std::string& name, glm::mat4 data)
    {
        if(m_Uniforms.find(name) == m_Uniforms.end()) return;
        glUniformMatrix4fv(m_Uniforms[name], 1, GL_FALSE, &data[0][0]);
    }

    GLuint getUniformLocation(const std::string&  name) {
        return m_Uniforms.at(name);
    }

private:

    GLuint ProgramID;
    std::string Path;

    std::string mVertexPath = "";
    std::string mFragmentPath = "";
    std::string mGeometryPath = "";

    std::string vPath, fPath;

    std::unordered_map<std::string, GLuint> m_Attributes;
    std::unordered_map<std::string, GLuint> m_Uniforms;

    void getUniformsLocations();

public:
    GLuint MVP_Location = -1;
    GLuint WorldTransform_Location = -1;
    GLuint Projection_Location = -1;
    GLuint View_Location = -1;
    GLuint ViewDirection_Location = -1;
    GLuint ViewPosition_Location = -1;
    GLuint LightSpaceMatrix_Location = -1;
    GLuint ModelView3x3Matrix_Location = -1;
    GLuint NormalMatrix_Location = -1;
    GLuint LightDirection_Location = -1;
    GLuint ClipPlane_Location = -1;
    GLuint Time_Location = -1;
    GLuint texture_diffuse_Location = -1;
    GLuint texture_specular_Location = -1;
    GLuint texture_normal_Location = -1;
    GLuint texture_height_Location = -1;
    GLuint skybox_Location = -1;
    GLuint viewPos_Location = -1;
    GLuint PrevViewPos_Location = -1;

};
using Shader_ptr = std::shared_ptr<Shader>;
}

#endif /// SHADER_H_INCLUDED
