#include "ShaderManagerOpenGLES.h"
#include <map>
#include <GLES2/gl2.h>
#include <GLES3/gl3.h>
#include <fstream>
#include "../LogWriter.h"
#include "../Module.h"
#include "../Utils.h"
#include "OpenGLESRenderer.h"

static const std::string defaultVertexShader = "\
attribute vec4 Position;\n\
attribute vec3 Normal;\n\
attribute vec2 TexCoord;\n\
uniform mat4 mvp_matrix;\n\
uniform vec3 color;\n\
varying vec2 v_texcoord;\n\
void main()\n\
{\n\
	gl_Position = mvp_matrix * Position;\n\
	v_texcoord = TexCoord;\n\
}";
static const std::string defaultFragmentShader = "\
precision mediump float;\
uniform mat4 mvp_matrix;\n\
uniform vec4 color;\n\
uniform sampler2D texture;\n\
varying vec2 v_texcoord;\n\
void main()\n\
{\n\
	gl_FragColor = texture2D(texture, v_texcoord);\n\
}";

static const std::map<IShaderManager::eVertexAttribute, unsigned int> attributeLocationMap = {
	{ IShaderManager::eVertexAttribute::WEIGHT, 9 },
	{ IShaderManager::eVertexAttribute::WEIGHT_INDEX, 10 }
};

CShaderManagerOpenGLES::CShaderManagerOpenGLES(const COpenGLESRenderer * renderer) :m_program(0), m_defaultProgram(0), m_renderer(renderer)
{
}

void CShaderManagerOpenGLES::BindProgram() const
{
	if (m_customProgram != 0)
	{
		m_program = m_customProgram;
	}
	glUseProgram(m_program);
	m_renderer->BindShaderManager(this);
}

void CShaderManagerOpenGLES::UnBindProgram() const
{
	if (!m_defaultProgram)
	{
		auto nonConstThis = const_cast<CShaderManagerOpenGLES*>(this);
		nonConstThis->NewProgram();
		nonConstThis->m_defaultProgram = m_program;
	}
	glUseProgram(m_program);
	m_renderer->BindShaderManager(this);
}

