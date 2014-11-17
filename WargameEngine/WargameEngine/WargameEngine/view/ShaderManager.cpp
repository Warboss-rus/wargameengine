#include "ShaderManager.h"
#define GLEW_STATIC
#include <GL/glew.h>
#include "gl.h"
#include <fstream>
#include "../LogWriter.h"
#include "../Module.h"

void CShaderManager::BindProgram() const
{
	if(m_program != -1)
		glUseProgram(m_program);
}

void CShaderManager::UnBindProgram() const
{
	glUseProgram(0);
}

GLuint CompileShader(std::string const& path, GLuint program, GLenum type)
{
	std::string shaderText;
	std::string line;
	std::ifstream iFile(path);
	while(std::getline(iFile, line))
	{
		shaderText += line + '\n';
	}
	iFile.close();
	GLuint shader = glCreateShader(type);
	glAttachShader(program, shader);
	GLcharARB const * text = shaderText.c_str();
	glShaderSource(shader, 1, &text, NULL);
	glCompileShader(shader);
	GLint compileStatus;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus != GL_TRUE)
	{
		char buffer[1000];
		int size = 0;
		glGetShaderInfoLog(shader, 1000, &size, buffer);
		LogWriter::WriteLine(std::string("Shader error: ") + buffer);
	}
	return shader;
}

void CShaderManager::NewProgram(std::string const& vertex, std::string const& fragment, std::string const& geometry)
{
	if (!GLEW_ARB_shader_objects)
	{
		LogWriter::WriteLine("Shader objects(GL_ARB_shader_objects) are not supported");
		return;
	}
	m_program = glCreateProgram();
	GLuint vertexShader(0), framgentShader(0), geometryShader(0);
	if(!vertex.empty())
	{
		if (!GLEW_ARB_vertex_shader)
		{
			LogWriter::WriteLine("Vertex Shaders(GL_ARB_vertex_shader) are not supported");
		}
		else
		{
			vertexShader = CompileShader(sModule::shaders + vertex, m_program, GL_VERTEX_SHADER);
		}
	}
	if(!fragment.empty())
	{
		if (!GLEW_ARB_fragment_shader)
		{
			LogWriter::WriteLine("Fragment Shaders(GL_ARB_fragment_shader) are not supported");
		}
		else
		{
			framgentShader = CompileShader(sModule::shaders + fragment, m_program, GL_FRAGMENT_SHADER);
		}
	}
	if(!geometry.empty())
	{
		if (!GLEW_ARB_geometry_shader4)
		{
			LogWriter::WriteLine("Geometry Shaders(GL_ARB_geometry_shader4) are not supported");
		}
		else
		{
			geometryShader = CompileShader(sModule::shaders + geometry, m_program, GL_GEOMETRY_SHADER);
		}
	}
	glBindAttribLocation(m_program, 16, "weights");
	glBindAttribLocation(m_program, 17, "weightIndices");
	glLinkProgram(m_program);
	glUseProgram(m_program);
	int unfrm = glGetUniformLocation(m_program, "texture");
	glUniform1i(unfrm, 0);
	unfrm = glGetUniformLocation(m_program, "shadowMap");
	glUniform1i(unfrm, 1);
	glDetachShader(m_program, vertexShader);
	glDeleteShader(vertexShader);
	glDetachShader(m_program, framgentShader);
	glDeleteShader(framgentShader);
	float def[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	glVertexAttrib4fv(16, def);
}

template<>
void CShaderManager::SetUniformValue<int>(std::string const& uniform, int value) const
{
	int unfrm = glGetUniformLocation(m_program, uniform.c_str());
	glUniform1i(unfrm, value);
}

template<>
void CShaderManager::SetUniformValue<float>(std::string const& uniform, float value) const
{
	int unfrm = glGetUniformLocation(m_program, uniform.c_str());
	glUniform1f(unfrm, value);
}

template<>
void CShaderManager::SetUniformValue<float*>(std::string const& uniform, int count, float* value) const
{
	int unfrm = glGetUniformLocation(m_program, uniform.c_str());
	glUniform1fv(unfrm, count, value);
}

template<>
void CShaderManager::SetUniformValue2<float*>(std::string const& uniform, int count, float* value) const
{
	int unfrm = glGetUniformLocation(m_program, uniform.c_str());
	glUniform2fv(unfrm, count, value);
}

template<>
void CShaderManager::SetUniformValue3<float*>(std::string const& uniform, int count, float* value) const
{
	int unfrm = glGetUniformLocation(m_program, uniform.c_str());
	glUniform3fv(unfrm, count, value);
}

template<>
void CShaderManager::SetUniformValue4<float*>(std::string const& uniform, int count, float* value) const
{
	int unfrm = glGetUniformLocation(m_program, uniform.c_str());
	glUniform4fv(unfrm, count, value);
}

template<>
void CShaderManager::SetUniformValue<double>(std::string const& uniform, double value) const
{
	int unfrm = glGetUniformLocation(m_program, uniform.c_str());
	glUniform1d(unfrm, value);
}

template<>
void CShaderManager::SetUniformValue<unsigned int>(std::string const& uniform, unsigned int value) const
{
	int unfrm = glGetUniformLocation(m_program, uniform.c_str());
	glUniform1ui(unfrm, value);
}

template<>
void CShaderManager::SetUniformMatrix4<float*>(std::string const& uniform, int count, float* value) const
{
	int unfrm = glGetUniformLocation(m_program, uniform.c_str());
	glUniformMatrix4fv(unfrm, count, false, value);
}

template<>
void CShaderManager::SetVertexAttrib4<float*>(unsigned int index, float * value) const
{
	glEnableVertexAttribArray(index);
	glVertexAttribPointer(index, 4, GL_FLOAT, false, 0, value);
}

template<>
void CShaderManager::SetVertexAttrib4<int*>(unsigned int index, int * value) const
{
	glEnableVertexAttribArray(index);
	glVertexAttribIPointer(index, 4, GL_INT, 0, value);
}

template<>
void CShaderManager::SetVertexAttrib4<unsigned int*>(unsigned int index, unsigned int * value) const
{
	glEnableVertexAttribArray(index);
	glVertexAttribIPointer(index, 4, GL_UNSIGNED_INT, 0, value);
}

template<>
void CShaderManager::DisableVertexAttrib4<float*>(unsigned int index, float * defaultValue) const
{
	glDisableVertexAttribArray(index);
	glVertexAttrib4fv(index, defaultValue);
}

template<>
void CShaderManager::DisableVertexAttrib4<int*>(unsigned int index, int * defaultValue) const
{
	glDisableVertexAttribArray(index);
	glVertexAttrib4iv(index, defaultValue);
}

template<>
void CShaderManager::DisableVertexAttrib4<unsigned int*>(unsigned int index, unsigned int * defaultValue) const
{
	glDisableVertexAttribArray(index);
	glVertexAttrib4uiv(index, defaultValue);
}