#include "OpenGLESRenderer.h"
#include "../LogWriter.h"
#include "../view/TextureManager.h"
#include "ShaderManagerOpenGLES.h"
#include "../Utils.h"
#include "../view/Matrix4.h"
#include "../view/IViewport.h"
#include <GLES2/gl2ext.h>//GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, GL_BGRA_EXT, GL_COMPRESSED_RGB_S3TC_DXT1_EXT
#include <GLES3/gl3.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

using namespace std;

namespace
{
static const std::string POSITION_KEY = "Position";
static const std::string NORMAL_KEY = "Normal";
static const std::string TEXCOORD_KEY = "TexCoord";
}

class COpenGlCachedTexture : public ICachedTexture
{
public:
	COpenGlCachedTexture();
	~COpenGlCachedTexture();

	virtual void Bind() const override;
	virtual void UnBind() const override;

	operator unsigned int();
private:
	unsigned int m_id;
};

class COpenGLDrawingList : public IDrawingList
{
public:
	COpenGLDrawingList(std::function<void()> const& onDraw);

	virtual void Draw() const override;
private:
	std::function<void()> m_onDraw;
};

class COpenGLVertexBuffer : public IVertexBuffer
{
public:
	COpenGLVertexBuffer(const float * vertex = nullptr, const float * normals = nullptr, const float * texcoords = nullptr, size_t size = 0, bool temp = false);
	~COpenGLVertexBuffer();
	virtual void Bind() const override;
	virtual void SetIndexBuffer(unsigned int * indexPtr, size_t indexesSize) override;
	virtual void DrawIndexes(size_t begin, size_t count) override;
	virtual void DrawAll(size_t count) override;
	virtual void DrawInstanced(size_t size, size_t instanceCount) override;
	virtual void UnBind() const override;
private:
	const float * m_vertex = nullptr;
	const float * m_normals = nullptr;
	const float * m_texCoords = nullptr;
	unsigned int* m_indexes = nullptr;
	GLuint m_vertexBuffer = 0;
	GLuint m_normalsBuffer = 0;
	GLuint m_texCoordBuffer = 0;
	GLuint m_indexesBuffer = 0;
};

class COpenGLESFrameBuffer : public IFrameBuffer
{
public:
	COpenGLESFrameBuffer();
	~COpenGLESFrameBuffer();
	virtual void Bind() const override;
	virtual void UnBind() const override;
	virtual void AssignTexture(ICachedTexture & texture, CachedTextureType type) override;
private:
	GLuint m_id;
};

class COpenGLESOcclusionQuery : public IOcclusionQuery
{
public:
	COpenGLESOcclusionQuery()
	{
		glGenQueries(1, &m_id);
	}
	~COpenGLESOcclusionQuery()
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
		GLuint result = 0;
		glGetQueryObjectuiv(m_id, GL_QUERY_RESULT_AVAILABLE, &result);
		int err = glGetError();
		if (result != 0)
		{
			glGetQueryObjectuiv(m_id, GL_QUERY_RESULT, &result);
			err = glGetError();
			return result != 0;
		}
		return true;
	}
private:
	GLuint m_id = 0;
};

static const float emptyColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };

void COpenGLESRenderer::SetTexture(std::wstring const& texture, bool forceLoadNow, int flags)
{
	if (forceLoadNow)
	{
		m_textureManager->LoadTextureNow(texture, nullptr, flags);
	}
	m_textureManager->SetTexture(texture, nullptr, flags);
	SetColor(emptyColor);
}

void COpenGLESRenderer::SetTexture(std::wstring const& texture, TextureSlot slot, int flags /*= 0*/)
{
	m_textureManager->SetTexture(texture, slot, flags);
	SetColor(emptyColor);
}

void COpenGLESRenderer::SetTexture(std::wstring const& texture, const std::vector<sTeamColor> * teamcolor /*= nullptr*/, int flags /*= 0*/)
{
	m_textureManager->SetTexture(texture, teamcolor, flags);
	SetColor(emptyColor);
}

static const map<RenderMode, GLenum> renderModeMap = {
	{ RenderMode::TRIANGLES, GL_TRIANGLES },
	{ RenderMode::TRIANGLE_STRIP, GL_TRIANGLE_STRIP },
	{ RenderMode::RECTANGLES, GL_TRIANGLE_STRIP },
	{ RenderMode::LINES, GL_LINES },
	{ RenderMode::LINE_LOOP, GL_LINE_LOOP }
};

