#include "ShaderManagerOpenGLES.h"
#include "../LogWriter.h"
#include "../Module.h"
#include "../Utils.h"
#include "OpenGLESRenderer.h"
#include <GLES3/gl3.h>
#include <fstream>
#include <map>

using namespace wargameEngine;
using namespace view;

namespace
{
constexpr char defaultVertexShader[] = "\
attribute vec4 Position;\n\
attribute vec3 Normal;\n\
attribute vec2 TexCoord;\n\
uniform mat4 mvp_matrix;\n\
varying vec2 v_texcoord;\n\
void main()\n\
{\n\
	gl_Position = mvp_matrix * Position;\n\
	v_texcoord = TexCoord;\n\
}";
constexpr char defaultFragmentShader[] = "\
precision mediump float;\
uniform vec4 color;\n\
uniform sampler2D mainTexture;\n\
varying vec2 v_texcoord;\n\
void main()\n\
{\n\
	vec4 tex = texture2D(mainTexture, v_texcoord);\n\
	gl_FragColor = vec4(tex.xyz + color.xyz, tex.w * color.w);\n\
}";

constexpr char VERTEX_ATTRIB_NAME[] = "Position";
constexpr char NORMAL_ATTRIB_NAME[] = "Normal";
constexpr char TEXCOORD_ATTRIB_NAME[] = "TexCoord";
constexpr char MATERIAL_AMBIENT_KEY[] = "material.ambient";
constexpr char MATERIAL_DIFFUSE_KEY[] = "material.diffuse";
constexpr char MATERIAL_SPECULAR_KEY[] = "material.specular";
constexpr char MATERIAL_SHINENESS_KEY[] = "material.shininess";
class COpenGLESShaderProgram : public IShaderProgram
{
public:
	unsigned int program;
	int vertexAttribLocation = -1;
	int normalAttribLocation = -1;
	int texCoordAttribLocation = -1;
	int materialAmbientLocation = -1;
	int materialDiffuseLocation = -1;
	int materialSpecularLocation = -1;
	int materialShinenessLocation = -1;
};

class COpenGLESVertexAttribCache : public IVertexAttribCache
{
public:
	COpenGLESVertexAttribCache(size_t size, const void* data)
	{
		glGenBuffers(1, &m_cache);
		glBindBuffer(GL_ARRAY_BUFFER, m_cache);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	}
	~COpenGLESVertexAttribCache()
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
	operator GLuint() const
	{
		return m_cache;
	}

private:
	GLuint m_cache;
};
}

CShaderManagerOpenGLES::~CShaderManagerOpenGLES()
{
	for (auto& buf : m_vertexAttribBuffers)
	{
		glDeleteBuffers(1, &buf.second);
	}
	glDeleteBuffers(1, &m_vertexInputBuffer);
}

void CShaderManagerOpenGLES::PushProgram(IShaderProgram const& program) const
{
	m_programs.push_back(&program);
	m_activeProgram = reinterpret_cast<COpenGLESShaderProgram const*>(m_programs.back())->program;
	glUseProgram(m_activeProgram);
	if (m_onProgramChange)
		m_onProgramChange();
}

void CShaderManagerOpenGLES::PopProgram() const
{
	m_programs.pop_back();
	m_activeProgram = reinterpret_cast<COpenGLESShaderProgram const*>(m_programs.back())->program;
	glUseProgram(m_activeProgram);
	if (m_onProgramChange)
		m_onProgramChange();
}

