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

class COpenGLVertexAttribCache : public IVertexAttribCache
{
public:
	COpenGLVertexAttribCache(int elementSize, size_t count, const void* data, GLenum format)
		:m_elementSize(elementSize)
		,m_format(format)
	{
		glGenBuffers(1, &m_cache);
		glBindBuffer(GL_ARRAY_BUFFER, m_cache);
		glBufferData(GL_ARRAY_BUFFER, count * elementSize * sizeof(float), data, GL_STATIC_DRAW);
	}
	~COpenGLVertexAttribCache()
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
	int GetElementSize() const { return m_elementSize; }
	GLenum GetFormat() const { return m_format; }
private:
	GLuint m_cache;
	const int m_elementSize;
	const GLenum m_format;
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

void CShaderManagerOpenGL::SetUniformValue(std::string const& uniform, int elementSize, size_t count, const float* value) const
{
	int unfrm = glGetUniformLocation(m_programs.back(), uniform.c_str());
	switch (elementSize)
	{
	case 1:
		glUniform1fv(unfrm, count, value);
		break;
	case 2:
		glUniform2fv(unfrm, count, value);
		break;
	case 3:
		glUniform3fv(unfrm, count, value);
		break;
	case 4:
		glUniform4fv(unfrm, count, value);
		break;
	case 16:
		glUniformMatrix4fv(unfrm, count, false, value);
		break;
	default:
		throw std::runtime_error("Unknown elementSize. 1, 2, 3, 4 or 16 expected");
	}
}

void CShaderManagerOpenGL::SetUniformValue(std::string const& uniform, int elementSize, size_t count, const int* value) const
{
	int unfrm = glGetUniformLocation(m_programs.back(), uniform.c_str());
	switch (elementSize)
	{
	case 1:
		glUniform1iv(unfrm, count, value);
		break;
	case 2:
		glUniform2iv(unfrm, count, value);
		break;
	case 3:
		glUniform3iv(unfrm, count, value);
		break;
	case 4:
		glUniform4iv(unfrm, count, value);
		break;
	default:
		throw std::runtime_error("Unknown elementSize. 1, 2, 3 or 4 expected");
	}
}

void CShaderManagerOpenGL::SetUniformValue(std::string const& uniform, int elementSize, size_t count, const unsigned int* value) const
{
	int unfrm = glGetUniformLocation(m_programs.back(), uniform.c_str());
	switch (elementSize)
	{
	case 1:
		glUniform1uiv(unfrm, count, value);
		break;
	case 2:
		glUniform2uiv(unfrm, count, value);
		break;
	case 3:
		glUniform3uiv(unfrm, count, value);
		break;
	case 4:
		glUniform4uiv(unfrm, count, value);
		break;
	default:
		throw std::runtime_error("Unknown elementSize. 1, 2, 3 or 4 expected");
	}
}

std::unique_ptr<IVertexAttribCache> CShaderManagerOpenGL::CreateVertexAttribCache(int elementSize, size_t count, const float* value) const
{
	return std::make_unique<COpenGLVertexAttribCache>(elementSize, count, value, GL_FLOAT);
}

std::unique_ptr<IVertexAttribCache> CShaderManagerOpenGL::CreateVertexAttribCache(int elementSize, size_t count, const int* value) const
{
	return std::make_unique<COpenGLVertexAttribCache>(elementSize, count, value, GL_INT);
}

std::unique_ptr<IVertexAttribCache> CShaderManagerOpenGL::CreateVertexAttribCache(int elementSize, size_t count, const unsigned int* value) const
{
	return std::make_unique<COpenGLVertexAttribCache>(elementSize, count, value, GL_UNSIGNED_INT);
}

void CShaderManagerOpenGL::SetVertexAttributeImpl(std::string const& attribute, int elementSize, size_t /*count*/, const void* values, bool perInstance, unsigned int format) const
{
	int index = glGetAttribLocation(m_programs.back(), attribute.c_str());
	glEnableVertexAttribArray(index);
	if(format == GL_FLOAT)
		glVertexAttribPointer(index, elementSize, format, false, 0, values);
	else
		glVertexAttribIPointer(index, elementSize, format, 0, values);
	if (perInstance) glVertexAttribDivisorARB(index, 1);
}

void CShaderManagerOpenGL::SetVertexAttribute(std::string const& attribute, int elementSize, size_t count, const float* values, bool perInstance) const
{
	SetVertexAttributeImpl(attribute, elementSize, count, values, perInstance, GL_FLOAT);
}

void CShaderManagerOpenGL::SetVertexAttribute(std::string const& attribute, int elementSize, size_t count, const int* values, bool perInstance) const
{
	SetVertexAttributeImpl(attribute, elementSize, count, values, perInstance, GL_INT);
}

void CShaderManagerOpenGL::SetVertexAttribute(std::string const& attribute, int elementSize, size_t count, const unsigned int* values, bool perInstance) const
{
	SetVertexAttributeImpl(attribute, elementSize, count, values, perInstance, GL_UNSIGNED_INT);
}

void CShaderManagerOpenGL::SetVertexAttribute(std::string const& attribute, IVertexAttribCache const& cache, bool perInstance /*= false*/) const
{
	auto& glCache = reinterpret_cast<COpenGLVertexAttribCache const&>(cache);
	glCache.Bind();
	SetVertexAttributeImpl(attribute, glCache.GetElementSize(), 0, NULL, perInstance, glCache.GetFormat());
	glCache.UnBind();
}

void CShaderManagerOpenGL::DisableVertexAttribute(std::string const& attribute, int /*size*/, const float* defaultValue) const
{
	int index = glGetAttribLocation(m_programs.back(), attribute.c_str());
	glDisableVertexAttribArray(index);
	glVertexAttrib4fv(index, defaultValue);
}

void CShaderManagerOpenGL::DisableVertexAttribute(std::string const& attribute, int /*size*/, const int* defaultValue) const
{
	int index = glGetAttribLocation(m_programs.back(), attribute.c_str());
	glDisableVertexAttribArray(index);
	glVertexAttrib4iv(index, defaultValue);
}

void CShaderManagerOpenGL::DisableVertexAttribute(std::string const& attribute, int /*size*/, const unsigned int* defaultValue) const
{
	int index = glGetAttribLocation(m_programs.back(), attribute.c_str());
	glDisableVertexAttribArray(index);
	glVertexAttrib4uiv(index, defaultValue);
}