GLuint CompileShader(std::string const& shaderText, GLuint program, GLenum type)
{
	GLuint shader = glCreateShader(type);
	glAttachShader(program, shader);
	GLchar const * text = shaderText.c_str();
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

GLuint ComplieShaderFromFile(std::wstring const& path, GLuint program, GLenum type)
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

void CShaderManagerOpenGLES::NewProgram(std::wstring const& vertex, std::wstring const& fragment, std::wstring const& geometry)
{
	m_program = glCreateProgram();
	GLuint vertexShader(0), framgentShader(0);
	vertexShader = vertex.empty() ? CompileShader(defaultVertexShader, m_program, GL_VERTEX_SHADER) : ComplieShaderFromFile(vertex, m_program, GL_VERTEX_SHADER);
	framgentShader = fragment.empty() ? CompileShader(defaultFragmentShader, m_program, GL_FRAGMENT_SHADER) : ComplieShaderFromFile(fragment, m_program, GL_FRAGMENT_SHADER);
	if(!geometry.empty())
	{
		LogWriter::WriteLine("Geomerty shaders are not supported in openGL ES");
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

	m_positionLocation = glGetAttribLocation(m_program, "Position");
	m_normalsLocation = glGetAttribLocation(m_program, "Normal");
	m_texCoordLocation = glGetAttribLocation(m_program, "TexCoord");
}

void CShaderManagerOpenGLES::SetUniformValue(std::string const& uniform, int count, const float* value) const
{
	int unfrm = glGetUniformLocation(m_program, uniform.c_str());
	glUniform1fv(unfrm, count, value);
}

void CShaderManagerOpenGLES::SetUniformValue(std::string const& uniform, int count, const int* value) const
{
	int unfrm = glGetUniformLocation(m_program, uniform.c_str());
	glUniform1iv(unfrm, count, value);
}

void CShaderManagerOpenGLES::SetUniformValue(std::string const& uniform, int count, const unsigned int* value) const
{
	int unfrm = glGetUniformLocation(m_program, uniform.c_str());
	glUniform1uiv(unfrm, count, value);
}

void CShaderManagerOpenGLES::SetUniformValue2(std::string const& uniform, int count, const float* value) const
{
	int unfrm = glGetUniformLocation(m_program, uniform.c_str());
	glUniform2fv(unfrm, count, value);
}

void CShaderManagerOpenGLES::SetUniformValue2(std::string const& uniform, int count, const int* value) const
{
	int unfrm = glGetUniformLocation(m_program, uniform.c_str());
	glUniform2iv(unfrm, count, value);
}

void CShaderManagerOpenGLES::SetUniformValue2(std::string const& uniform, int count, const unsigned int* value) const
{
	int unfrm = glGetUniformLocation(m_program, uniform.c_str());
	glUniform2uiv(unfrm, count, value);
}

void CShaderManagerOpenGLES::SetUniformValue3(std::string const& uniform, int count, const float* value) const
{
	int unfrm = glGetUniformLocation(m_program, uniform.c_str());
	glUniform3fv(unfrm, count, value);
}

void CShaderManagerOpenGLES::SetUniformValue3(std::string const& uniform, int count, const int* value) const
{
	int unfrm = glGetUniformLocation(m_program, uniform.c_str());
	glUniform3iv(unfrm, count, value);
}

void CShaderManagerOpenGLES::SetUniformValue3(std::string const& uniform, int count, const unsigned int* value) const
{
	int unfrm = glGetUniformLocation(m_program, uniform.c_str());
	glUniform3uiv(unfrm, count, value);
}

void CShaderManagerOpenGLES::SetUniformValue4(std::string const& uniform, int count, const float* value) const
{
	int unfrm = glGetUniformLocation(m_program, uniform.c_str());
	glUniform4fv(unfrm, count, value);
}

void CShaderManagerOpenGLES::SetUniformValue4(std::string const& uniform, int count, const int* value) const
{
	int unfrm = glGetUniformLocation(m_program, uniform.c_str());
	glUniform4iv(unfrm, count, value);
}

void CShaderManagerOpenGLES::SetUniformValue4(std::string const& uniform, int count, const unsigned int* value) const
{
	int unfrm = glGetUniformLocation(m_program, uniform.c_str());
	glUniform4uiv(unfrm, count, value);
}

void CShaderManagerOpenGLES::SetUniformMatrix4(std::string const& uniform, size_t count, float* value) const
{
	int unfrm = glGetUniformLocation(m_program, uniform.c_str());
	glUniformMatrix4fv(unfrm, count, false, value);
}

void CShaderManagerOpenGLES::SetVertexAttribute(eVertexAttribute attributeIndex, int elementSize, size_t /*totalSize*/, float* values) const
{
	glEnableVertexAttribArray(attributeLocationMap.at(attributeIndex));
	glVertexAttribPointer(attributeLocationMap.at(attributeIndex), elementSize, GL_FLOAT, false, 0, values);
}

void CShaderManagerOpenGLES::SetVertexAttribute(eVertexAttribute attributeIndex, int elementSize, size_t /*totalSize*/, int* values) const
{
	glEnableVertexAttribArray(attributeLocationMap.at(attributeIndex));
	glVertexAttribIPointer(attributeLocationMap.at(attributeIndex), elementSize, GL_INT, 0, values);
}

void CShaderManagerOpenGLES::SetVertexAttribute(eVertexAttribute attributeIndex, int elementSize, size_t /*totalSize*/, unsigned int* values) const
{
	glEnableVertexAttribArray(attributeLocationMap.at(attributeIndex));
	glVertexAttribIPointer(attributeLocationMap.at(attributeIndex), elementSize, GL_UNSIGNED_INT, 0, values);
}

void CShaderManagerOpenGLES::DisableVertexAttribute(eVertexAttribute attributeIndex, int /*size*/, float* defaultValue) const
{
	glDisableVertexAttribArray(attributeLocationMap.at(attributeIndex));
	glVertexAttrib4fv(attributeLocationMap.at(attributeIndex), defaultValue);
}

void CShaderManagerOpenGLES::DisableVertexAttribute(eVertexAttribute attributeIndex, int /*size*/, int* defaultValue) const
{
	glDisableVertexAttribArray(attributeLocationMap.at(attributeIndex));
	glVertexAttribI4iv(attributeLocationMap.at(attributeIndex), defaultValue);
}

void CShaderManagerOpenGLES::DisableVertexAttribute(eVertexAttribute attributeIndex, int /*size*/, unsigned int* defaultValue) const
{
	glDisableVertexAttribArray(attributeLocationMap.at(attributeIndex));
	glVertexAttribI4uiv(attributeLocationMap.at(attributeIndex), defaultValue);
}