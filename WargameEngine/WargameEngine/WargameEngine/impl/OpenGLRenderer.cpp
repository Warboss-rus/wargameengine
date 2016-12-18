#include "OpenGLRenderer.h"
#include <GL/glew.h>
#include "gl.h"
#include "../LogWriter.h"
#include "../view/TextureManager.h"
#include "ShaderManagerOpenGL.h"
#include "../view/Matrix4.h"
#include "../view/IViewport.h"
#include <gtc/matrix_transform.inl>
#pragma warning(push)
#pragma warning(disable: 4201)
#include <gtc/type_ptr.hpp>
#pragma warning(pop)
#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

namespace
{
static const int positionIndex = 0;
static const int normalIndex = 2;
static const int texCoordIndex = 1;

glm::vec3 FromVector3(CVector3f const& v)
{
	glm::vec3 result;
	memcpy(&result.x, &v.x, sizeof(float) * 3);
	return result;
}

glm::mat4 FromMatrix(Matrix4F const& m)
{
	glm::mat4 result;
	memcpy(&result, &m, sizeof(float) * 16);
	return result;
}
}

class COpenGLDrawingList : public IDrawingList
{
public:
	COpenGLDrawingList(unsigned int id);
	~COpenGLDrawingList();

	virtual void Draw() const override;
private:
	unsigned int m_id;
};

class COpenGLVertexBuffer : public IVertexBuffer
{
public:
	COpenGLVertexBuffer(const float * vertex = nullptr, const float * normals = nullptr, const float * texcoords = nullptr, size_t size = 0, bool temp = true, GLuint mainVAO = 0);
	~COpenGLVertexBuffer();
	virtual void Bind() const override;
	virtual void SetIndexBuffer(unsigned int * indexPtr, size_t indexesSize) override;
	virtual void DrawIndexes(size_t begin, size_t count) override;
	virtual void DrawAll(size_t count) override;
	virtual void DrawInstanced(size_t size, size_t instanceCount) override;
	virtual void UnBind() const override;
private:
	void CreateVBO(size_t size, size_t components, const void* data, GLuint program, const char* attribName);
	GLuint m_vao = 0;
	GLuint m_mainVAO = 0;
	std::vector<GLuint> m_buffers;
	GLuint m_indexesBuffer = NULL;
};

class COpenGLFrameBuffer : public IFrameBuffer
{
public:
	COpenGLFrameBuffer();
	~COpenGLFrameBuffer();
	virtual void Bind() const override;
	virtual void UnBind() const override;
	virtual void AssignTexture(ICachedTexture & texture, CachedTextureType type) override;
private:
	unsigned int m_id;
};

class COpenGLOcclusionQuery : public IOcclusionQuery
{
public:
	COpenGLOcclusionQuery()
	{
		glGenQueries(1, &m_id);
	}
	~COpenGLOcclusionQuery()
	{
		glDeleteQueries(1, &m_id);
	}
	virtual void Query(std::function<void() > const& handler, bool renderToScreen) override
	{
		if (!renderToScreen)
		{
			glDepthMask(GL_FALSE);
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		}
		glBeginQuery(GL_ANY_SAMPLES_PASSED, m_id);
		handler();
		glEndQuery(GL_ANY_SAMPLES_PASSED);
		if (!renderToScreen)
		{
			glDepthMask(GL_TRUE);
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		}
	}

	virtual bool IsVisible() const override
	{
		if (GLEW_ARB_query_buffer_object)
		{
			int result = 1;//true by default
			glGetQueryObjectiv(m_id, GL_QUERY_RESULT_NO_WAIT, &result);
			return result != 0;
		}
		else
		{
			GLint result = 0;
			glGetQueryObjectiv(m_id, GL_QUERY_RESULT_AVAILABLE, &result);
			int err = glGetError();
			if (result != 0)
			{
				glGetQueryObjectiv(m_id, GL_QUERY_RESULT, &result);
				err = glGetError();
				return result != 0;
			}
			return true;
		}
	}
private:
	GLuint m_id = 0;
};

void COpenGLRenderer::SetTexture(std::wstring const& texture, bool forceLoadNow, int flags)
{
	if (forceLoadNow)
	{
		m_textureManager->LoadTextureNow(texture, nullptr, flags);
	}
	m_textureManager->SetTexture(texture, nullptr, flags);
}