COpenGLESRenderer::COpenGLESRenderer()
	: m_textureManager(nullptr), m_version(3)
{
	m_viewMatrices.push_back(glm::mat4());
	m_viewMatrix = &m_viewMatrices.back();
	m_shaderManager.DoOnProgramChange([this]() {
		UpdateUniforms();
	});
}

void COpenGLESRenderer::RenderArrays(RenderMode mode, std::vector<CVector3f> const& vertices, std::vector<CVector3f> const& normals, std::vector<CVector2f> const& texCoords)
{
	m_shaderManager.SetVertexAttribute(POSITION_KEY, 3, vertices.size(), (float*)&vertices[0].x);
	if (!normals.empty()) m_shaderManager.SetVertexAttribute(NORMAL_KEY, 3, normals.size(), (float*)&normals[0].x);
	if (!texCoords.empty()) m_shaderManager.SetVertexAttribute(TEXCOORD_KEY, 2, texCoords.size(), (float*)&texCoords[0].x);
	glDrawArrays(renderModeMap.at(mode), 0, vertices.size());
	float def[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_shaderManager.DisableVertexAttribute(POSITION_KEY, 3, def);
	m_shaderManager.DisableVertexAttribute(NORMAL_KEY, 3, def);
	m_shaderManager.DisableVertexAttribute(TEXCOORD_KEY, 2, def);
}

std::vector<float> TransformDoubleToFloat(const void * data, size_t count)
{
	double * ddata = (double*)data;
	std::vector<float> result;
	result.reserve(count);
	for (size_t i = 0; i < count; ++i)
	{
		result.push_back(ddata[i]);
	}
	return result;
}

void COpenGLESRenderer::RenderArrays(RenderMode mode, std::vector<CVector2i> const& vertices, std::vector<CVector2f> const& texCoords)
{
	m_shaderManager.SetVertexAttribute(POSITION_KEY, 2, vertices.size(), (int*)&vertices[0].x);
	if(!texCoords.empty()) m_shaderManager.SetVertexAttribute(TEXCOORD_KEY, 2, texCoords.size(), (float*)&texCoords[0].x);
	glDrawArrays(renderModeMap.at(mode), 0, vertices.size());
	float def[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_shaderManager.DisableVertexAttribute(POSITION_KEY, 3, def);
	m_shaderManager.DisableVertexAttribute(TEXCOORD_KEY, 2, def);
}

void COpenGLESRenderer::PushMatrix()
{
	m_viewMatrices.push_back(*m_viewMatrix);
	m_viewMatrix = &m_viewMatrices.back();
}

void COpenGLESRenderer::PopMatrix()
{
	m_viewMatrices.pop_back();
	m_viewMatrix = &m_viewMatrices.back();
	UpdateUniforms();
}

void COpenGLESRenderer::Translate(const int dx, const int dy, const int dz)
{
	Translate(static_cast<float>(dx), static_cast<float>(dy), static_cast<float>(dz));
}

void COpenGLESRenderer::Translate(const double dx, const double dy, const double dz)
{
	Translate(static_cast<float>(dx), static_cast<float>(dy), static_cast<float>(dz));
}

void COpenGLESRenderer::Translate(const float dx, const float dy, const float dz)
{
	*m_viewMatrix = glm::translate(*m_viewMatrix, glm::vec3(dx, dy, dz));
	UpdateUniforms();
}

void COpenGLESRenderer::Scale(double scale)
{
	*m_viewMatrix = glm::scale(*m_viewMatrix, glm::vec3(static_cast<float>(scale), static_cast<float>(scale), static_cast<float>(scale)));
	UpdateUniforms();
}

void COpenGLESRenderer::Rotate(double angle, double x, double y, double z)
{
	*m_viewMatrix = glm::rotate(*m_viewMatrix, (float)angle, glm::vec3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)));
	UpdateUniforms();
}

void COpenGLESRenderer::GetViewMatrix(float * matrix) const
{
	memcpy(matrix, glm::value_ptr(*m_viewMatrix), sizeof(Matrix4F));
}

void COpenGLESRenderer::ResetViewMatrix()
{
	*m_viewMatrix = glm::mat4();
	UpdateUniforms();
}