GLuint CompileShader(std::string const& shaderText, GLuint program, GLenum type)
{
	GLuint shader = glCreateShader(type);
	glAttachShader(program, shader);
	GLchar const* text = shaderText.c_str();
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

std::unique_ptr<IShaderProgram> CShaderManagerOpenGLES::NewProgram(const Path& vertex, const Path& fragment, const Path& geometry)
{
	std::unique_ptr<COpenGLESShaderProgram> program = std::make_unique<COpenGLESShaderProgram>();
	program->program = glCreateProgram();
	GLuint vertexShader(0), framgentShader(0);
	vertexShader = vertex.empty() ? CompileShader(defaultVertexShader, program->program, GL_VERTEX_SHADER) : CompileShaderFromFile(vertex, program->program, GL_VERTEX_SHADER);
	framgentShader = fragment.empty() ? CompileShader(defaultFragmentShader, program->program, GL_FRAGMENT_SHADER) : CompileShaderFromFile(fragment, program->program, GL_FRAGMENT_SHADER);
	if (!geometry.empty())
	{
		LogWriter::WriteLine("Geomerty shaders are not supported in openGL ES");
	}
	NewProgramImpl(program->program, vertexShader, framgentShader);
	program->vertexAttribLocation = glGetAttribLocation(program->program, VERTEX_ATTRIB_NAME);
	program->normalAttribLocation = glGetAttribLocation(program->program, NORMAL_ATTRIB_NAME);
	program->texCoordAttribLocation = glGetAttribLocation(program->program, TEXCOORD_ATTRIB_NAME);
	program->materialAmbientLocation = glGetUniformLocation(program->program, MATERIAL_AMBIENT_KEY);
	program->materialDiffuseLocation = glGetUniformLocation(program->program, MATERIAL_DIFFUSE_KEY);
	program->materialSpecularLocation = glGetUniformLocation(program->program, MATERIAL_SPECULAR_KEY);
	program->materialShinenessLocation = glGetUniformLocation(program->program, MATERIAL_SHINENESS_KEY);

	return std::move(program);
}

std::unique_ptr<IShaderProgram> CShaderManagerOpenGLES::NewProgramSource(std::string const& vertex /*= ""*/, std::string const& fragment /*= ""*/, std::string const& geometry /*= ""*/)
{
	std::unique_ptr<COpenGLESShaderProgram> program = std::make_unique<COpenGLESShaderProgram>();
	program->program = glCreateProgram();
	GLuint vertexShader(0), framgentShader(0);
	vertexShader = CompileShader(vertex.empty() ? defaultVertexShader : vertex, program->program, GL_VERTEX_SHADER);
	framgentShader = CompileShader(fragment.empty() ? defaultFragmentShader : fragment, program->program, GL_FRAGMENT_SHADER);
	if (!geometry.empty())
	{
		LogWriter::WriteLine("Geomerty shaders are not supported in openGL ES");
	}
	NewProgramImpl(program->program, vertexShader, framgentShader);

	program->vertexAttribLocation = glGetAttribLocation(program->program, VERTEX_ATTRIB_NAME);
	program->normalAttribLocation = glGetAttribLocation(program->program, NORMAL_ATTRIB_NAME);
	program->texCoordAttribLocation = glGetAttribLocation(program->program, TEXCOORD_ATTRIB_NAME);
	program->materialAmbientLocation = glGetUniformLocation(program->program, MATERIAL_AMBIENT_KEY);
	program->materialDiffuseLocation = glGetUniformLocation(program->program, MATERIAL_DIFFUSE_KEY);
	program->materialSpecularLocation = glGetUniformLocation(program->program, MATERIAL_SPECULAR_KEY);
	program->materialShinenessLocation = glGetUniformLocation(program->program, MATERIAL_SHINENESS_KEY);

	return std::move(program);
}

void CShaderManagerOpenGLES::SetUniformValue(std::string const& uniform, int elementSize, size_t count, const float* value) const
{
	int unfrm = GetUniformLocation(uniform);
	if (unfrm == -1)
		return;
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

void CShaderManagerOpenGLES::SetUniformValue(std::string const& uniform, int elementSize, size_t count, const int* value) const
{
	GLint unfrm = GetUniformLocation(uniform);
	if (unfrm == -1)
		return;
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
		glUniform4iv(unfrm, count, value);
		break;
	default:
		throw std::runtime_error("Unknown elementSize. 1, 2, 3 or 4 expected");
	}
}

void CShaderManagerOpenGLES::SetUniformValue(std::string const& uniform, int elementSize, size_t count, const unsigned int* value) const
{
	GLint unfrm = GetUniformLocation(uniform);
	if (unfrm == -1)
		return;
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

std::unique_ptr<IVertexAttribCache> CShaderManagerOpenGLES::CreateVertexAttribCache(size_t size, const void* value) const
{
	return std::make_unique<COpenGLESVertexAttribCache>(size, value);
}

void CShaderManagerOpenGLES::DoOnProgramChange(std::function<void()> const& handler)
{
	m_onProgramChange = handler;
}

void CShaderManagerOpenGLES::SetInputAttributes(const void* vertices, const void* normals, const void* texCoords, size_t count, size_t vertexComponents)
{
	constexpr size_t normalComponents = 3;
	constexpr size_t texcoordComponents = 2;
	if (!m_vertexInputBuffer)
	{
		glGenBuffers(1, &m_vertexInputBuffer);
	}
	auto& glProgram = reinterpret_cast<const COpenGLESShaderProgram&>(*m_programs.back());
	std::vector<float> data((vertexComponents + (normals ? 3 : 0) + (texCoords ? 2 : 0)) * count);
	memcpy(data.data(), vertices, count * vertexComponents * sizeof(float));
	if (normals)
	{
		memcpy(data.data() + count * vertexComponents, normals, count * normalComponents * sizeof(float));
	}
	if (texCoords)
	{
		memcpy(data.data() + count * vertexComponents + (normals ? count * normalComponents : 0), texCoords, count * texcoordComponents * sizeof(float));
	}
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexInputBuffer);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STREAM_DRAW);
	if (glProgram.vertexAttribLocation != -1)
	{
		glEnableVertexAttribArray(glProgram.vertexAttribLocation);
		glVertexAttribPointer(glProgram.vertexAttribLocation, vertexComponents, GL_FLOAT, false, 0, 0);
	}
	if (glProgram.normalAttribLocation != -1)
	{
		if (normals)
		{
			glEnableVertexAttribArray(glProgram.normalAttribLocation);
			glVertexAttribPointer(glProgram.normalAttribLocation, normalComponents, GL_FLOAT, GL_FALSE, 0, (void*)(count * vertexComponents * sizeof(float)));
		}
		else
		{
			glDisableVertexAttribArray(glProgram.normalAttribLocation);
		}
	}
	if (glProgram.texCoordAttribLocation != -1)
	{
		if (texCoords)
		{
			glEnableVertexAttribArray(glProgram.texCoordAttribLocation);
			glVertexAttribPointer(glProgram.texCoordAttribLocation, texcoordComponents, GL_FLOAT, GL_FALSE, 0, (void*)((count * vertexComponents + (normals ? count * normalComponents : 0)) * sizeof(float)));
		}
		else
		{
			glDisableVertexAttribArray(glProgram.texCoordAttribLocation);
		}
	}
}