void COpenGLRenderer::SetTexture(std::wstring const& texture, TextureSlot slot, int flags /*= 0*/)
{
	m_textureManager->SetTexture(texture, slot, flags);
}

void COpenGLRenderer::SetTexture(std::wstring const& texture, const std::vector<sTeamColor> * teamcolor /*= nullptr*/, int flags /*= 0*/)
{
	m_textureManager->SetTexture(texture, teamcolor, flags);
}

static const map<RenderMode, GLenum> renderModeMap = {
	{ RenderMode::TRIANGLES, GL_TRIANGLES },
	{ RenderMode::TRIANGLE_STRIP, GL_TRIANGLE_STRIP },
	{ RenderMode::RECTANGLES, GL_QUADS },
	{ RenderMode::LINES, GL_LINES },
	{ RenderMode::LINE_LOOP, GL_LINE_LOOP }
};

COpenGLRenderer::COpenGLRenderer()
	:m_textureManager(nullptr)
{
	glDepthFunc(GL_LESS);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glewInit();

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	m_viewMatrices.push_back(glm::mat4());
	m_color.a = 1.0f;
	m_shaderManager.DoOnProgramChange([this]() {
		UpdateMatrices();
		UpdateColor();
	});

	m_defaultProgram = m_shaderManager.NewProgram();
	m_shaderManager.PushProgram(*m_defaultProgram);
}

void COpenGLRenderer::RenderArrays(RenderMode mode, std::vector<CVector3f> const& vertices, std::vector<CVector3f> const& normals, std::vector<CVector2f> const& texCoords)
{
	m_shaderManager.SetVertexAttribute(CShaderManagerOpenGL::VERTEX_ATTRIB_NAME, 3, vertices.size(), (float*)vertices.data(), false);
	m_shaderManager.SetVertexAttribute(CShaderManagerOpenGL::NORMAL_ATTRIB_NAME, 3, normals.size(), normals.empty() ? nullptr : (float*)normals.data(), false);
	m_shaderManager.SetVertexAttribute(CShaderManagerOpenGL::TEXCOORD_ATTRIB_NAME, 2, texCoords.size(), texCoords.empty() ? nullptr : (float*)texCoords.data(), false);
	glDrawArrays(renderModeMap.at(mode), 0, vertices.size());
}

void COpenGLRenderer::RenderArrays(RenderMode mode, std::vector<CVector2i> const& vertices, std::vector<CVector2f> const& texCoords)
{
	std::vector<float> fvalues;
	fvalues.reserve(vertices.size() * 2);
	for (auto& v : vertices)
	{
		fvalues.push_back(static_cast<float>(v.x));
		fvalues.push_back(static_cast<float>(v.y));
	}
	m_shaderManager.SetVertexAttribute(CShaderManagerOpenGL::VERTEX_ATTRIB_NAME, 2, vertices.size(), fvalues.data(), false);
	m_shaderManager.SetVertexAttribute(CShaderManagerOpenGL::NORMAL_ATTRIB_NAME, 3, 0, (float*)nullptr, false);
	m_shaderManager.SetVertexAttribute(CShaderManagerOpenGL::TEXCOORD_ATTRIB_NAME, 2, texCoords.size(), texCoords.empty() ? nullptr : (float*)texCoords.data(), false);
	glDrawArrays(renderModeMap.at(mode), 0, vertices.size());
}

void COpenGLRenderer::PushMatrix()
{
	m_viewMatrices.push_back(m_viewMatrices.back());
}

void COpenGLRenderer::PopMatrix()
{
	m_viewMatrices.pop_back();
	UpdateMatrices();
}

void COpenGLRenderer::Translate(const int dx, const int dy, const int dz)
{
	Translate(static_cast<float>(dx), static_cast<float>(dy), static_cast<float>(dz));
}

void COpenGLRenderer::Translate(const double dx, const double dy, const double dz)
{
	Translate(static_cast<float>(dx), static_cast<float>(dy), static_cast<float>(dz));
}

