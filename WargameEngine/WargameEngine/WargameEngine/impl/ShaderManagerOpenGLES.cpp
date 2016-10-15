#include "ShaderManagerOpenGLES.h"
#include <map>
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

class COpenGLESShaderProgram : public IShaderProgram
{
public:
	unsigned program;
};

CShaderManagerOpenGLES::CShaderManagerOpenGLES()
{
	PushProgram(*NewProgram());
}

void CShaderManagerOpenGLES::PushProgram(IShaderProgram const& program) const
{
	m_programs.push_back(reinterpret_cast<COpenGLESShaderProgram const&>(program).program);
	glUseProgram(m_programs.back());
	if(m_onProgramChange) m_onProgramChange();
}

void CShaderManagerOpenGLES::PopProgram() const
{
	m_programs.pop_back();
	glUseProgram(m_programs.back());
	if (m_onProgramChange) m_onProgramChange();
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

std::unique_ptr<IShaderProgram> CShaderManagerOpenGLES::NewProgram(std::wstring const& vertex, std::wstring const& fragment, std::wstring const& geometry)
{
	std::unique_ptr<COpenGLESShaderProgram> program = std::make_unique<COpenGLESShaderProgram>();
	program->program = glCreateProgram();
	GLuint vertexShader(0), framgentShader(0);
	vertexShader = vertex.empty() ? CompileShader(defaultVertexShader, program->program, GL_VERTEX_SHADER) : ComplieShaderFromFile(vertex, program->program, GL_VERTEX_SHADER);
	framgentShader = fragment.empty() ? CompileShader(defaultFragmentShader, program->program, GL_FRAGMENT_SHADER) : ComplieShaderFromFile(fragment, program->program, GL_FRAGMENT_SHADER);
	if(!geometry.empty())
	{
		LogWriter::WriteLine("Geomerty shaders are not supported in openGL ES");
	}
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

	m_positionLocation = glGetAttribLocation(program->program, "Position");
	m_normalsLocation = glGetAttribLocation(program->program, "Normal");
	m_texCoordLocation = glGetAttribLocation(program->program, "TexCoord");
	return std::move(program);
}

void CShaderManagerOpenGLES::SetUniformValue(std::string const& uniform, int count, const float* value) const
{
	int unfrm = glGetUniformLocation(m_programs.back(), uniform.c_str());
	glUniform1fv(unfrm, count, value);
}

void CShaderManagerOpenGLES::SetUniformValue(std::string const& uniform, int count, const int* value) const
{
	int unfrm = glGetUniformLocation(m_programs.back(), uniform.c_str());
	glUniform1iv(unfrm, count, value);
}

void CShaderManagerOpenGLES::SetUniformValue(std::string const& uniform, int count, const unsigned int* value) const
{
	int unfrm = glGetUniformLocation(m_programs.back(), uniform.c_str());
	glUniform1uiv(unfrm, count, value);
}

void CShaderManagerOpenGLES::SetUniformValue2(std::string const& uniform, int count, const float* value) const
{
	int unfrm = glGetUniformLocation(m_programs.back(), uniform.c_str());
	glUniform2fv(unfrm, count, value);
}

void CShaderManagerOpenGLES::SetUniformValue2(std::string const& uniform, int count, const int* value) const
{
	int unfrm = glGetUniformLocation(m_programs.back(), uniform.c_str());
	glUniform2iv(unfrm, count, value);
}

void CShaderManagerOpenGLES::SetUniformValue2(std::string const& uniform, int count, const unsigned int* value) const
{
	int unfrm = glGetUniformLocation(m_programs.back(), uniform.c_str());
	glUniform2uiv(unfrm, count, value);
}

void CShaderManagerOpenGLES::SetUniformValue3(std::string const& uniform, int count, const float* value) const
{
	int unfrm = glGetUniformLocation(m_programs.back(), uniform.c_str());
	glUniform3fv(unfrm, count, value);
}

void CShaderManagerOpenGLES::SetUniformValue3(std::string const& uniform, int count, const int* value) const
{
	int unfrm = glGetUniformLocation(m_programs.back(), uniform.c_str());
	glUniform3iv(unfrm, count, value);
}

void CShaderManagerOpenGLES::SetUniformValue3(std::string const& uniform, int count, const unsigned int* value) const
{
	int unfrm = glGetUniformLocation(m_programs.back(), uniform.c_str());
	glUniform3uiv(unfrm, count, value);
}

void CShaderManagerOpenGLES::SetUniformValue4(std::string const& uniform, int count, const float* value) const
{
	int unfrm = glGetUniformLocation(m_programs.back(), uniform.c_str());
	glUniform4fv(unfrm, count, value);
}

void CShaderManagerOpenGLES::SetUniformValue4(std::string const& uniform, int count, const int* value) const
{
	int unfrm = glGetUniformLocation(m_programs.back(), uniform.c_str());
	glUniform4iv(unfrm, count, value);
}

void CShaderManagerOpenGLES::SetUniformValue4(std::string const& uniform, int count, const unsigned int* value) const
{
	int unfrm = glGetUniformLocation(m_programs.back(), uniform.c_str());
	glUniform4uiv(unfrm, count, value);
}

void CShaderManagerOpenGLES::SetUniformMatrix4(std::string const& uniform, size_t count, float* value) const
{
	int unfrm = glGetUniformLocation(m_programs.back(), uniform.c_str());
	glUniformMatrix4fv(unfrm, count, false, value);
}

void CShaderManagerOpenGLES::SetVertexAttribute(std::string const& attribute, int elementSize, size_t /*totalSize*/, float* values) const
{
	int index = glGetAttribLocation(m_programs.back(), attribute.c_str());
	glEnableVertexAttribArray(index);
	glVertexAttribPointer(index, elementSize, GL_FLOAT, false, 0, values);
}

void CShaderManagerOpenGLES::SetVertexAttribute(std::string const& attribute, int elementSize, size_t /*totalSize*/, int* values) const
{
	int index = glGetAttribLocation(m_programs.back(), attribute.c_str());
	glEnableVertexAttribArray(index);
	glVertexAttribIPointer(index, elementSize, GL_INT, 0, values);
}

void CShaderManagerOpenGLES::SetVertexAttribute(std::string const& attribute, int elementSize, size_t /*totalSize*/, unsigned int* values) const
{
	int index = glGetAttribLocation(m_programs.back(), attribute.c_str());
	glEnableVertexAttribArray(index);
	glVertexAttribIPointer(index, elementSize, GL_UNSIGNED_INT, 0, values);
}

void CShaderManagerOpenGLES::SetPerInstanceVertexAttribute(std::string const& attribute, int elementSize, size_t totalSize, float* values) const
{
	int index = glGetAttribLocation(m_programs.back(), attribute.c_str());
	glEnableVertexAttribArray(index);
	glVertexAttribPointer(index, elementSize, GL_FLOAT, false, 0, values);
	glVertexAttribDivisor(index, 1);
}

void CShaderManagerOpenGLES::DisableVertexAttribute(std::string const& attribute, int /*size*/, float* defaultValue) const
{
	int index = glGetAttribLocation(m_programs.back(), attribute.c_str());
	glDisableVertexAttribArray(index);
	glVertexAttrib4fv(index, defaultValue);
}

void CShaderManagerOpenGLES::DisableVertexAttribute(std::string const& attribute, int /*size*/, int* defaultValue) const
{
	int index = glGetAttribLocation(m_programs.back(), attribute.c_str());
	glDisableVertexAttribArray(index);
	glVertexAttribI4iv(index, defaultValue);
}

void CShaderManagerOpenGLES::DisableVertexAttribute(std::string const& attribute, int /*size*/, unsigned int* defaultValue) const
{
	int index = glGetAttribLocation(m_programs.back(), attribute.c_str());
	glDisableVertexAttribArray(index);
	glVertexAttribI4uiv(index, defaultValue);
}

void CShaderManagerOpenGLES::DoOnProgramChange(std::function<void()> const& handler)
{
	m_onProgramChange = handler;
}