void CShaderManagerOpenGLES::SetInputAttributes(const IVertexAttribCache& cache, size_t vertexOffset, size_t normalOffset, size_t texCoordOffset, size_t stride)
{
	constexpr size_t vertexComponents = 3;
	constexpr size_t normalComponents = 3;
	constexpr size_t texcoordComponents = 2;
	auto& glProgram = reinterpret_cast<const COpenGLESShaderProgram&>(*m_programs.back());
	auto& glCache = reinterpret_cast<COpenGLESVertexAttribCache const&>(cache);
	glCache.Bind();
	if (glProgram.vertexAttribLocation != -1)
	{
		glEnableVertexAttribArray(glProgram.vertexAttribLocation);
		glVertexAttribPointer(glProgram.vertexAttribLocation, vertexComponents, GL_FLOAT, GL_FALSE, stride, (void*)vertexOffset);
	}
	if (glProgram.normalAttribLocation != -1)
	{
		if (normalOffset)
		{
			glEnableVertexAttribArray(glProgram.normalAttribLocation);
			glVertexAttribPointer(glProgram.normalAttribLocation, normalComponents, GL_FLOAT, GL_FALSE, stride, (void*)normalOffset);
		}
		else
		{
			glDisableVertexAttribArray(glProgram.normalAttribLocation);
		}
	}
	if (glProgram.texCoordAttribLocation != -1)
	{
		if (texCoordOffset)
		{
			glEnableVertexAttribArray(glProgram.texCoordAttribLocation);
			glVertexAttribPointer(glProgram.texCoordAttribLocation, texcoordComponents, GL_FLOAT, GL_FALSE, stride, (void*)texCoordOffset);
		}
		else
		{
			glDisableVertexAttribArray(glProgram.texCoordAttribLocation);
		}
	}
	glCache.UnBind();
}

