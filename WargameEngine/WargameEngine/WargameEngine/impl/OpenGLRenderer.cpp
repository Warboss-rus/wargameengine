#include "OpenGLRenderer.h"
#include <GL/glew.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#ifdef _WINDOWS
#include <Windows.h>
#endif
#include <GL/gl.h>
#endif
#include "../LogWriter.h"
#include "../view/TextureManager.h"
#include "../view/Matrix4.h"
#include "../view/IViewport.h"
#include <glm/gtc/matrix_transform.hpp>
#pragma warning(push)
#pragma warning(disable: 4201)
#include <glm/gtc/type_ptr.hpp>
#pragma warning(pop)
#define _USE_MATH_DEFINES
#include <math.h>

namespace
{
static const std::string VERTEX_ATTRIB_NAME = "Position";
static const std::string NORMAL_ATTRIB_NAME = "Normal";
static const std::string TEXCOORD_ATTRIB_NAME = "TexCoord";
}

using namespace std;

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
	COpenGLVertexBuffer(CShaderManagerOpenGL & shaderMan, const float * vertex = nullptr, const float * normals = nullptr, const float * texcoords = nullptr, size_t size = 0, bool temp = true, GLuint mainVAO = 0);
	~COpenGLVertexBuffer();
	virtual void Bind() const override;
	virtual void SetIndexBuffer(unsigned int * indexPtr, size_t indexesSize) override;
	virtual void DrawIndexes(size_t begin, size_t count) override;
	virtual void DrawAll(size_t count) override;
	virtual void DrawInstanced(size_t size, size_t instanceCount) override;
	virtual void UnBind() const override;
private:
	void CreateVBO(size_t size, size_t components, const float* data, const std::string& attribName);
	CShaderManagerOpenGL & m_shaderMan;
	GLuint m_vao = 0;
	GLuint m_mainVAO = 0;
	GLuint m_indexesBuffer = 0;
	std::vector<std::unique_ptr<IVertexAttribCache>> m_buffers;
	const float * m_vertex;
	const float * m_normals;
	const float * m_texCoords;
	size_t m_vertexCount;
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
	GLuint m_id;
};

class COpenGLOcclusionQuery : public IOcclusionQuery
{
public:
	COpenGLOcclusionQuery()
	{
		
	}
	~COpenGLOcclusionQuery()
	{
		glDeleteQueries(1, &m_id);
	}
	virtual void Query(std::function<void() > const& handler, bool renderToScreen) override
	{
		if (!m_id)
		{
			glGenQueries(1, &m_id);
		}
		if (!renderToScreen)
		{
			glDepthMask(GL_FALSE);
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		}
		glBeginQuery(GL_ANY_SAMPLES_PASSED_CONSERVATIVE, m_id);
		handler();
		glEndQuery(GL_ANY_SAMPLES_PASSED_CONSERVATIVE);
		if (!renderToScreen)
		{
			glDepthMask(GL_TRUE);
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		}
	}