void COpenGLRenderer::Translate(const float dx, const float dy, const float dz)
{
	m_viewMatrices.back() = glm::translate(m_viewMatrices.back(), glm::vec3(dx, dy, dz));
	UpdateMatrices();
}

void COpenGLRenderer::Scale(double scale)
{
	float fscale = static_cast<float>(scale);
	m_viewMatrices.back() = glm::scale(m_viewMatrices.back(), glm::vec3(fscale, fscale, fscale));
	UpdateMatrices();
}

void COpenGLRenderer::Rotate(double angle, double x, double y, double z)
{
	m_viewMatrices.back() = glm::rotate(m_viewMatrices.back(), static_cast<float>(angle * M_PI / 180), glm::vec3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)));
	UpdateMatrices();
}

void COpenGLRenderer::GetViewMatrix(float * matrix) const
{
	memcpy(matrix, glm::value_ptr(m_viewMatrices.back()), sizeof(Matrix4F));
}

void COpenGLRenderer::ResetViewMatrix()
{
	m_viewMatrices.back() = glm::mat4();
	UpdateMatrices();
}

void COpenGLRenderer::LookAt(CVector3f const& position, CVector3f const& direction, CVector3f const& up)
{
	m_viewMatrices.back() = glm::lookAt(FromVector3(position), FromVector3(direction), FromVector3(up));
	UpdateMatrices();
}

void COpenGLRenderer::SetColor(const float r, const float g, const float b, const float a)
{
	m_color = glm::vec4(r, g, b, a);
	UpdateColor();
}

void COpenGLRenderer::SetColor(const int r, const int g, const int b, const int a)
{
	auto charToFloat = [](const int value) {return static_cast<float>(value) / UCHAR_MAX; };
	SetColor(charToFloat(r), charToFloat(g), charToFloat(b), charToFloat(a));
}

void COpenGLRenderer::SetColor(const float * color)
{
	SetColor(color[0], color[1], color[2], color[3]);
}

void COpenGLRenderer::SetColor(const int * color)
{
	SetColor(color[0], color[1], color[2], color[3]);
}

std::unique_ptr<ICachedTexture> COpenGLRenderer::RenderToTexture(std::function<void() > const& func, unsigned int width, unsigned int height)
{
	//set up texture
	GLint prevTexture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevTexture);
	auto texture = std::make_unique<COpenGlCachedTexture>();
	texture->Bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	UnbindTexture();
	//set up buffer
	GLint prevBuffer;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevBuffer);
	GLuint framebuffer = 0;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *texture, 0);
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		LogWriter::WriteLine("framebuffer error code=" + std::to_string(status));
	}
	GLint oldViewport[4];
	glGetIntegerv(GL_VIEWPORT, oldViewport);
	glViewport(0, 0, width, height);
	auto oldProjectionMatrix = m_projectionMatrix;
	m_projectionMatrix = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));
	PushMatrix();
	ResetViewMatrix();

	glClear(GL_COLOR_BUFFER_BIT);
	func();

	m_projectionMatrix = oldProjectionMatrix;
	PopMatrix();
	glViewport(oldViewport[0], oldViewport[1], oldViewport[2], oldViewport[3]);

	glBindFramebuffer(GL_FRAMEBUFFER, prevBuffer);
	glBindTexture(GL_TEXTURE_2D, prevTexture);
	glDeleteFramebuffers(1, &framebuffer);
	return move(texture);
}

std::unique_ptr<ICachedTexture> COpenGLRenderer::CreateTexture(const void * data, unsigned int width, unsigned int height, CachedTextureType type)
{
	static const std::map<CachedTextureType, GLenum> typeMap = {
		{ CachedTextureType::RGBA, GL_RGBA },
		{ CachedTextureType::ALPHA, GL_RED },
		{ CachedTextureType::DEPTH, GL_DEPTH_COMPONENT }
	};
	auto texture = std::make_unique<COpenGlCachedTexture>();
	texture->Bind();
	glTexImage2D(GL_TEXTURE_2D, 0, type == CachedTextureType::ALPHA ? GL_R8 : typeMap.at(type), width, height, 0, typeMap.at(type), GL_UNSIGNED_BYTE, data);
	if (type == CachedTextureType::ALPHA)
	{
		GLint swizzleMask[] = { GL_ZERO, GL_ZERO, GL_ZERO, GL_RED };
		glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	if (type == CachedTextureType::DEPTH)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	}
	return move(texture);
}