void COpenGLESRenderer::LookAt(CVector3f const& position, CVector3f const& direction, CVector3f const& up)
{
	*m_viewMatrix = glm::lookAt(glm::make_vec3(position.ptr()), glm::make_vec3(direction.ptr()), glm::make_vec3(up.ptr()));
	UpdateUniforms();
}

void COpenGLESRenderer::SetColor(const float r, const float g, const float b, const float a)
{
	float color[] = { r, g, b, a };
	SetColor(color);
}

float ToFloat(int color)
{
	return static_cast<float>(color) / UCHAR_MAX;
}

void COpenGLESRenderer::SetColor(const int r, const int g, const int b, const int a)
{
	float color[] = { ToFloat(r), ToFloat(g), ToFloat(b), ToFloat(a) };
	SetColor(color);
}

void COpenGLESRenderer::SetColor(const float * color)
{
	memcpy(m_color, color, sizeof(float) * 4);
	static const std::string colorKey = "color";
	m_shaderManager.SetUniformValue(colorKey, 4, 1, m_color);
}

void COpenGLESRenderer::SetColor(const int * color)
{
	float fcolor[] = { ToFloat(color[0]), ToFloat(color[1]), ToFloat(color[2]), 1.0f };
	SetColor(fcolor);
}

std::unique_ptr<ICachedTexture> COpenGLESRenderer::RenderToTexture(std::function<void() > const& func, unsigned int width, unsigned int height)
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
	glm::mat4 oldProjection = m_projectionMatrix;
	m_projectionMatrix = glm::ortho<float>(0, width, 0, height);
	PushMatrix();
	ResetViewMatrix();

	glClear(GL_COLOR_BUFFER_BIT);
	func();

	m_projectionMatrix = oldProjection;
	PopMatrix();
	glViewport(oldViewport[0], oldViewport[1], oldViewport[2], oldViewport[3]);

	glBindFramebuffer(GL_FRAMEBUFFER, prevBuffer);
	glBindTexture(GL_TEXTURE_2D, prevTexture);
	glDeleteFramebuffers(1, &framebuffer);
	return move(texture);
}

std::unique_ptr<ICachedTexture> COpenGLESRenderer::CreateTexture(const void * data, unsigned int width, unsigned int height, CachedTextureType type)
{
	static const std::map<CachedTextureType, GLenum> typeMap = {
		{ CachedTextureType::RGBA, GL_RGBA },
		{ CachedTextureType::ALPHA, GL_ALPHA },
		{ CachedTextureType::DEPTH, GL_DEPTH_COMPONENT }
	};
	auto texture = std::make_unique<COpenGlCachedTexture>();
	texture->Bind();
	glTexImage2D(GL_TEXTURE_2D, 0, typeMap.at(type), width, height, 0, typeMap.at(type), GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	if (type == CachedTextureType::DEPTH && m_version >= 3)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	}
	return move(texture);
}

ICachedTexture* COpenGLESRenderer::GetTexturePtr(std::wstring const& texture) const
{
	return m_textureManager->GetTexturePtr(texture);
}

std::unique_ptr<IDrawingList> COpenGLESRenderer::CreateDrawingList(std::function<void() > const& func)
{
	return std::make_unique<COpenGLDrawingList>(func);
}

std::unique_ptr<IVertexBuffer> COpenGLESRenderer::CreateVertexBuffer(const float * vertex, const float * normals, const float * texcoords, size_t size, bool temp)
{
	return std::make_unique<COpenGLVertexBuffer>(vertex, normals, texcoords, size, temp);
}

std::unique_ptr<IFrameBuffer> COpenGLESRenderer::CreateFramebuffer() const
{
	return std::make_unique<COpenGLESFrameBuffer>();
}

std::unique_ptr<IOcclusionQuery> COpenGLESRenderer::CreateOcclusionQuery()
{
	return std::make_unique<COpenGLESOcclusionQuery>();
}

IShaderManager& COpenGLESRenderer::GetShaderManager()
{
	return m_shaderManager;
}

void COpenGLESRenderer::SetTextureManager(CTextureManager & textureManager)
{
	m_textureManager = &textureManager;
}

