#include "ShaderManagerOpenGL.h"
#include <map>
#include <GL/glew.h>
#include "gl.h"
#include <fstream>
#include "../LogWriter.h"
#include "../Module.h"

static const std::map<IShaderManager::eVertexAttribute, unsigned int> attributeLocationMap = {
	{ IShaderManager::eVertexAttribute::WEIGHT, 9 },
	{ IShaderManager::eVertexAttribute::WEIGHT_INDEX, 10 }
};

CShaderManagerOpenGL::CShaderManagerOpenGL() :m_program(0)
{
}

void CShaderManagerOpenGL::BindProgram() const
{
	if(m_program != 0)
		glUseProgram(m_program);
}

void CShaderManagerOpenGL::UnBindProgram() const
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

void CShaderManagerOpenGL::NewProgram(std::string const& vertex, std::string const& fragment, std::string const& geometry)
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
			vertexShader = CompileShader(vertex, m_program, GL_VERTEX_SHADER);
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
			framgentShader = CompileShader(fragment, m_program, GL_FRAGMENT_SHADER);
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
			geometryShader = CompileShader(geometry, m_program, GL_GEOMETRY_SHADER);
		}
	}
	glBindAttribLocation(m_program, attributeLocationMap.at(eVertexAttribute::WEIGHT), "weights");
	auto error = glGetError();
	glBindAttribLocation(m_program, attributeLocationMap.at(eVertexAttribute::WEIGHT_INDEX), "weightIndices");
	error = glGetError();
	glLinkProgram(m_program);
	glUseProgram(m_program);
	int unfrm = glGetUniformLocation(m_program, "texture");
	glUniform1i(unfrm, 0);
	unfrm = glGetUniformLocation(m_program, "shadowMap");
	glUniform1i(unfrm, 1);
	unfrm = glGetUniformLocation(m_program, "specular");
	glUniform1i(unfrm, 2);
	unfrm = glGetUniformLocation(m_program, "bump");
	glUniform1i(unfrm, 3);
	glDetachShader(m_program, vertexShader);
	glDeleteShader(vertexShader);
	glDetachShader(m_program, framgentShader);
	glDeleteShader(framgentShader);
	float def[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	glVertexAttrib4fv(attributeLocationMap.at(eVertexAttribute::WEIGHT), def);
}

void CShaderManagerOpenGL::SetUniformValue(std::string const& uniform, int count, const float* value) const
{
	int unfrm = glGetUniformLocation(m_program, uniform.c_str());
	glUniform1fv(unfrm, count, value);
}

void CShaderManagerOpenGL::SetUniformValue(std::string const& uniform, int count, const int* value) const
{
	int unfrm = glGetUniformLocation(m_program, uniform.c_str());
	glUniform1iv(unfrm, count, value);
}

void CShaderManagerOpenGL::SetUniformValue(std::string const& uniform, int count, const unsigned int* value) const
{
	int unfrm = glGetUniformLocation(m_program, uniform.c_str());
	glUniform1uiv(unfrm, count, value);
}

void CShaderManagerOpenGL::SetUniformValue2(std::string const& uniform, int count, const float* value) const
{
	int unfrm = glGetUniformLocation(m_program, uniform.c_str());
	glUniform2fv(unfrm, count, value);
}

void CShaderManagerOpenGL::SetUniformValue2(std::string const& uniform, int count, const int* value) const
{
	int unfrm = glGetUniformLocation(m_program, uniform.c_str());
	glUniform2iv(unfrm, count, value);
}

void CShaderManagerOpenGL::SetUniformValue2(std::string const& uniform, int count, const unsigned int* value) const
{
	int unfrm = glGetUniformLocation(m_program, uniform.c_str());
	glUniform2uiv(unfrm, count, value);
}

void CShaderManagerOpenGL::SetUniformValue3(std::string const& uniform, int count, const float* value) const
{
	int unfrm = glGetUniformLocation(m_program, uniform.c_str());
	glUniform3fv(unfrm, count, value);
}

void CShaderManagerOpenGL::SetUniformValue3(std::string const& uniform, int count, const int* value) const
{
	int unfrm = glGetUniformLocation(m_program, uniform.c_str());
	glUniform3iv(unfrm, count, value);
}

void CShaderManagerOpenGL::SetUniformValue3(std::string const& uniform, int count, const unsigned int* value) const
{
	int unfrm = glGetUniformLocation(m_program, uniform.c_str());
	glUniform3uiv(unfrm, count, value);
}

void CShaderManagerOpenGL::SetUniformValue4(std::string const& uniform, int count, const float* value) const
{
	int unfrm = glGetUniformLocation(m_program, uniform.c_str());
	glUniform4fv(unfrm, count, value);
}

void CShaderManagerOpenGL::SetUniformValue4(std::string const& uniform, int count, const int* value) const
{
	int unfrm = glGetUniformLocation(m_program, uniform.c_str());
	glUniform4iv(unfrm, count, value);
}

void CShaderManagerOpenGL::SetUniformValue4(std::string const& uniform, int count, const unsigned int* value) const
{
	int unfrm = glGetUniformLocation(m_program, uniform.c_str());
	glUniform4uiv(unfrm, count, value);
}

void CShaderManagerOpenGL::SetUniformMatrix4(std::string const& uniform, size_t count, float* value) const
{
	int unfrm = glGetUniformLocation(m_program, uniform.c_str());
	glUniformMatrix4fv(unfrm, count, false, value);
}

void CShaderManagerOpenGL::SetVertexAttribute(eVertexAttribute attributeIndex, int elementSize, size_t /*totalSize*/, float* values) const
{
	glEnableVertexAttribArray(attributeLocationMap.at(attributeIndex));
	glVertexAttribPointer(attributeLocationMap.at(attributeIndex), elementSize, GL_FLOAT, false, 0, values);
}

void CShaderManagerOpenGL::SetVertexAttribute(eVertexAttribute attributeIndex, int elementSize, size_t /*totalSize*/, int* values) const
{
	glEnableVertexAttribArray(attributeLocationMap.at(attributeIndex));
	glVertexAttribIPointer(attributeLocationMap.at(attributeIndex), elementSize, GL_INT, 0, values);
}

void CShaderManagerOpenGL::SetVertexAttribute(eVertexAttribute attributeIndex, int elementSize, size_t /*totalSize*/, unsigned int* values) const
{
	glEnableVertexAttribArray(attributeLocationMap.at(attributeIndex));
	glVertexAttribIPointer(attributeLocationMap.at(attributeIndex), elementSize, GL_UNSIGNED_INT, 0, values);
}

void CShaderManagerOpenGL::DisableVertexAttribute(eVertexAttribute attributeIndex, int /*size*/, float* defaultValue) const
{
	glDisableVertexAttribArray(attributeLocationMap.at(attributeIndex));
	glVertexAttrib4fv(attributeLocationMap.at(attributeIndex), defaultValue);
}

void CShaderManagerOpenGL::DisableVertexAttribute(eVertexAttribute attributeIndex, int /*size*/, int* defaultValue) const
{
	glDisableVertexAttribArray(attributeLocationMap.at(attributeIndex));
	glVertexAttrib4iv(attributeLocationMap.at(attributeIndex), defaultValue);
}

void CShaderManagerOpenGL::DisableVertexAttribute(eVertexAttribute attributeIndex, int /*size*/, unsigned int* defaultValue) const
{
	glDisableVertexAttribArray(attributeLocationMap.at(attributeIndex));
	glVertexAttrib4uiv(attributeLocationMap.at(attributeIndex), defaultValue);
}