class CMockDrawingList : public IDrawingList
{
public:
	CMockDrawingList(std::function<void() > const& func)
		:m_func(func)
	{
	}

	virtual void Draw() const override
	{
		m_func();
	}
private:
	std::function<void() > m_func;
};

std::unique_ptr<IDrawingList> COpenGLRenderer::CreateDrawingList(std::function<void() > const& func)
{
	return std::make_unique<CMockDrawingList>(func);
	/*unsigned int list = glGenLists(1);
	glNewList(list, GL_COMPILE);
	func();
	glEndList();
	return std::make_unique<COpenGLDrawingList>(list);*/
}

std::unique_ptr<IVertexBuffer> COpenGLRenderer::CreateVertexBuffer(const float * vertex, const float * normals, const float * texcoords, size_t size, bool temp)
{
	return std::make_unique<COpenGLVertexBuffer>(vertex, normals, texcoords, size, temp, m_vao);
}

std::unique_ptr<IFrameBuffer> COpenGLRenderer::CreateFramebuffer() const
{
	return std::make_unique<COpenGLFrameBuffer>();
}

IShaderManager& COpenGLRenderer::GetShaderManager()
{
	return m_shaderManager;
}

void COpenGLRenderer::SetTextureManager(CTextureManager & textureManager)
{
	m_textureManager = &textureManager;
}

void COpenGLRenderer::SetMaterial(const float * ambient, const float * diffuse, const float * specular, const float shininess)
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

COpenGlCachedTexture::COpenGlCachedTexture()
{
	glGenTextures(1, &m_id);
}

COpenGlCachedTexture::~COpenGlCachedTexture()
{
	glDeleteTextures(1, &m_id);
}

void COpenGlCachedTexture::Bind() const
{
	glBindTexture(GL_TEXTURE_2D, m_id);
}

void COpenGlCachedTexture::UnBind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

COpenGlCachedTexture::operator unsigned int() const
{
	return m_id;
}

COpenGLDrawingList::COpenGLDrawingList(unsigned int id)
	:m_id(id)
{
}

COpenGLDrawingList::~COpenGLDrawingList()
{
	glDeleteLists(m_id, 1);
}

void COpenGLDrawingList::Draw() const
{
	glCallList(m_id);
}

COpenGLVertexBuffer::COpenGLVertexBuffer(const float * vertex, const float * normals, const float * texcoords, size_t size, bool /*temp*/, GLuint mainVAO)
	: m_mainVAO(mainVAO)
{
	GLint program;
	glGetIntegerv(GL_CURRENT_PROGRAM, &program);
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	if (vertex)
	{
		CreateVBO(size, 3, vertex, program, CShaderManagerOpenGL::VERTEX_ATTRIB_NAME);
	}
	if (normals)
	{
		CreateVBO(size, 3, normals, program, CShaderManagerOpenGL::NORMAL_ATTRIB_NAME);
	}
	if (texcoords)
	{
		CreateVBO(size, 2, texcoords, program, CShaderManagerOpenGL::TEXCOORD_ATTRIB_NAME);
	}
	UnBind();
}

void COpenGLVertexBuffer::CreateVBO(size_t size, size_t components, const void* data, GLuint program, const char* attribName)
{
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, size * components * sizeof(float), data, GL_STATIC_DRAW);
	int index = glGetAttribLocation(program, attribName);
	if (index == -1) return;
	glVertexAttribPointer(index, components, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(index);
	m_buffers.push_back(buffer);
}

COpenGLVertexBuffer::~COpenGLVertexBuffer()
{
	UnBind();
	glDeleteBuffers(m_buffers.size(), m_buffers.data());
	glDeleteBuffers(1, &m_indexesBuffer);
	glDeleteVertexArrays(1, &m_vao);
}

void COpenGLVertexBuffer::Bind() const
{
	auto err = glGetError();
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexesBuffer);
	err = glGetError();
}