void COpenGLESRenderer::SetMaterial(const float * ambient, const float * diffuse, const float * specular, const float shininess)
{
	/*glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);*/
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

COpenGlCachedTexture::operator unsigned int()
{
	return m_id;
}

COpenGLDrawingList::COpenGLDrawingList(std::function<void()> const& onDraw)
	:m_onDraw(onDraw)
{
}

void COpenGLDrawingList::Draw() const
{
	m_onDraw();
}

COpenGLVertexBuffer::COpenGLVertexBuffer(const float * vertex, const float * normals, const float * texcoords, size_t size, bool temp)
{
	if (temp)
	{
		m_vertex = vertex;
		m_normals = normals;
		m_texCoords = texcoords;
	}
	else
	{
		if (vertex)
		{
			glGenBuffers(1, &m_vertexBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, size * 3 * sizeof(float), vertex, GL_STATIC_DRAW);
		}
		if (normals)
		{
			glGenBuffers(1, &m_normalsBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, m_normalsBuffer);
			glBufferData(GL_ARRAY_BUFFER, size * 3 * sizeof(float), normals, GL_STATIC_DRAW);
		}
		if (texcoords)
		{
			glGenBuffers(1, &m_texCoordBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, m_texCoordBuffer);
			glBufferData(GL_ARRAY_BUFFER, size * 2 * sizeof(float), texcoords, GL_STATIC_DRAW);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

COpenGLVertexBuffer::~COpenGLVertexBuffer()
{
	UnBind();
	glDeleteBuffers(4, &m_vertexBuffer);
}

void COpenGLVertexBuffer::Bind() const
{
	GLint program;
	glGetIntegerv(GL_CURRENT_PROGRAM, &program);
	if (m_vertex || m_vertexBuffer)
	{
		int index = glGetAttribLocation(program, POSITION_KEY.c_str());
		if (m_vertexBuffer) glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 0, m_vertexBuffer ? 0 : m_vertex);
	}
	if (m_normals || m_normalsBuffer)
	{
		int index = glGetAttribLocation(program, NORMAL_KEY.c_str());
		if (m_normalsBuffer) glBindBuffer(GL_ARRAY_BUFFER, m_normalsBuffer);
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 0, m_normalsBuffer ? 0 : m_normals);
	}
	if (m_texCoords || m_texCoordBuffer)
	{
		int index = glGetAttribLocation(program, TEXCOORD_KEY.c_str());
		if (m_texCoordBuffer) glBindBuffer(GL_ARRAY_BUFFER, m_texCoordBuffer);
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, 2, GL_FLOAT, GL_FALSE, 0, m_texCoordBuffer ? 0 : m_texCoords);
	}
	if (m_indexesBuffer)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexesBuffer);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void COpenGLVertexBuffer::SetIndexBuffer(unsigned int * indexPtr, size_t indexesSize)
{
	if ((m_vertexBuffer || m_normalsBuffer || m_texCoordBuffer) && indexPtr)
	{
		glGenBuffers(1, &m_indexesBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexesBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexesSize * sizeof(unsigned), indexPtr, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	else
	{
		m_indexes = indexPtr;
	}
}

void COpenGLVertexBuffer::DrawIndexes(size_t begin, size_t count)
{
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, m_indexesBuffer ? reinterpret_cast<void*>(begin * sizeof(unsigned int)) : m_indexes + begin);
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
	GLint program;
	glGetIntegerv(GL_CURRENT_PROGRAM, &program);
	glDisableVertexAttribArray(glGetAttribLocation(program, TEXCOORD_KEY.c_str()));
	glDisableVertexAttribArray(glGetAttribLocation(program, NORMAL_KEY.c_str()));
	glDisableVertexAttribArray(glGetAttribLocation(program, POSITION_KEY.c_str()));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void COpenGLESRenderer::WindowCoordsToWorldVector(IViewport & viewport, int x, int y, CVector3f & start, CVector3f & end) const
{
	float viewportData[4] = { (float)viewport.GetX(), (float)viewport.GetY(), (float)viewport.GetWidth(), (float)viewport.GetHeight() };
	//Set OpenGL Windows coordinates
	double winX = (double)x;
	double winY = viewportData[3] - (double)y;

	//Cast a ray from eye to mouse cursor;
	auto ToVector3f = [](glm::vec3 const& src) {return CVector3f(src.x, src.y, src.z); };
	start = ToVector3f(glm::unProject(glm::vec3(winX, winY, 0.0f), glm::make_mat4(viewport.GetViewMatrix()), glm::make_mat4(viewport.GetProjectionMatrix()),
		glm::make_vec4(viewportData)));
	end = ToVector3f(glm::unProject(glm::vec3(winX, winY, 1.0f), glm::make_mat4(viewport.GetViewMatrix()), glm::make_mat4(viewport.GetProjectionMatrix()),
		glm::make_vec4(viewportData)));
}

void COpenGLESRenderer::WorldCoordsToWindowCoords(IViewport & viewport, CVector3f const& worldCoords, int& x, int& y) const
{
	float viewportData[4] = { (float)viewport.GetX(), (float)viewport.GetY(), (float)viewport.GetWidth(), (float)viewport.GetHeight() };
	auto windowPos = glm::project(glm::make_vec3(worldCoords.ptr()), glm::make_mat4(viewport.GetViewMatrix()), glm::make_mat4(viewport.GetProjectionMatrix()), glm::make_vec4(viewportData));
	x = static_cast<int>(windowPos.x);
	y = static_cast<int>(viewportData[3] - windowPos.y);
}

void COpenGLESRenderer::EnableLight(size_t index, bool enable)
{
	/*if (enable)
	{
		glEnable(GL_LIGHT0 + index);
	}
	else
	{
		glDisable(GL_LIGHT0 + index);
	}*/
}

/*static const map<LightningType, GLenum> lightningTypesMap = {
	{ LightningType::DIFFUSE, GL_DIFFUSE },
	{ LightningType::AMBIENT, GL_AMBIENT },
	{ LightningType::SPECULAR, GL_SPECULAR }
};*/

void COpenGLESRenderer::SetLightColor(size_t index, LightningType type, float * values)
{
	//glLightfv(GL_LIGHT0 + index, lightningTypesMap.at(type), values);
}

void COpenGLESRenderer::SetLightPosition(size_t index, float* pos)
{
	//glLightfv(GL_LIGHT0 + index, GL_POSITION, pos);
}

float COpenGLESRenderer::GetMaximumAnisotropyLevel() const
{
	float aniso = 16.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
	return aniso;
}

void COpenGLESRenderer::GetProjectionMatrix(float * matrix) const
{
	memcpy(matrix, glm::value_ptr(m_projectionMatrix), sizeof(Matrix4F));
}

void COpenGLESRenderer::EnableDepthTest(bool enable)
{
	if(enable)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
}

void COpenGLESRenderer::EnableBlending(bool enable)
{
	if (enable)
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);
}

void COpenGLESRenderer::SetUpViewport(unsigned int viewportX, unsigned int viewportY, unsigned int viewportWidth, unsigned int viewportHeight, float viewingAngle, float nearPane, float farPane)
{
	m_projectionMatrix = glm::perspectiveFov<float>(static_cast<float>(viewingAngle * 180.0 / M_PI), viewportWidth, viewportHeight, nearPane, farPane);
	glViewport(viewportX, viewportY, viewportWidth, viewportHeight);
}

void COpenGLESRenderer::EnablePolygonOffset(bool enable, float factor /*= 0.0f*/, float units /*= 0.0f*/)
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

void COpenGLESRenderer::ClearBuffers(bool color, bool depth)
{
	GLbitfield mask = 0;
	if (color) mask |= GL_COLOR_BUFFER_BIT;
	if (depth) mask |= GL_DEPTH_BUFFER_BIT;
	glClear(mask);
}

void COpenGLESRenderer::ActivateTextureSlot(TextureSlot slot)
{
	glActiveTexture(GL_TEXTURE0 + static_cast<int>(slot));
	glEnable(GL_TEXTURE_2D);
}

void COpenGLESRenderer::UnbindTexture()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

std::unique_ptr<ICachedTexture> COpenGLESRenderer::CreateEmptyTexture()
{
	return std::make_unique<COpenGlCachedTexture>();
}

void COpenGLESRenderer::SetTextureAnisotropy(float value)
{
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, value);
}

void COpenGLESRenderer::UploadTexture(ICachedTexture & texture, unsigned char * data, unsigned int width, unsigned int height, unsigned short /*bpp*/, int flags, TextureMipMaps const& mipmaps)
{
	GLint maxTextureSize;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
	auto error = glGetError();
	texture.Bind();
	error = glGetError();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (flags & TextureFlags::TEXTURE_NO_WRAP) ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (flags & TextureFlags::TEXTURE_NO_WRAP) ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	error = glGetError();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (flags & TEXTURE_BUILD_MIPMAPS || !mipmaps.empty()) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	error = glGetError();
	GLenum format = (flags & TEXTURE_BGRA) ? GL_BGRA_EXT : ((flags & TEXTURE_HAS_ALPHA) ? GL_RGBA : GL_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, (flags & TEXTURE_HAS_ALPHA) ? GL_RGBA : GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	error = glGetError();
	for (size_t i = 0; i < mipmaps.size(); i++)
	{
		auto& mipmap = mipmaps[i];
		glTexImage2D(GL_TEXTURE_2D, i + 1, (flags & TEXTURE_HAS_ALPHA) ? GL_RGBA : GL_RGB, mipmap.width, mipmap.height, 0, format, GL_UNSIGNED_BYTE, mipmap.data);
	}
	if (m_version >= 3)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipmaps.size());
	}
}

