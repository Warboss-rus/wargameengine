#include "ShaderManager.h"
#define GLEW_STATIC
#include <GL/glew.h>
#include <GL\glut.h>
#include <fstream>
#include "..\LogWriter.h"

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
		CLogWriter::WriteLine(std::string("Shader error: ") + buffer);
	}
	return shader;
}

void CShaderManager::NewProgram(std::string const& vertex, std::string const& fragment, std::string const& geometry)
{
	if (!GLEW_ARB_shader_objects)
	{
		CLogWriter::WriteLine("Shader objects(GL_ARB_shader_objects) are not supported");
		return;
	}
	m_program = glCreateProgram();
	GLuint vertexShader(0), framgentShader(0);
	if(!vertex.empty())
	{
		if (!GLEW_ARB_vertex_shader)
		{
			CLogWriter::WriteLine("Vertex Shaders(GL_ARB_vertex_shader) are not supported");
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
			CLogWriter::WriteLine("Fragment Shaders(GL_ARB_fragment_shader) are not supported");
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
			CLogWriter::WriteLine("Geometry Shaders(GL_ARB_geometry_shader4) are not supported");
		}
		else
		{
			framgentShader = CompileShader(fragment, m_program, GL_GEOMETRY_SHADER);
		}
	}
	glLinkProgram(m_program);
	glUseProgram(m_program);
	int texture = glGetUniformLocation(m_program, "texture");
	glUniform1i(texture, 0);
	glDeleteShader(vertexShader);
	glDetachShader(m_program, vertexShader);
	glDeleteShader(framgentShader);
	glDetachShader(m_program, framgentShader);
}

template<>
void CShaderManager::SetUniformValue<int>(std::string const& uniform, int value)
{
	int texture = glGetUniformLocation(m_program, "texture");
	glUniform1i(texture, value);
}

template<>
void CShaderManager::SetUniformValue<float>(std::string const& uniform, float value)
{
	int texture = glGetUniformLocation(m_program, "texture");
	glUniform1f(texture, value);
}

template<>
void CShaderManager::SetUniformValue<double>(std::string const& uniform, double value)
{
	int texture = glGetUniformLocation(m_program, "texture");
	glUniform1i(texture, value);
}

template<>
void CShaderManager::SetUniformValue<unsigned int>(std::string const& uniform, unsigned int value)
{
	int texture = glGetUniformLocation(m_program, "texture");
	glUniform1ui(texture, value);
}