void CShaderManagerOpenGLES::SetMaterial(const float* ambient, const float* diffuse, const float* specular, const float shininess)
{
	auto& glProgram = reinterpret_cast<const COpenGLESShaderProgram&>(*m_programs.back());
	glUniform4fv(glProgram.materialAmbientLocation, 1, ambient);
	glUniform4fv(glProgram.materialDiffuseLocation, 1, diffuse);
	glUniform4fv(glProgram.materialSpecularLocation, 1, specular);
	glUniform1f(glProgram.materialShinenessLocation, shininess);
}

void CShaderManagerOpenGLES::SetVertexAttributeImpl(std::string const& attribute, int elementSize, size_t count, const void* values, bool perInstance, unsigned int format) const
{
	auto& programCache = GetProgramCache();
	auto indexIt = programCache.attribLocations.find(attribute);
	if (indexIt == programCache.attribLocations.end())
	{
		programCache.attribLocations.emplace(std::make_pair(attribute, glGetAttribLocation(m_activeProgram, attribute.c_str())));
		programCache.attribState.emplace(std::make_pair(attribute, false));
	}
	int index = programCache.attribLocations[attribute];
	if (index == -1)
		return;
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
	glBufferData(GL_ARRAY_BUFFER, elementSize * count * sizeof(float), values, GL_STREAM_DRAW);

	if (format == GL_FLOAT)
		glVertexAttribPointer(index, elementSize, format, GL_FALSE, 0, NULL);
	else
		glVertexAttribIPointer(index, elementSize, format, 0, NULL);
	glEnableVertexAttribArray(index);
	if (perInstance)
		glVertexAttribDivisor(index, 1);
}

CShaderManagerOpenGLES::ShaderProgramCache& CShaderManagerOpenGLES::GetProgramCache() const
{
	auto it = m_shaderProgramCache.find(m_activeProgram);
	if (it == m_shaderProgramCache.end())
	{
		it = m_shaderProgramCache.emplace(std::make_pair(m_activeProgram, ShaderProgramCache())).first;
	}
	return it->second;
}

int CShaderManagerOpenGLES::GetUniformLocation(std::string const& uniform) const
{
	auto& programCache = GetProgramCache();
	auto it = programCache.uniformLocations.find(uniform);
	if (it == programCache.uniformLocations.end())
	{
		it = programCache.uniformLocations.emplace(std::make_pair(uniform, glGetUniformLocation(m_activeProgram, uniform.c_str()))).first;
	}
	return it->second;
}

void CShaderManagerOpenGLES::SetVertexAttribute(std::string const& attribute, int elementSize, size_t count, const float* values, bool perInstance) const
{
	SetVertexAttributeImpl(attribute, elementSize, count, values, perInstance, GL_FLOAT);
}

void CShaderManagerOpenGLES::SetVertexAttribute(std::string const& attribute, int elementSize, size_t count, const int* values, bool perInstance) const
{
	SetVertexAttributeImpl(attribute, elementSize, count, values, perInstance, GL_INT);
}