void COpenGLESRenderer::UploadCompressedTexture(ICachedTexture & texture, unsigned char * data, unsigned int width, unsigned int height, size_t size, int flags, TextureMipMaps const& mipmaps)
{
	texture.Bind();
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
		glCompressedTexImage2D(GL_TEXTURE_2D, i + 1, format, mipmap.width, mipmap.height, 0, mipmap.size, mipmap.data);
	}
	if (m_version >= 3)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipmaps.size());
	}
}

bool COpenGLESRenderer::Force32Bits() const
{
	return false;
}

bool COpenGLESRenderer::ForceFlipBMP() const
{
	return false;
}

bool COpenGLESRenderer::ConvertBgra() const
{
	return true;
}

std::string COpenGLESRenderer::GetName() const
{
	return "OpenGLES";
}

void COpenGLESRenderer::SetVersion(int version)
{
	m_version = version;
}

void COpenGLESRenderer::Init(int width, int height)
{
	glDepthFunc(GL_LESS);
	glEnable(GL_TEXTURE_2D);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glViewport(0, 0, width, height);
	m_defaultProgram = m_shaderManager.NewProgram();
	m_shaderManager.PushProgram(*m_defaultProgram);
}

bool COpenGLESRenderer::SupportsFeature(Feature /*feature*/) const
{
	return true;
}