void COpenGLVertexBuffer::SetIndexBuffer(unsigned int * indexPtr, size_t indexesSize)
{
	glGenBuffers(1, &m_indexesBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexesBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexesSize * sizeof(unsigned), indexPtr, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
}

void COpenGLVertexBuffer::DrawIndexes(size_t begin, size_t count)
{
	auto err = glGetError();
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, reinterpret_cast<void*>(begin * sizeof(unsigned int)));
	err = glGetError();
}

void COpenGLVertexBuffer::DrawAll(size_t count)
{
	glDrawArrays(GL_TRIANGLES, 0, count);
}

void COpenGLVertexBuffer::DrawInstanced(size_t size, size_t instanceCount)
{
	glDrawArraysInstanced(GL_TRIANGLES, 0, size, instanceCount);
}

void COpenGLVertexBuffer::UnBind() const
{
	glBindVertexArray(m_mainVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void COpenGLRenderer::WindowCoordsToWorldVector(IViewport & viewport, int x, int y, CVector3f & start, CVector3f & end) const
{
	glm::vec4 viewportData((float)viewport.GetX(), (float)viewport.GetY(), (float)viewport.GetWidth(), (float)viewport.GetHeight());
	//Set OpenGL Windows coordinates
	float winX = (float)x;
	float winY = viewportData[3] - (float)y;

	auto ToVector3f = [](glm::vec3 const& v)->CVector3f { return { v.x, v.y, v.z }; };
	//Cast a ray from eye to mouse cursor;
	start = ToVector3f(glm::unProject(glm::vec3(winX, winY, 0.0f), FromMatrix(viewport.GetViewMatrix()), FromMatrix(viewport.GetProjectionMatrix()), viewportData));
	end = ToVector3f(glm::unProject(glm::vec3(winX, winY, 1.0f), FromMatrix(viewport.GetViewMatrix()), FromMatrix(viewport.GetProjectionMatrix()), viewportData));
}

void COpenGLRenderer::WorldCoordsToWindowCoords(IViewport & viewport, CVector3f const& worldCoords, int& x, int& y) const
{
	glm::vec4 viewportData( (float)viewport.GetX(), (float)viewport.GetY(), (float)viewport.GetWidth(), (float)viewport.GetHeight() );
	auto windowPos = glm::project(FromVector3(worldCoords), FromMatrix(viewport.GetViewMatrix()), FromMatrix(viewport.GetProjectionMatrix()), viewportData);
	x = static_cast<int>(windowPos.x);
	y = static_cast<int>(viewportData[3] - windowPos.y);
}

void COpenGLRenderer::EnableLight(size_t index, bool enable)
{
	if (enable)
	{
		glEnable(GL_LIGHT0 + index);
	}
	else
	{
		glDisable(GL_LIGHT0 + index);
	}
}

static const map<LightningType, GLenum> lightningTypesMap = {
	{ LightningType::DIFFUSE, GL_DIFFUSE },
	{ LightningType::AMBIENT, GL_AMBIENT },
	{ LightningType::SPECULAR, GL_SPECULAR }
};

void COpenGLRenderer::SetLightColor(size_t index, LightningType type, float * values)
{
	glLightfv(GL_LIGHT0 + index, lightningTypesMap.at(type), values);
}

void COpenGLRenderer::SetLightPosition(size_t index, float* pos)
{
	glLightfv(GL_LIGHT0 + index, GL_POSITION, pos);
}

float COpenGLRenderer::GetMaximumAnisotropyLevel() const
{
	float aniso = 1.0f;
	if (GLEW_EXT_texture_filter_anisotropic)
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
	return aniso;
}

void COpenGLRenderer::EnableVertexLightning(bool enable)
{
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, enable ? GL_MODULATE : GL_REPLACE);
	if (enable)
	{
		glEnable(GL_LIGHTING);
		glEnable(GL_NORMALIZE);
	}
	else
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_NORMALIZE);
	}
}

void COpenGLRenderer::GetProjectionMatrix(float * matrix) const
{
	memcpy(matrix, glm::value_ptr(m_projectionMatrix), sizeof(float) * 16);
}

void COpenGLRenderer::EnableDepthTest(bool enable)
{
	if(enable)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
}

void COpenGLRenderer::EnableBlending(bool enable)
{
	if (enable)
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);
}

