///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================
#include <Shader.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <Log.h>
#include <Includes.h>

Shader::Shader(const char* vertex, const char* fragment)
{
    Path = std::string(vertex);
    std::cout << fragment << std::endl;
    /// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	/// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex, std::ios::in);
	if(VertexShaderStream.is_open()){
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}else{
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex);
		getchar();
		return ;
	}

	/// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}



	GLint Result = GL_FALSE;
	int InfoLogLength;



	/// Compile Vertex Shader
	//printf("Compiling shader : %s\n", vertex);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	/// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}



	/// Compile Fragment Shader
	//printf("Compiling shader : %s\n", fragment);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	/// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}



	/// Link the program
	//printf("Linking program\n");
    ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	/// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		char ProgramErrorMessage[InfoLogLength+1];
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, ProgramErrorMessage);
		//printf("%s\n", &ProgramErrorMessage[0]);
		std::cout << ProgramErrorMessage << std::endl;
		Global::Log::WriteToLog("Error while compiling: " + Helpers::removeExtension(Path));
		Global::Log::WriteToLog(std::string(ProgramErrorMessage));
	}

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	this->getUniformsLocations();


}

void Shader::getUniformsLocations()
{

    // NOTE(Joey): query the number of active uniforms and attributes
        int nrAttributes, nrUniforms;
        glGetProgramiv(ProgramID, GL_ACTIVE_ATTRIBUTES, &nrAttributes);
        glGetProgramiv(ProgramID, GL_ACTIVE_UNIFORMS, &nrUniforms);
        //m_Attributes.resize(nrAttributes);
        //m_Uniforms.resize(nrUniforms);

        // NOTE(Joey): iterate over all active attributes
        char buffer[128];
        int Size;
        for (unsigned int i = 0; i < nrAttributes; ++i)
        {
            GLenum glType;
            glGetActiveAttrib(ProgramID, i, sizeof(buffer), 0, &Size, &glType, buffer);

            this->m_Uniforms[std::string(buffer)] = glGetAttribLocation(ProgramID, buffer);
        }

        // NOTE(Joey): iterate over all active uniforms
        for (unsigned int i = 0; i < nrUniforms; ++i)
        {
            GLenum glType;
            glGetActiveUniform(ProgramID, i, sizeof(buffer), 0, &Size, &glType, buffer);

            this->m_Uniforms[std::string(buffer)] = glGetUniformLocation(ProgramID, buffer);
        }

    this->MVP_Location = glGetUniformLocation(this->ProgramID, "MVP");
    this->WorldTransform_Location = glGetUniformLocation(this->ProgramID, "model");
    this->Projection_Location = glGetUniformLocation(this->ProgramID, "projection");
    this->View_Location = glGetUniformLocation(this->ProgramID, "view");
    this->LightSpaceMatrix_Location = glGetUniformLocation(this->ProgramID, "lightSpaceMatrix");
    this->viewPos_Location = glGetUniformLocation(this->ProgramID, "viewPos");
    this->PrevViewPos_Location = glGetUniformLocation(this->ProgramID, "PrevView");
    //this->MVP_Location = glGetUniformLocation(this->ProgramID, "MVP");
    //this->MVP_Location = glGetUniformLocation(this->ProgramID, "MVP");
}