	virtual bool IsVisible() const override
	{
		if (!m_id) return true;
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
			if (result != 0)
			{
				glGetQueryObjectiv(m_id, GL_QUERY_RESULT, &result);
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
	{ RenderMode::RECTANGLES, GL_QUADS },//deprecated
	{ RenderMode::LINES, GL_LINES },
	{ RenderMode::LINE_LOOP, GL_LINE_LOOP } //
};


#ifdef _WINDOWS
void APIENTRY ErrorCallback(GLenum /*source*/, GLenum /*type*/, GLuint /*id*/, GLenum /*severity*/, GLsizei /*length*/, const GLchar *message, const void * /*userParam*/)
#else
void ErrorCallback(GLenum /*source*/, GLenum /*type*/, GLuint /*id*/, GLenum /*severity*/, GLsizei /*length*/, const GLchar *message, const void * /*userParam*/)
#endif
{
	LogWriter::WriteLine(message);
}

COpenGLRenderer::COpenGLRenderer()
	:m_textureManager(nullptr)
{
	glewInit();
	glDepthFunc(GL_LESS);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#ifdef _DEBUG
	glDebugMessageCallback(ErrorCallback, nullptr);
#endif

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	m_modelMatrices.push_back(glm::mat4());
	m_modelMatrix = &m_modelMatrices.back();
	m_color[3] = 1.0f;
	m_shaderManager.DoOnProgramChange([this]() {
		UpdateMatrices();
		UpdateColor();
	});

	m_defaultProgram = m_shaderManager.NewProgram();
	m_shaderManager.PushProgram(*m_defaultProgram);
}

void COpenGLRenderer::RenderArrays(RenderMode mode, std::vector<CVector3f> const& vertices, std::vector<CVector3f> const& normals, std::vector<CVector2f> const& texCoords)
{
	m_shaderManager.SetVertexAttribute(VERTEX_ATTRIB_NAME, 3, vertices.size(), (float*)vertices.data(), false);
	m_shaderManager.SetVertexAttribute(NORMAL_ATTRIB_NAME, 3, normals.size(), normals.empty() ? nullptr : (float*)normals.data(), false);
	m_shaderManager.SetVertexAttribute(TEXCOORD_ATTRIB_NAME, 2, texCoords.size(), texCoords.empty() ? nullptr : (float*)texCoords.data(), false);
	glDrawArrays(renderModeMap.at(mode), 0, static_cast<GLsizei>(vertices.size()));
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
	m_shaderManager.SetVertexAttribute(VERTEX_ATTRIB_NAME, 2, vertices.size(), fvalues.data(), false);
	m_shaderManager.SetVertexAttribute(NORMAL_ATTRIB_NAME, 3, 0, (float*)nullptr, false);
	m_shaderManager.SetVertexAttribute(TEXCOORD_ATTRIB_NAME, 2, texCoords.size(), texCoords.empty() ? nullptr : (float*)texCoords.data(), false);
	glDrawArrays(renderModeMap.at(mode), 0, static_cast<GLsizei>(vertices.size()));
}

void COpenGLRenderer::PushMatrix()
{
	m_modelMatrices.push_back(m_modelMatrices.back());
	m_modelMatrix = &m_modelMatrices.back();
}

void COpenGLRenderer::PopMatrix()
{
	m_modelMatrices.pop_back();
	m_modelMatrix = &m_modelMatrices.back();
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
	if (abs(dx) < FLT_EPSILON && abs(dy) < FLT_EPSILON && abs(dz) < FLT_EPSILON) return;
	*m_modelMatrix = glm::translate(*m_modelMatrix, glm::vec3(dx, dy, dz));
	UpdateMatrices();
}

void COpenGLRenderer::Scale(double scale)
{
	if (fabs(scale - 1.0) < DBL_EPSILON) return;
	float fscale = static_cast<float>(scale);
	*m_modelMatrix = glm::scale(*m_modelMatrix, glm::vec3(fscale, fscale, fscale));
	UpdateMatrices();
}

void COpenGLRenderer::Rotate(double angle, double x, double y, double z)
{
	if (fabs(angle) < DBL_EPSILON) return;
	*m_modelMatrix = glm::rotate(*m_modelMatrix, static_cast<float>(angle * M_PI / 180), glm::vec3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)));
	UpdateMatrices();
}

void COpenGLRenderer::GetViewMatrix(float * matrix) const
{
	glm::mat4 modelView = m_viewMatrix * *m_modelMatrix;
	memcpy(matrix, glm::value_ptr(modelView), sizeof(Matrix4F));
}

void COpenGLRenderer::ResetModelView()
{
	*m_modelMatrix = glm::mat4();
	m_viewMatrix = glm::mat4();
	UpdateMatrices();
}

void COpenGLRenderer::LookAt(CVector3f const& position, CVector3f const& direction, CVector3f const& up)
{
	m_viewMatrix = glm::lookAt(glm::make_vec3(position.ptr()), glm::make_vec3(direction.ptr()), glm::make_vec3(up.ptr()));
	*m_modelMatrix = glm::mat4();
	UpdateMatrices();
}

void COpenGLRenderer::SetColor(const float r, const float g, const float b, const float a)
{
	const float color[] = { r, g, b, a };
	SetColor(color);
}

void COpenGLRenderer::SetColor(const int r, const int g, const int b, const int a)
{
	const int color[] = { r, g, b, a };
	SetColor(color);
}

void COpenGLRenderer::SetColor(const float * color)
{
	memcpy(m_color, color, sizeof(float) * 4);
	UpdateColor();
}

void COpenGLRenderer::SetColor(const int * color)
{
	auto charToFloat = [](const int value) {return static_cast<float>(value) / UCHAR_MAX; };
	float fcolor[] = { charToFloat(color[0]), charToFloat(color[1]), charToFloat(color[2]), charToFloat(color[3]) };
	SetColor(fcolor);
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
	glm::mat4 oldProjectionMatrix = m_projectionMatrix;
	auto oldView = m_viewMatrix;
	m_projectionMatrix = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));
	PushMatrix();
	ResetModelView();

	glClear(GL_COLOR_BUFFER_BIT);
	func();

	m_projectionMatrix = oldProjectionMatrix;
	m_viewMatrix = oldView;
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