void COpenGLRenderer::SetUpViewport(unsigned int viewportX, unsigned int viewportY, unsigned int viewportWidth, unsigned int viewportHeight, float viewingAngle, float nearPane, float farPane)
{
	m_projectionMatrix = glm::perspectiveFov<float>(static_cast<float>(viewingAngle * 180.0 / M_PI), static_cast<float>(viewportWidth), static_cast<float>(viewportHeight), nearPane, farPane);
	glViewport(viewportX, viewportY, viewportWidth, viewportHeight);
}

void COpenGLRenderer::EnablePolygonOffset(bool enable, float factor /*= 0.0f*/, float units /*= 0.0f*/)
{
	if (enable)
	{
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(factor, units);
	}
	else
	{
		glPolygonOffset(0.0f, 0.0f);
		glDisable(GL_POLYGON_OFFSET_FILL);
	}
}

void COpenGLRenderer::ClearBuffers(bool color, bool depth)
{
	GLbitfield mask = 0;
	if (color) mask |= GL_COLOR_BUFFER_BIT;
	if (depth) mask |= GL_DEPTH_BUFFER_BIT;
	glClear(mask);
}

void COpenGLRenderer::ActivateTextureSlot(TextureSlot slot)
{
	glActiveTexture(GL_TEXTURE0 + static_cast<int>(slot));
	glEnable(GL_TEXTURE_2D);
}

void COpenGLRenderer::UnbindTexture()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

std::unique_ptr<ICachedTexture> COpenGLRenderer::CreateEmptyTexture()
{
	return std::make_unique<COpenGlCachedTexture>();
}

void COpenGLRenderer::SetTextureAnisotropy(float value)
{
	if (GLEW_EXT_texture_filter_anisotropic)
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, value);
	}
}

void COpenGLRenderer::UploadTexture(ICachedTexture & texture, unsigned char * data, unsigned int width, unsigned int height, unsigned short, int flags, TextureMipMaps const& mipmaps)
{
	texture.Bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (flags & TextureFlags::TEXTURE_NO_WRAP) ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (flags & TextureFlags::TEXTURE_NO_WRAP) ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (flags & TEXTURE_BUILD_MIPMAPS || !mipmaps.empty()) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	GLenum format = (flags & TEXTURE_BGRA) ? ((flags & TEXTURE_HAS_ALPHA) ? GL_BGRA : GL_BGR_EXT) : ((flags & TEXTURE_HAS_ALPHA) ? GL_RGBA : GL_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, (flags & TEXTURE_HAS_ALPHA) ? GL_RGBA : GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	if (flags & TEXTURE_BUILD_MIPMAPS)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	for (size_t i = 0; i < mipmaps.size(); i++)
	{
		auto& mipmap = mipmaps[i];
		glTexImage2D(GL_TEXTURE_2D, i + 1, (flags & TEXTURE_HAS_ALPHA) ? GL_RGBA : GL_RGB, mipmap.width, mipmap.height, 0, format, GL_UNSIGNED_BYTE, mipmap.data);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipmaps.size());
}

void COpenGLRenderer::UploadCompressedTexture(ICachedTexture & texture, unsigned char * data, unsigned int width, unsigned int height, size_t size, int flags, TextureMipMaps const& mipmaps)
{
	texture.Bind();
	if (!GLEW_EXT_texture_compression_s3tc)
	{
		LogWriter::WriteLine("Compressed textures are not supported");
		return;
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (flags & TextureFlags::TEXTURE_NO_WRAP) ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (flags & TextureFlags::TEXTURE_NO_WRAP) ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (flags & TEXTURE_BUILD_MIPMAPS || !mipmaps.empty()) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);

	static const std::map<int, int> compressionMap = {
		{ TEXTURE_COMPRESSION_DXT1_NO_ALPHA, GL_COMPRESSED_RGB_S3TC_DXT1_EXT },
		{ TEXTURE_COMPRESSION_DXT1, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT },
		{ TEXTURE_COMPRESSION_DXT3, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT },
		{ TEXTURE_COMPRESSION_DXT5, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT }
	};
	GLenum format = compressionMap.at(flags & TEXTURE_COMPRESSION_MASK);

	glCompressedTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, size, data);

	for (size_t i = 0; i < mipmaps.size(); i++)
	{
		auto& mipmap = mipmaps[i];
		glCompressedTexImage2DARB(GL_TEXTURE_2D, i + 1, format, mipmap.width, mipmap.height, 0, mipmap.size, mipmap.data);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipmaps.size());
}

