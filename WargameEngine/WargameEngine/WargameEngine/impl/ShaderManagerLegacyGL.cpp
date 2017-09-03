#include "ShaderManagerLegacyGL.h"
#include <map>
#include <GL/glew.h>
#include "gl.h"
#include <fstream>
#include "../LogWriter.h"
#include "../Module.h"

using namespace wargameEngine;
using namespace view;

namespace
{

class COpenGLShaderProgram : public IShaderProgram
{
public:
	unsigned int program;
};

class CLegacyGLVertexAttribCache : public IVertexAttribCache
{
public:
	CLegacyGLVertexAttribCache(size_t size, const void* data)
	{
		glGenBuffers(1, &m_cache);
		glBindBuffer(GL_ARRAY_BUFFER, m_cache);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	}
	~CLegacyGLVertexAttribCache()
	{
		glDeleteBuffers(1, &m_cache);
	}
	void Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_cache);
	}
	void UnBind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
private:
	GLuint m_cache;
};

GLuint CompileShader(const std::string& shaderText, GLuint program, GLenum type)
{
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

GLuint CompileShaderFromFile(const Path& path, GLuint program, GLenum type)
{
	std::string shaderText;
	std::string line;
	std::ifstream iFile(path);
	while (std::getline(iFile, line))
	{
		shaderText += line + '\n';
	}
	iFile.close();
	return CompileShader(shaderText, program, type);
}
GLenum FormatToGLEnum(IShaderManager::Format format)
{
	switch (format)
	{
	case IShaderManager::Format::Float32: return GL_FLOAT;
	case IShaderManager::Format::SInt32: return GL_INT;
	case IShaderManager::Format::UInt32: return GL_UNSIGNED_INT;
	default: throw std::runtime_error("Unknown format");
	}
}
}

CShaderManagerLegacyGL::CShaderManagerLegacyGL()
{
	m_programs.push_back(0);
}

void CShaderManagerLegacyGL::PushProgram(IShaderProgram const& program) const
{
	unsigned int p = reinterpret_cast<COpenGLShaderProgram const&>(program).program;
	m_programs.push_back(p);
	glUseProgram(m_programs.back());
}

void CShaderManagerLegacyGL::PopProgram() const
{
	m_programs.pop_back();
	glUseProgram(m_programs.back());
}

std::unique_ptr<IShaderProgram> CShaderManagerLegacyGL::NewProgram(const Path& vertex, const Path& fragment, const Path& geometry)
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
			vertexShader = CompileShaderFromFile(vertex, program->program, GL_VERTEX_SHADER);
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
			framgentShader = CompileShaderFromFile(fragment, program->program, GL_FRAGMENT_SHADER);
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
			geometryShader = CompileShaderFromFile(geometry, program->program, GL_GEOMETRY_SHADER);
		}
	}
	NewProgramImpl(program->program, vertexShader, framgentShader);

	return std::move(program);
}