ICachedTexture* COpenGLRenderer::GetTexturePtr(std::wstring const& texture) const
{
	return m_textureManager->GetTexturePtr(texture);
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
	return std::make_unique<COpenGLVertexBuffer>(m_shaderManager, vertex, normals, texcoords, size, temp, m_vao);
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
	static const std::string ambientKey = "material.ambient";
	static const std::string diffuseKey = "material.diffuse";
	static const std::string specularKey = "material.specular";
	static const std::string shininessKey = "material.shininess";
	m_shaderManager.SetUniformValue(ambientKey, 4, 1, ambient);
	m_shaderManager.SetUniformValue(diffuseKey, 4, 1, diffuse);
	m_shaderManager.SetUniformValue(specularKey, 4, 1, specular);
	m_shaderManager.SetUniformValue(shininessKey, 1, 1, &shininess);
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

COpenGLVertexBuffer::COpenGLVertexBuffer(CShaderManagerOpenGL & shaderMan, const float * vertex, const float * normals, const float * texcoords, size_t size, bool temp, GLuint mainVAO)
	: m_shaderMan(shaderMan)
	, m_mainVAO(mainVAO)
{
	if (temp)
	{
		m_vertex = vertex;
		m_normals = normals;
		m_texCoords = texcoords;
		m_vertexCount = size;
	}
	else
	{
		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);
		if (vertex)
		{
			CreateVBO(size, 3, vertex, VERTEX_ATTRIB_NAME);
		}
		if (normals)
		{
			CreateVBO(size, 3, normals, NORMAL_ATTRIB_NAME);
		}
		if (texcoords)
		{
			CreateVBO(size, 2, texcoords, TEXCOORD_ATTRIB_NAME);
		}
		UnBind();
	}
}

void COpenGLVertexBuffer::CreateVBO(size_t size, size_t components, const float* data, const std::string& attribName)
{
	m_buffers.push_back(m_shaderMan.CreateVertexAttribCache(static_cast<int>(components), size, data));
	m_shaderMan.SetVertexAttribute(attribName, *m_buffers.back());
}

COpenGLVertexBuffer::~COpenGLVertexBuffer()
{
	UnBind();
	if(m_indexesBuffer) glDeleteBuffers(1, &m_indexesBuffer);
	if(m_vao) glDeleteVertexArrays(1, &m_vao);
}

void COpenGLVertexBuffer::Bind() const
{
	if (m_vao)
	{
		glBindVertexArray(m_vao);
	}
	else
	{
		m_shaderMan.SetVertexAttribute(VERTEX_ATTRIB_NAME, 3, m_vertexCount, m_vertex);
		m_shaderMan.SetVertexAttribute(NORMAL_ATTRIB_NAME, 3, m_vertexCount, m_normals);
		m_shaderMan.SetVertexAttribute(TEXCOORD_ATTRIB_NAME, 2, m_vertexCount, m_texCoords);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexesBuffer);
}

void COpenGLVertexBuffer::SetIndexBuffer(unsigned int * indexPtr, size_t indexesSize)
{
	glGenBuffers(1, &m_indexesBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexesBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexesSize * sizeof(unsigned), indexPtr, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void COpenGLVertexBuffer::DrawIndexes(size_t begin, size_t count)
{
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(count), GL_UNSIGNED_INT, reinterpret_cast<void*>(begin * sizeof(unsigned int)));
}

void COpenGLVertexBuffer::DrawAll(size_t count)
{
	glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(count));
}

void COpenGLVertexBuffer::DrawInstanced(size_t size, size_t instanceCount)
{
	glDrawArraysInstanced(GL_TRIANGLES, 0, static_cast<GLsizei>(size), static_cast<GLsizei>(instanceCount));
}

void COpenGLVertexBuffer::UnBind() const
{
	if (m_vao)
	{
		glBindVertexArray(m_mainVAO);
	}
	else
	{
		CVector3f def;
		m_shaderMan.DisableVertexAttribute(VERTEX_ATTRIB_NAME, 3, def);
		m_shaderMan.DisableVertexAttribute(NORMAL_ATTRIB_NAME, 3, def);
		m_shaderMan.DisableVertexAttribute(TEXCOORD_ATTRIB_NAME, 2, def);
	}
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
	glm::mat4 proj = glm::make_mat4(viewport.GetProjectionMatrix());
	glm::mat4 view = glm::make_mat4(viewport.GetViewMatrix());
	start = ToVector3f(glm::unProject(glm::vec3(winX, winY, 0.0f), view, proj, viewportData));
	end = ToVector3f(glm::unProject(glm::vec3(winX, winY, 1.0f), view, proj, viewportData));
}

