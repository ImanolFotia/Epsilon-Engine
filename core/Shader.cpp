///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================
#include <pch.hpp>
#include <Shader.h>
#include <Log.h>
#include <Helpers.hpp>
#include <Driver/API/OpenGL/ShaderPreProcessor.h>

namespace Epsilon
{
	Shader::Shader(const char *vertex, const char *fragment)
	{
		Path = std::string(vertex);
		mVertexPath = vertex;
		mFragmentPath = fragment;

		std::string VertexShaderCode = GLSLPreProcessor::PreProcessGLSL(std::string(vertex));

		std::string FragmentShaderCode = GLSLPreProcessor::PreProcessGLSL(std::string(fragment));

		GLuint VertexShaderID = generateVertexProgram(VertexShaderCode);

		GLuint FragmentShaderID = generateFragmentProgram(FragmentShaderCode);

		//std::cout << Path << std::endl;

		/// Link the program
		ProgramID = glCreateProgram();
		glAttachShader(ProgramID, VertexShaderID);
		glAttachShader(ProgramID, FragmentShaderID);
		glLinkProgram(ProgramID);

		GLint Result = GL_FALSE;
		int InfoLogLength;
		/// Check the program
		glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
		glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0)
		{
			std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
			glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
			//printf("%s\n", &ProgramErrorMessage[0]);
			std::cout << &ProgramErrorMessage[0] << std::endl;
			Log::WriteToLog("Error while compiling: " + Helpers::removeExtension(Path));
			Log::WriteToLog(std::string(&ProgramErrorMessage[0]));
		}
		glDeleteShader(VertexShaderID);
		glDeleteShader(FragmentShaderID);
		this->getUniformsLocations();
	}

	Shader::Shader(const char *comp_shader_path) : mComputePath(comp_shader_path)
	{
		GLuint ProgramID = glCreateProgram();
		GLuint cs = glCreateShader(GL_COMPUTE_SHADER);

		std::string csSrc = GLSLPreProcessor::PreProcessGLSL(mComputePath);
		const char *cs_code_cstr = csSrc.c_str();
		glShaderSource(cs, 2, &cs_code_cstr, NULL);
		glCompileShader(cs);
		int rvalue;
		glGetShaderiv(cs, GL_COMPILE_STATUS, &rvalue);
		if (!rvalue)
		{
			fprintf(stderr, "Error in compiling the compute shader\n");
			GLchar log[10240];
			GLsizei length;
			glGetShaderInfoLog(cs, 10239, &length, log);
			fprintf(stderr, "Compiler log:\n%s\n", log);
			exit(40);
		}
		glAttachShader(ProgramID, cs);

		glLinkProgram(ProgramID);
		glGetProgramiv(ProgramID, GL_LINK_STATUS, &rvalue);
		if (!rvalue)
		{
			fprintf(stderr, "Error in linking compute shader program\n");
			GLchar log[10240];
			GLsizei length;
			glGetProgramInfoLog(ProgramID, 10239, &length, log);
			fprintf(stderr, "Linker log:\n%s\n", log);
			exit(41);
		}

		glDeleteShader(cs);
		glUseProgram(ProgramID);
		this->getUniformsLocations();
	}

	Shader::Shader(const char* path, int zzz) {
		mShaderFile.Open(path);

		//mShaderFile[];
	}

	bool Shader::Reload()
	{
		std::string VertexShaderCode = GLSLPreProcessor::PreProcessGLSL(std::string(mVertexPath));

		std::string FragmentShaderCode = GLSLPreProcessor::PreProcessGLSL(std::string(mFragmentPath));

		std::string ComputeShaderCode = GLSLPreProcessor::PreProcessGLSL(std::string(mComputePath));

		GLuint VertexShaderID = generateVertexProgram(VertexShaderCode);

		GLuint FragmentShaderID = generateFragmentProgram(FragmentShaderCode);

		GLuint GeometryShaderID = 0;
		
		GLuint ComputeShaderID = 0;

		if (!mGeometryPath.empty())
		{
			std::string GeometryShaderCode = GLSLPreProcessor::PreProcessGLSL(std::string(mGeometryPath));
			GeometryShaderID = generateGeometryProgram(GeometryShaderCode);
		}

		/// Link the program
		//printf("Linking program\n");
		GLuint newProgramID = glCreateProgram();
		glAttachShader(newProgramID, VertexShaderID);
		glAttachShader(newProgramID, FragmentShaderID);
		glAttachShader(newProgramID, GeometryShaderID);
		glLinkProgram(newProgramID);

		GLint Result = GL_FALSE;
		int InfoLogLength;
		/// Check the program
		glGetProgramiv(newProgramID, GL_LINK_STATUS, &Result);
		glGetProgramiv(newProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0)
		{
			std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
			glGetProgramInfoLog(newProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
			//printf("%s\n", &ProgramErrorMessage[0]);
			std::cout << &ProgramErrorMessage[0] << std::endl;
			Log::WriteToLog("Error while compiling: " + Helpers::removeExtension(Path));
			Log::WriteToLog(std::string(&ProgramErrorMessage[0]));
			return false;
		}
		else
		{
			glDeleteShader(VertexShaderID);
			glDeleteShader(FragmentShaderID);
			glDeleteShader(GeometryShaderID);
			Destroy();
			ProgramID = newProgramID;
			this->getUniformsLocations();
			return true;
		}
	}

	Shader::Shader(const char *vertex, const char *fragment, const char *geometry)
	{
		Path = std::string(vertex);
		mVertexPath = vertex;
		mFragmentPath = fragment;
		mGeometryPath = geometry;

		std::string VertexShaderCode = GLSLPreProcessor::PreProcessGLSL(std::string(vertex));

		std::string FragmentShaderCode = GLSLPreProcessor::PreProcessGLSL(std::string(fragment));

		std::string GeometryShaderCode = GLSLPreProcessor::PreProcessGLSL(std::string(geometry));

		GLuint VertexShaderID = generateVertexProgram(VertexShaderCode);

		GLuint FragmentShaderID = generateFragmentProgram(FragmentShaderCode);

		GLuint GeometryShaderID = generateGeometryProgram(GeometryShaderCode);

		/// Link the program
		//printf("Linking program\n");
		ProgramID = glCreateProgram();
		glAttachShader(ProgramID, VertexShaderID);
		glAttachShader(ProgramID, FragmentShaderID);
		glAttachShader(ProgramID, GeometryShaderID);
		glLinkProgram(ProgramID);

		GLint Result = GL_FALSE;
		int InfoLogLength;
		/// Check the program
		glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
		glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0)
		{
			std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
			glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
			//printf("%s\n", &ProgramErrorMessage[0]);
			std::cout << &ProgramErrorMessage[0] << std::endl;
			Log::WriteToLog("Error while compiling: " + Helpers::removeExtension(Path));
			Log::WriteToLog(std::string(&ProgramErrorMessage[0]));
		}
		glDeleteShader(VertexShaderID);
		glDeleteShader(FragmentShaderID);
		glDeleteShader(GeometryShaderID);
		this->getUniformsLocations();
	}

	GLuint Shader::generateFragmentProgram(std::string pShaderString)
	{
		GLint Result = GL_FALSE;
		int InfoLogLength;
		/// Compile Fragment Shader
		//printf("Compiling shader : %s\n", fragment);
		GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
		char const *FragmentSourcePointer = pShaderString.c_str();
		glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
		glCompileShader(FragmentShaderID);

		/// Check Fragment Shader
		glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0)
		{
			std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
			glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
			printf("%s\n", &FragmentShaderErrorMessage[0]);
		}

		Log::WriteToLog("Fragment shader: " + std::string(mFragmentPath) + " was successfully compiled.");
		return FragmentShaderID;
	}

	GLuint Shader::generateVertexProgram(std::string pShaderString)
	{
		GLint Result = GL_FALSE;
		int InfoLogLength;
		GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		char const *VertexSourcePointer = pShaderString.c_str();
		glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
		glCompileShader(VertexShaderID);

		/// Check Vertex Shader
		glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0)
		{
			std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
			glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
			printf("%s\n", &VertexShaderErrorMessage[0]);
		}

		Log::WriteToLog("Vertex shader: " + std::string(mVertexPath) + " was successfully compiled.");
		return VertexShaderID;
	}

	GLuint Shader::generateGeometryProgram(std::string pShaderString)
	{
		GLint Result = GL_FALSE;
		int InfoLogLength;
		GLuint GeometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);
		char const *GeometrySourcePointer = pShaderString.c_str();
		glShaderSource(GeometryShaderID, 1, &GeometrySourcePointer, NULL);
		glCompileShader(GeometryShaderID);

		/// Check Vertex Shader
		glGetShaderiv(GeometryShaderID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(GeometryShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0)
		{
			std::vector<char> GeometryShaderErrorMessage(InfoLogLength + 1);
			glGetShaderInfoLog(GeometryShaderID, InfoLogLength, NULL, &GeometryShaderErrorMessage[0]);
			printf("%s\n", &GeometryShaderErrorMessage[0]);
		}

		return GeometryShaderID;
	}
	void Shader::getUniformsLocations()
	{

		int nrAttributes, nrUniforms;
		glGetProgramiv(ProgramID, GL_ACTIVE_ATTRIBUTES, &nrAttributes);
		glGetProgramiv(ProgramID, GL_ACTIVE_UNIFORMS, &nrUniforms);
		//m_Attributes.resize(nrAttributes);
		//m_Uniforms.resize(nrUniforms);

		char buffer[128];
		int Size;
		for (int i = 0; i < nrAttributes; ++i)
		{
			GLenum glType;
			glGetActiveAttrib(ProgramID, i, sizeof(buffer), 0, &Size, &glType, buffer);

			this->m_Attributes[std::string(buffer)] = glGetAttribLocation(ProgramID, buffer);
			//std::cout << "Attribute: " << i << " :: " << buffer << std::endl;
		}

		for (int i = 0; i < nrUniforms; ++i)
		{
			GLenum glType;
			glGetActiveUniform(ProgramID, i, sizeof(buffer), 0, &Size, &glType, buffer);

			this->m_Uniforms[std::string(buffer)] = glGetUniformLocation(ProgramID, buffer);
			//std::cout << "Uniform: " << i << " :: " << buffer << std::endl;
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
}