bool COpenGLRenderer::Force32Bits() const
{
	return false;
}

bool COpenGLRenderer::ForceFlipBMP() const
{
	return false;
}

bool COpenGLRenderer::ConvertBgra() const
{
	return false;
}

std::string COpenGLRenderer::GetName() const
{
	return "OpenGL";
}

bool COpenGLRenderer::SupportsFeature(Feature feature) const
{
	if (feature == Feature::INSTANSING)
	{
		return GLEW_ARB_draw_instanced && GLEW_ARB_instanced_arrays;
	}
	return true;
}

void COpenGLRenderer::EnableMultisampling(bool enable)
{
	if (GLEW_ARB_multisample)
	{
		if (enable)
			glEnable(GL_MULTISAMPLE_ARB);
		else
			glDisable(GL_MULTISAMPLE_ARB);
	}
	else
	{
		throw std::runtime_error("MSAA is not supported");
	}
}

std::unique_ptr<IOcclusionQuery> COpenGLRenderer::CreateOcclusionQuery()
{
	return std::make_unique<COpenGLOcclusionQuery>();
}

void COpenGLRenderer::DrawIn2D(std::function<void()> const& drawHandler)
{
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glEnable(GL_BLEND);
	glm::mat4 oldProjection = m_projectionMatrix;
	m_projectionMatrix = glm::ortho(static_cast<float>(viewport[0]), static_cast<float>(viewport[2]), static_cast<float>(viewport[3]), static_cast<float>(viewport[1]));
	PushMatrix();
	ResetViewMatrix();

	drawHandler();

	m_projectionMatrix = oldProjection;
	PopMatrix();
}

COpenGLFrameBuffer::COpenGLFrameBuffer()
{
	glGenFramebuffers(1, &m_id);
	Bind();
}

COpenGLFrameBuffer::~COpenGLFrameBuffer()
{
	UnBind();
	glDeleteBuffers(1, &m_id);
}

void COpenGLFrameBuffer::Bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_id);
}

void COpenGLFrameBuffer::UnBind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, NULL);
}

void COpenGLFrameBuffer::AssignTexture(ICachedTexture & texture, CachedTextureType type)
{
	static const std::map<CachedTextureType, GLenum> typeMap = {
		{ CachedTextureType::RGBA, GL_COLOR_ATTACHMENT0 },
		{ CachedTextureType::ALPHA, GL_STENCIL_ATTACHMENT },
		{ CachedTextureType::DEPTH, GL_DEPTH_ATTACHMENT }
	};
	const std::map<CachedTextureType, pair<GLboolean, string>> extensionMap = {
		{ CachedTextureType::RGBA, {GLEW_ARB_color_buffer_float, "GL_ARB_color_buffer_float" }},
		{ CachedTextureType::ALPHA, {GLEW_ARB_stencil_texturing, "GL_ARB_stencil_texturing" }},
		{ CachedTextureType::DEPTH, {GLEW_ARB_depth_buffer_float, "GL_ARB_depth_buffer_float" }}
	};
	if (!extensionMap.at(type).first)
	{
		throw std::runtime_error(extensionMap.at(type).second + " is not supported");
	}
	if (type == CachedTextureType::DEPTH)
	{
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}
	glFramebufferTexture2D(GL_FRAMEBUFFER, typeMap.at(type), GL_TEXTURE_2D, (COpenGlCachedTexture&)texture, 0);
	auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		throw std::runtime_error("Error creating framebuffer");
	}
}

void COpenGLRenderer::UpdateMatrices() const
{
	auto m = m_projectionMatrix * m_viewMatrices.back();
	m_shaderManager.SetUniformValue("mvp_matrix", 16, 1, glm::value_ptr(m));
}

void COpenGLRenderer::UpdateColor() const
{
	m_shaderManager.SetUniformValue("color", 4, 1, glm::value_ptr(m_color));
}