void CShaderManagerLegacyGL::NewProgramImpl(unsigned program, unsigned vertexShader, unsigned framgentShader)
{
	glBindAttribLocation(program, 9, "weights");
	glBindAttribLocation(program, 10, "weightIndices");
	glLinkProgram(program);
	glUseProgram(program);
	int unfrm = glGetUniformLocation(program, "texture");
	glUniform1i(unfrm, 0);
	unfrm = glGetUniformLocation(program, "shadowMap");
	glUniform1i(unfrm, 1);
	unfrm = glGetUniformLocation(program, "specular");
	glUniform1i(unfrm, 2);
	unfrm = glGetUniformLocation(program, "bump");
	glUniform1i(unfrm, 3);
	glDetachShader(program, vertexShader);
	glDeleteShader(vertexShader);
	glDetachShader(program, framgentShader);
	glDeleteShader(framgentShader);
	float def[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	glVertexAttrib4fv(glGetAttribLocation(program, "weights"), def);
}

std::unique_ptr<IShaderProgram> CShaderManagerLegacyGL::NewProgramSource(std::string const& vertex /* = "" */, std::string const& fragment /* = "" */, std::string const& geometry /* = "" */)
{
	if (!GLEW_ARB_shader_objects)
	{
		LogWriter::WriteLine("Shader objects(GL_ARB_shader_objects) are not supported");
		return nullptr;
	}
	std::unique_ptr<COpenGLShaderProgram> program = std::make_unique<COpenGLShaderProgram>();
	program->program = glCreateProgram();
	GLuint vertexShader(0), framgentShader(0), geometryShader(0);
	if (!vertex.empty())
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
	if (!fragment.empty())
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
	if (!geometry.empty())
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
	NewProgramImpl(program->program, vertexShader, framgentShader);

	return std::move(program);
}

void CShaderManagerLegacyGL::SetUniformValue(std::string const& uniform, int elementSize, size_t count, const float* value) const
{
	int unfrm = glGetUniformLocation(m_programs.back(), uniform.c_str());
	switch (elementSize)
	{
	case 1:
		glUniform1fv(unfrm, static_cast<GLsizei>(count), value);
		break;
	case 2:
		glUniform2fv(unfrm, static_cast<GLsizei>(count), value);
		break;
	case 3:
		glUniform3fv(unfrm, static_cast<GLsizei>(count), value);
		break;
	case 4:
		glUniform4fv(unfrm, static_cast<GLsizei>(count), value);
		break;
	case 16:
		glUniformMatrix4fv(unfrm, static_cast<GLsizei>(count), false, value);
		break;
	default:
		throw std::runtime_error("Unknown elementSize. 1, 2, 3, 4 or 16 expected");
	}
}

void CShaderManagerLegacyGL::SetUniformValue(std::string const& uniform, int elementSize, size_t count, const int* value) const
{
	int unfrm = glGetUniformLocation(m_programs.back(), uniform.c_str());
	switch (elementSize)
	{
	case 1:
		glUniform1iv(unfrm, static_cast<GLsizei>(count), value);
		break;
	case 2:
		glUniform2iv(unfrm, static_cast<GLsizei>(count), value);
		break;
	case 3:
		glUniform3iv(unfrm, static_cast<GLsizei>(count), value);
		break;
	case 4:
		glUniform4iv(unfrm, static_cast<GLsizei>(count), value);
		break;
	default:
		throw std::runtime_error("Unknown elementSize. 1, 2, 3 or 4 expected");
	}
}

void CShaderManagerLegacyGL::SetUniformValue(std::string const& uniform, int elementSize, size_t count, const unsigned int* value) const
{
	int unfrm = glGetUniformLocation(m_programs.back(), uniform.c_str());
	switch (elementSize)
	{
	case 1:
		glUniform1uiv(unfrm, static_cast<GLsizei>(count), value);
		break;
	case 2:
		glUniform2uiv(unfrm, static_cast<GLsizei>(count), value);
		break;
	case 3:
		glUniform3uiv(unfrm, static_cast<GLsizei>(count), value);
		break;
	case 4:
		glUniform4uiv(unfrm, static_cast<GLsizei>(count), value);
		break;
	default:
		throw std::runtime_error("Unknown elementSize. 1, 2, 3 or 4 expected");
	}
}

std::unique_ptr<IVertexAttribCache> CShaderManagerLegacyGL::CreateVertexAttribCache(size_t size, const void* value) const
{
	return std::make_unique<CLegacyGLVertexAttribCache>(size, value);
}

void CShaderManagerLegacyGL::SetVertexAttributeImpl(std::string const& attribute, int elementSize, size_t /*count*/, const void* values, bool perInstance, unsigned int format) const
{
	int index = glGetAttribLocation(m_programs.back(), attribute.c_str());
	glEnableVertexAttribArray(index);
	if(format == GL_FLOAT)
		glVertexAttribPointer(index, elementSize, format, false, 0, values);
	else
		glVertexAttribIPointer(index, elementSize, format, 0, values);
	if (perInstance) glVertexAttribDivisorARB(index, 1);
}

void CShaderManagerLegacyGL::SetVertexAttribute(std::string const& attribute, int elementSize, size_t count, const float* values, bool perInstance) const
{
	SetVertexAttributeImpl(attribute, elementSize, count, values, perInstance, GL_FLOAT);
}

void CShaderManagerLegacyGL::SetVertexAttribute(std::string const& attribute, int elementSize, size_t count, const int* values, bool perInstance) const
{
	SetVertexAttributeImpl(attribute, elementSize, count, values, perInstance, GL_INT);
}

void CShaderManagerLegacyGL::SetVertexAttribute(std::string const& attribute, int elementSize, size_t count, const unsigned int* values, bool perInstance) const
{
	SetVertexAttributeImpl(attribute, elementSize, count, values, perInstance, GL_UNSIGNED_INT);
}

void CShaderManagerLegacyGL::SetVertexAttribute(std::string const& attribute, IVertexAttribCache const& cache, int elementSize, size_t count, Format type, bool perInstance /*= false*/, size_t offset/* = 0*/) const
{
	auto& glCache = reinterpret_cast<CLegacyGLVertexAttribCache const&>(cache);
	glCache.Bind();
	SetVertexAttributeImpl(attribute, elementSize, count, (void*)offset, perInstance, FormatToGLEnum(type));
	glCache.UnBind();
}

bool CShaderManagerLegacyGL::NeedsMVPMatrix() const
{
	return false;
}

void CShaderManagerLegacyGL::SetMatrices(const float*, const float*, const float*, const float*, size_t)
{
	//Not used with legacy GL
}

void CShaderManagerLegacyGL::DisableVertexAttribute(std::string const& attribute, int /*size*/, const float* defaultValue) const
{
	int index = glGetAttribLocation(m_programs.back(), attribute.c_str());
	glDisableVertexAttribArray(index);
	glVertexAttrib4fv(index, defaultValue);
}

void CShaderManagerLegacyGL::DisableVertexAttribute(std::string const& attribute, int /*size*/, const int* defaultValue) const
{
	int index = glGetAttribLocation(m_programs.back(), attribute.c_str());
	glDisableVertexAttribArray(index);
	glVertexAttrib4iv(index, defaultValue);
}

void CShaderManagerLegacyGL::DisableVertexAttribute(std::string const& attribute, int /*size*/, const unsigned int* defaultValue) const
{
	int index = glGetAttribLocation(m_programs.back(), attribute.c_str());
	glDisableVertexAttribArray(index);
	glVertexAttrib4uiv(index, defaultValue);
}