void CShaderManagerOpenGLES::SetVertexAttribute(std::string const& attribute, int elementSize, size_t count, const unsigned int* values, bool perInstance) const
{
	SetVertexAttributeImpl(attribute, elementSize, count, values, perInstance, GL_UNSIGNED_INT);
}

void CShaderManagerOpenGLES::SetVertexAttribute(std::string const& attribute, IVertexAttribCache const& cache, int elementSize, size_t count, TYPE type, bool perInstance /*= false*/, size_t offset) const
{
	auto& glCache = reinterpret_cast<COpenGLESVertexAttribCache const&>(cache);
	glCache.Bind();
	int index = glGetAttribLocation(m_activeProgram, attribute.c_str());
	if (index != -1)
	{
		if (type == IShaderManager::TYPE::FLOAT32)
			glVertexAttribPointer(index, elementSize, GL_FLOAT, GL_FALSE, 0, (void*)offset);
		else
			glVertexAttribIPointer(index, elementSize, type == IShaderManager::TYPE::SINT32 ? GL_INT : GL_UNSIGNED_INT, 0, (void*)offset);
		glEnableVertexAttribArray(index);
		if (perInstance)
			glVertexAttribDivisor(index, 1);
	}
	glCache.UnBind();
}

void CShaderManagerOpenGLES::DisableVertexAttribute(std::string const& attribute, int /*size*/, const float* defaultValue) const
{
	int index = glGetAttribLocation(m_activeProgram, attribute.c_str());
	if (index == -1)
		return;
	glDisableVertexAttribArray(index);
	glVertexAttrib4fv(index, defaultValue);
}

void CShaderManagerOpenGLES::DisableVertexAttribute(std::string const& attribute, int /*size*/, const int* defaultValue) const
{
	int index = glGetAttribLocation(m_activeProgram, attribute.c_str());
	if (index == -1)
		return;
	glDisableVertexAttribArray(index);
	glVertexAttribI4iv(index, defaultValue);
}

void CShaderManagerOpenGLES::DisableVertexAttribute(std::string const& attribute, int /*size*/, const unsigned int* defaultValue) const
{
	int index = glGetAttribLocation(m_activeProgram, attribute.c_str());
	if (index == -1)
		return;
	glDisableVertexAttribArray(index);
	glVertexAttribI4uiv(index, defaultValue);
}

void CShaderManagerOpenGLES::NewProgramImpl(unsigned prgm, unsigned vertexShader, unsigned framgentShader)
{
	glLinkProgram(prgm);
	GLint isLinked = 0;
	glGetProgramiv(prgm, GL_LINK_STATUS, &isLinked);
	if (isLinked != GL_TRUE)
	{
		char buffer[1000];
		int size = 0;
		glGetProgramInfoLog(prgm, 1000, &size, buffer);
		LogWriter::WriteLine(std::string("Shader error: ") + buffer);
	}
	glUseProgram(prgm);
	int unfrm = glGetUniformLocation(prgm, "mainTexture");
	glUniform1i(unfrm, 0);
	unfrm = glGetUniformLocation(prgm, "cubemapTexture");
	glUniform1i(unfrm, 0);
	unfrm = glGetUniformLocation(prgm, "shadowMap");
	glUniform1i(unfrm, 1);
	unfrm = glGetUniformLocation(prgm, "specular");
	glUniform1i(unfrm, 2);
	unfrm = glGetUniformLocation(prgm, "bump");
	glUniform1i(unfrm, 3);
	if (vertexShader)
	{
		glDetachShader(prgm, vertexShader);
		glDeleteShader(vertexShader);
	}
	if (framgentShader)
	{
		glDetachShader(prgm, framgentShader);
		glDeleteShader(framgentShader);
	}
	float def[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	unfrm = glGetAttribLocation(prgm, "weights");
	if (unfrm >= 0)
	{
		glVertexAttrib4fv(unfrm, def);
	}
	if (!m_programs.empty())
	{
		glUseProgram(m_activeProgram);
	}
}
