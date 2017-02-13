#include "ShaderManagerOpenGL.h"
#include <map>
#include <GL/glew.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#ifdef _WINDOWS
#include <Windows.h>
#endif
#include <GL/gl.h>
#endif
#include <fstream>
#include "../LogWriter.h"
#include "../Module.h"
#include "../Utils.h"

static const std::string defaultVertexShader = "\
#version 330 core\n\
layout (location = 0) in vec3 Position;\n\
layout (location = 1) in vec3 Normal;\n\
layout (location = 2) in vec2 TexCoord;\n\
uniform mat4 mvp_matrix;\n\
out vec2 v_texcoord;\n\
void main()\n\
{\n\
	gl_Position = mvp_matrix * vec4(Position, 1.0);\n\
	v_texcoord = TexCoord;\n\
}";
static const std::string defaultFragmentShader = "\
#version 330 core\n\
uniform sampler2D mainTexture;\n\
uniform vec4 color;\n\
in vec2 v_texcoord;\n\
out vec4 fragColor;\n\
void main()\n\
{\n\
	vec4 tex = texture(mainTexture, v_texcoord);\n\
	float alpha = tex.w * color.w;\n\
	if(alpha <= 0.01)\n\
		discard;\n\
	fragColor = vec4(tex.xyz + color.xyz, alpha);\n\
}";

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
}

CShaderManagerOpenGL::~CShaderManagerOpenGL()
{
	for (auto& buf : m_vertexAttribBuffers)
	{
		glDeleteBuffers(1, &buf.second);
	}
}

void CShaderManagerOpenGL::PushProgram(IShaderProgram const& program) const
{
	unsigned int p = reinterpret_cast<COpenGLShaderProgram const&>(program).program;
	m_programs.push_back(p);
	m_activeProgram = m_programs.back();
	glUseProgram(m_activeProgram);
	if(m_onProgramChange) m_onProgramChange();
}

void CShaderManagerOpenGL::PopProgram() const
{
	m_programs.pop_back();
	m_activeProgram = m_programs.back();
	glUseProgram(m_activeProgram);
	if (m_onProgramChange) m_onProgramChange();
}

GLuint CompileShader(std::string const& shaderText, GLuint program, GLenum type)
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