void COpenGLESRenderer::UpdateUniforms() const
{
	auto m = m_projectionMatrix * *m_viewMatrix;
	static const std::string mvpKey = "mvp_matrix";
	m_shaderManager.SetUniformValue(mvpKey, 16, 1, glm::value_ptr(m));
	static const std::string colorKey = "color";
	m_shaderManager.SetUniformValue(colorKey, 4, 1, m_color);
}

void COpenGLESRenderer::DrawIn2D(std::function<void()> const& drawHandler)
{
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glEnable(GL_BLEND);
	glm::mat4 oldProjection = m_projectionMatrix;
	m_projectionMatrix = glm::ortho<float>(viewport[0], viewport[2], viewport[3], viewport[1]);
	m_viewMatrices.push_back(glm::mat4());
	m_viewMatrix = &m_viewMatrices.back();
	UpdateUniforms();

	drawHandler();

	m_projectionMatrix = oldProjection;;
	PopMatrix();
}

COpenGLESFrameBuffer::COpenGLESFrameBuffer()
{
	glGenFramebuffers(1, &m_id);
	Bind();
}

COpenGLESFrameBuffer::~COpenGLESFrameBuffer()
{
	UnBind();
	glDeleteBuffers(1, &m_id);
}

void COpenGLESFrameBuffer::Bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_id);
}

void COpenGLESFrameBuffer::UnBind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void COpenGLESFrameBuffer::AssignTexture(ICachedTexture & texture, CachedTextureType type)
{
	static const std::map<CachedTextureType, GLenum> typeMap = {
		{ CachedTextureType::RGBA, GL_COLOR_ATTACHMENT0 },
		{ CachedTextureType::ALPHA, GL_STENCIL_ATTACHMENT },
		{ CachedTextureType::DEPTH, GL_DEPTH_ATTACHMENT }
	};
	if (type == CachedTextureType::DEPTH)
	{
		glDrawBuffers(0, NULL);
		glReadBuffer(GL_NONE);
	}
	glFramebufferTexture2D(GL_FRAMEBUFFER, typeMap.at(type), GL_TEXTURE_2D, (COpenGlCachedTexture&)texture, 0);
	auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		throw std::runtime_error("Error creating framebuffer");
	}
}