void COpenGLRenderer::WorldCoordsToWindowCoords(IViewport & viewport, CVector3f const& worldCoords, int& x, int& y) const
{
	glm::vec4 viewportData( (float)viewport.GetX(), (float)viewport.GetY(), (float)viewport.GetWidth(), (float)viewport.GetHeight() );
	auto windowPos = glm::project(glm::make_vec3(worldCoords.ptr()), glm::make_mat4(viewport.GetViewMatrix()), glm::make_mat4(viewport.GetProjectionMatrix()), viewportData);
	x = static_cast<int>(windowPos.x);
	y = static_cast<int>(viewportData[3] - windowPos.y);
}

void COpenGLRenderer::SetNumberOfLights(size_t count)
{
	static const std::string numberOfLightsKey = "lightsCount";
	int number = static_cast<int>(count);
	m_shaderManager.SetUniformValue(numberOfLightsKey, 1, 1, &number);
}

void COpenGLRenderer::SetUpLight(size_t index, CVector3f const& position, const float * ambient, const float * diffuse, const float * specular)
{
	const std::string key = "lights[" + std::to_string(index) + "].";
	m_shaderManager.SetUniformValue(key + "pos", 3, 1, position.ptr());
	m_shaderManager.SetUniformValue(key + "ambient", 4, 1, ambient);
	m_shaderManager.SetUniformValue(key + "diffuse", 4, 1, diffuse);
	m_shaderManager.SetUniformValue(key + "specular", 4, 1, specular);
}

float COpenGLRenderer::GetMaximumAnisotropyLevel() const
{
	float aniso = 1.0f;
	if (GLEW_EXT_texture_filter_anisotropic)
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
	return aniso;
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
		glTexImage2D(GL_TEXTURE_2D, i + 1, (flags & TEXTURE_HAS_ALPHA) ? GL_RGBA : GL_RGB, static_cast<GLsizei>(mipmap.width), static_cast<GLsizei>(mipmap.height), 0, format, GL_UNSIGNED_BYTE, mipmap.data);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, static_cast<GLsizei>(mipmaps.size()));
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

	glCompressedTexImage2D(GL_TEXTURE_2D, 0, format, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, static_cast<GLsizei>(size), data);

	for (size_t i = 0; i < mipmaps.size(); i++)
	{
		auto& mipmap = mipmaps[i];
		glCompressedTexImage2D(GL_TEXTURE_2D, i + 1, format, static_cast<GLsizei>(mipmap.width), static_cast<GLsizei>(mipmap.height), 0, static_cast<GLsizei>(mipmap.size), mipmap.data);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, static_cast<GLsizei>(mipmaps.size()));
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
	glm::mat4 oldView = m_viewMatrix;
	m_projectionMatrix = glm::ortho(static_cast<float>(viewport[0]), static_cast<float>(viewport[2]), static_cast<float>(viewport[3]), static_cast<float>(viewport[1]));
	PushMatrix();
	ResetModelView();

	drawHandler();

	m_projectionMatrix = oldProjection;
	m_viewMatrix = oldView;
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
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
	glm::mat4 m = m_projectionMatrix * m_viewMatrix * *m_modelMatrix;
	static const std::string mvpMatrixKey = "mvp_matrix";
	static const std::string view_matrix_key = "view_matrix";
	static const std::string model_matrix_key = "model_matrix";
	static const std::string proj_matrix_key = "proj_matrix";
	m_shaderManager.SetUniformValue(mvpMatrixKey, 16, 1, glm::value_ptr(m));
	m_shaderManager.SetUniformValue(view_matrix_key, 16, 1, glm::value_ptr(m_viewMatrix));
	m_shaderManager.SetUniformValue(model_matrix_key, 16, 1, glm::value_ptr(*m_modelMatrix));
	m_shaderManager.SetUniformValue(proj_matrix_key, 16, 1, glm::value_ptr(m_projectionMatrix));
}

void COpenGLRenderer::UpdateColor() const
{
	m_shaderManager.SetUniformValue("color", 4, 1, m_color);
}