GLuint CompileShaderFromFile(std::wstring const& path, GLuint program, GLenum type)
{
	std::string shaderText;
	std::string line;
	std::ifstream iFile;
	OpenFile(iFile, path);
	while (std::getline(iFile, line))
	{
		shaderText += line + '\n';
	}
	iFile.close();
	return CompileShader(shaderText, program, type);
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
	vertexShader = vertex.empty() ? CompileShader(defaultVertexShader, program->program, GL_VERTEX_SHADER) : CompileShaderFromFile(vertex, program->program, GL_VERTEX_SHADER);
	framgentShader = fragment.empty() ? CompileShader(defaultFragmentShader, program->program, GL_FRAGMENT_SHADER): CompileShaderFromFile(fragment, program->program, GL_FRAGMENT_SHADER);
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
	glLinkProgram(program->program);
	GLint isLinked = 0;
	glGetProgramiv(program->program, GL_LINK_STATUS, &isLinked);
	if (isLinked != GL_TRUE)
	{
		char buffer[1000];
		int size = 0;
		glGetProgramInfoLog(program->program, 1000, &size, buffer);
		LogWriter::WriteLine(std::string("Shader error: ") + buffer);
	}
	glUseProgram(program->program);
	int unfrm = glGetUniformLocation(program->program, "mainTexture");
	glUniform1i(unfrm, 0);
	unfrm = glGetUniformLocation(program->program, "cubemapTexture");
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
	if (geometryShader)
	{
		glDetachShader(program->program, geometryShader);
		glDeleteShader(geometryShader);
	}
	float def[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	glVertexAttrib4fv(glGetAttribLocation(program->program, "weights"), def);
	if (!m_programs.empty())
	{
		glUseProgram(m_activeProgram);
	}

	return std::move(program);
}

void CShaderManagerOpenGL::SetUniformValue(std::string const& uniform, int elementSize, size_t count, const float* value) const
{
	int unfrm = GetUniformLocation(uniform);
	if (unfrm == -1) return;
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

void CShaderManagerOpenGL::SetUniformValue(std::string const& uniform, int elementSize, size_t count, const int* value) const
{
	GLint unfrm = GetUniformLocation(uniform);
	if (unfrm == -1) return;
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

void CShaderManagerOpenGL::SetUniformValue(std::string const& uniform, int elementSize, size_t count, const unsigned int* value) const
{
	GLint unfrm = GetUniformLocation(uniform);
	if (unfrm == -1) return;
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

void CShaderManagerOpenGL::DoOnProgramChange(std::function<void()> const& handler)
{
	m_onProgramChange = handler;
}

void CShaderManagerOpenGL::SetVertexAttributeImpl(std::string const& attribute, int elementSize, size_t count, const void* values, bool perInstance, unsigned int format) const
{
	auto& programCache = GetProgramCache();
	auto indexIt = programCache.attribLocations.find(attribute);
	if (indexIt == programCache.attribLocations.end())
	{
		programCache.attribLocations.emplace(std::make_pair(attribute, glGetAttribLocation(m_activeProgram, attribute.c_str())));
		programCache.attribState.emplace(std::make_pair(attribute, false));
	}
	int index = programCache.attribLocations[attribute];
	if (index == -1) return;
	if (!values)
	{
		glDisableVertexAttribArray(index);
		return;
	}
	if (m_vertexAttribBuffers.find(attribute) == m_vertexAttribBuffers.end())
	{
		unsigned int buffer;
		glGenBuffers(1, &buffer);
		m_vertexAttribBuffers.emplace(std::make_pair(attribute, buffer));
	}
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexAttribBuffers.at(attribute));
	glBufferData(GL_ARRAY_BUFFER, elementSize * count * sizeof(float), values, GL_DYNAMIC_DRAW);

	if(format == GL_FLOAT)
		glVertexAttribPointer(index, elementSize, format, GL_FALSE, 0, NULL);
	else
		glVertexAttribIPointer(index, elementSize, format, 0, NULL);
	glEnableVertexAttribArray(index);
	if (perInstance) glVertexAttribDivisorARB(index, 1);
}

CShaderManagerOpenGL::ShaderProgramCache& CShaderManagerOpenGL::GetProgramCache() const
{
	auto it = m_shaderProgramCache.find(m_activeProgram);
	if (it == m_shaderProgramCache.end())
	{
		it = m_shaderProgramCache.emplace(std::make_pair(m_activeProgram, ShaderProgramCache())).first;
	}
	return it->second;
}

int CShaderManagerOpenGL::GetUniformLocation(std::string const& uniform) const
{
	auto& programCache = GetProgramCache();
	auto it = programCache.uniformLocations.find(uniform);
	if (it == programCache.uniformLocations.end())
	{
		it = programCache.uniformLocations.emplace(std::make_pair(uniform, glGetUniformLocation(m_activeProgram, uniform.c_str()))).first;
	}
	return it->second;
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

void CShaderManagerOpenGL::SetVertexAttribute(std::string const& attribute, IVertexAttribCache const& cache, bool perInstance /*= false*/, size_t offset/* = 0*/) const
{
	auto& glCache = reinterpret_cast<COpenGLVertexAttribCache const&>(cache);
	glCache.Bind();
	int index = glGetAttribLocation(m_activeProgram, attribute.c_str());
	if (index != -1)
	{
		if (glCache.GetFormat() == GL_FLOAT)
			glVertexAttribPointer(index, glCache.GetElementSize(), glCache.GetFormat(), GL_FALSE, 0, (void*)offset);
		else
			glVertexAttribIPointer(index, glCache.GetElementSize(), glCache.GetFormat(), 0, (void*)offset);
		glEnableVertexAttribArray(index);
		if (perInstance) glVertexAttribDivisorARB(index, 1);
	}
	glCache.UnBind();
}

void CShaderManagerOpenGL::DisableVertexAttribute(std::string const& attribute, int /*size*/, const float* defaultValue) const
{
	int index = glGetAttribLocation(m_activeProgram, attribute.c_str());
	if (index == -1) return;
	glDisableVertexAttribArray(index);
	glVertexAttrib4fv(index, defaultValue);
}

void CShaderManagerOpenGL::DisableVertexAttribute(std::string const& attribute, int /*size*/, const int* defaultValue) const
{
	int index = glGetAttribLocation(m_activeProgram, attribute.c_str());
	if (index == -1) return;
	glDisableVertexAttribArray(index);
	glVertexAttribI4iv(index, defaultValue);
}

void CShaderManagerOpenGL::DisableVertexAttribute(std::string const& attribute, int /*size*/, const unsigned int* defaultValue) const
{
	int index = glGetAttribLocation(m_activeProgram, attribute.c_str());
	if (index == -1) return;
	glDisableVertexAttribArray(index);
	glVertexAttribI4uiv(index, defaultValue);
}