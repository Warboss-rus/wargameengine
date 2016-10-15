#include "ShaderManagerOpenGL.h"
#include <map>
#include <GL/glew.h>
#include "gl.h"
#include <fstream>
#include "../LogWriter.h"
#include "../Module.h"

class COpenGLShaderProgram : public IShaderProgram
{
public:
	unsigned int program;
};

CShaderManagerOpenGL::CShaderManagerOpenGL()
{
	m_programs.push_back(0);
}

void CShaderManagerOpenGL::PushProgram(IShaderProgram const& program) const
{
	unsigned int p = reinterpret_cast<COpenGLShaderProgram const&>(program).program;
	m_programs.push_back(p);
	glUseProgram(m_programs.back());
}

void CShaderManagerOpenGL::PopProgram() const
{
	m_programs.pop_back();
	glUseProgram(m_programs.back());
}

GLuint CompileShader(std::wstring const& path, GLuint program, GLenum type)
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

std::unique_ptr<IShaderProgram> CShaderManagerOpenGL::NewProgram(std::wstring const& vertex, std::wstring const& fragment, std::wstring const& geometry)
{
	if (!GLEW_ARB_shader_objects)
	{
		LogWriter::WriteLine("Shader objects(GL_ARB_shader_objects) are not supported");
		return nullptr;
	}
	std::unique_ptr<COpenGLShaderProgram> program = std::make_unique<COpenGLShaderProgram>();
	program->program = glCreateProgram();
	GLuint vertexShader(0), framgentShader(0), geometryShader(0);
	if(!vertex.empty())
	{
		if (!GLEW_ARB_vertex_shader)
		{
			LogWriter::WriteLine("Vertex Shaders(GL_ARB_vertex_shader) are not supported");
		}
		else
		{
			vertexShader = CompileShader(vertex, program->program, GL_VERTEX_SHADER);
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
			framgentShader = CompileShader(fragment, program->program, GL_FRAGMENT_SHADER);
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
			geometryShader = CompileShader(geometry, program->program, GL_GEOMETRY_SHADER);
		}
	}
	glBindAttribLocation(program->program, 9, "weights");
	glBindAttribLocation(program->program, 10, "weightIndices");
	glLinkProgram(program->program);
	glUseProgram(program->program);
	int unfrm = glGetUniformLocation(program->program, "texture");
	glUniform1i(unfrm, 0);
	unfrm = glGetUniformLocation(program->program, "shadowMap");
	glUniform1i(unfrm, 1);
	unfrm = glGetUniformLocation(program->program, "specular");
	glUniform1i(unfrm, 2);
	unfrm = glGetUniformLocation(program->program, "bump");
	glUniform1i(unfrm, 3);
	glDetachShader(program->program, vertexShader);
	glDeleteShader(vertexShader);
	glDetachShader(program->program, framgentShader);
	glDeleteShader(framgentShader);
	float def[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	glVertexAttrib4fv(glGetAttribLocation(program->program, "weights"), def);
	return std::move(program);
}

void CShaderManagerOpenGL::SetUniformValue(std::string const& uniform, int count, const float* value) const
{
	int unfrm = glGetUniformLocation(m_programs.back(), uniform.c_str());
	glUniform1fv(unfrm, count, value);
}

void CShaderManagerOpenGL::SetUniformValue(std::string const& uniform, int count, const int* value) const
{
	int unfrm = glGetUniformLocation(m_programs.back(), uniform.c_str());
	glUniform1iv(unfrm, count, value);
}

void CShaderManagerOpenGL::SetUniformValue(std::string const& uniform, int count, const unsigned int* value) const
{
	int unfrm = glGetUniformLocation(m_programs.back(), uniform.c_str());
	glUniform1uiv(unfrm, count, value);
}

void CShaderManagerOpenGL::SetUniformValue2(std::string const& uniform, int count, const float* value) const
{
	int unfrm = glGetUniformLocation(m_programs.back(), uniform.c_str());
	glUniform2fv(unfrm, count, value);
}

void CShaderManagerOpenGL::SetUniformValue2(std::string const& uniform, int count, const int* value) const
{
	int unfrm = glGetUniformLocation(m_programs.back(), uniform.c_str());
	glUniform2iv(unfrm, count, value);
}

void CShaderManagerOpenGL::SetUniformValue2(std::string const& uniform, int count, const unsigned int* value) const
{
	int unfrm = glGetUniformLocation(m_programs.back(), uniform.c_str());
	glUniform2uiv(unfrm, count, value);
}

void CShaderManagerOpenGL::SetUniformValue3(std::string const& uniform, int count, const float* value) const
{
	int unfrm = glGetUniformLocation(m_programs.back(), uniform.c_str());
	glUniform3fv(unfrm, count, value);
}

void CShaderManagerOpenGL::SetUniformValue3(std::string const& uniform, int count, const int* value) const
{
	int unfrm = glGetUniformLocation(m_programs.back(), uniform.c_str());
	glUniform3iv(unfrm, count, value);
}

void CShaderManagerOpenGL::SetUniformValue3(std::string const& uniform, int count, const unsigned int* value) const
{
	int unfrm = glGetUniformLocation(m_programs.back(), uniform.c_str());
	glUniform3uiv(unfrm, count, value);
}

void CShaderManagerOpenGL::SetUniformValue4(std::string const& uniform, int count, const float* value) const
{
	int unfrm = glGetUniformLocation(m_programs.back(), uniform.c_str());
	glUniform4fv(unfrm, count, value);
}

void CShaderManagerOpenGL::SetUniformValue4(std::string const& uniform, int count, const int* value) const
{
	int unfrm = glGetUniformLocation(m_programs.back(), uniform.c_str());
	glUniform4iv(unfrm, count, value);
}

void CShaderManagerOpenGL::SetUniformValue4(std::string const& uniform, int count, const unsigned int* value) const
{
	int unfrm = glGetUniformLocation(m_programs.back(), uniform.c_str());
	glUniform4uiv(unfrm, count, value);
}

void CShaderManagerOpenGL::SetUniformMatrix4(std::string const& uniform, size_t count, float* value) const
{
	int unfrm = glGetUniformLocation(m_programs.back(), uniform.c_str());
	glUniformMatrix4fv(unfrm, count, false, value);
}

void CShaderManagerOpenGL::SetVertexAttribute(std::string const& attribute, int elementSize, size_t /*totalSize*/, float* values) const
{
	int index = glGetAttribLocation(m_programs.back(), attribute.c_str());
	glEnableVertexAttribArray(index);
	glVertexAttribPointer(index, elementSize, GL_FLOAT, false, 0, values);
}

void CShaderManagerOpenGL::SetVertexAttribute(std::string const& attribute, int elementSize, size_t /*totalSize*/, int* values) const
{
	int index = glGetAttribLocation(m_programs.back(), attribute.c_str());
	glEnableVertexAttribArray(index);
	glVertexAttribIPointer(index, elementSize, GL_INT, 0, values);
}

void CShaderManagerOpenGL::SetVertexAttribute(std::string const& attribute, int elementSize, size_t /*totalSize*/, unsigned int* values) const
{
	int index = glGetAttribLocation(m_programs.back(), attribute.c_str());
	glEnableVertexAttribArray(index);
	glVertexAttribIPointer(index, elementSize, GL_UNSIGNED_INT, 0, values);
}

void CShaderManagerOpenGL::SetPerInstanceVertexAttribute(std::string const& attribute, int elementSize, size_t /*totalSize*/, float* values) const
{
	int index = glGetAttribLocation(m_programs.back(), attribute.c_str());
	glEnableVertexAttribArray(index);
	glVertexAttribPointer(index, elementSize, GL_FLOAT, false, 0, values);
	glVertexAttribDivisorARB(index, 1);
}

void CShaderManagerOpenGL::DisableVertexAttribute(std::string const& attribute, int /*size*/, float* defaultValue) const
{
	int index = glGetAttribLocation(m_programs.back(), attribute.c_str());
	glDisableVertexAttribArray(index);
	glVertexAttrib4fv(index, defaultValue);
}

void CShaderManagerOpenGL::DisableVertexAttribute(std::string const& attribute, int /*size*/, int* defaultValue) const
{
	int index = glGetAttribLocation(m_programs.back(), attribute.c_str());
	glDisableVertexAttribArray(index);
	glVertexAttrib4iv(index, defaultValue);
}

void CShaderManagerOpenGL::DisableVertexAttribute(std::string const& attribute, int /*size*/, unsigned int* defaultValue) const
{
	int index = glGetAttribLocation(m_programs.back(), attribute.c_str());
	glDisableVertexAttribArray(index);
	glVertexAttrib4uiv(index, defaultValue);
}