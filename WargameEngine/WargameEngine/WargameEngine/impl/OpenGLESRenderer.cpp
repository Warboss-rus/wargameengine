#include "OpenGLESRenderer.h"
#include "../LogWriter.h"
#include "../view/TextureManager.h"
#include "../Utils.h"
#include "../view/IViewport.h"
#include <GLES2/gl2ext.h>//GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, GL_BGRA_EXT, GL_COMPRESSED_RGB_S3TC_DXT1_EXT
#include <GLES3/gl3.h>

using namespace std;

namespace
{
static const std::string POSITION_KEY = "Position";
static const std::string NORMAL_KEY = "Normal";
static const std::string TEXCOORD_KEY = "TexCoord";
}

class COpenGLESCachedTexture : public ICachedTexture
{
public:
	COpenGLESCachedTexture(GLenum type = GL_TEXTURE_2D);
	~COpenGLESCachedTexture();

	operator GLuint() const { return m_id; }
	GLenum GetType() const { return m_type; }
private:
	unsigned int m_id;
	GLenum m_type;
};

class COpenGLESVertexBuffer : public IVertexBuffer
{
public:
	COpenGLESVertexBuffer(CShaderManagerOpenGLES & shaderMan, const float * vertex = nullptr, const float * normals = nullptr, const float * texcoords = nullptr, size_t size = 0, bool temp = true, GLuint mainVAO = 0);
	~COpenGLESVertexBuffer();
	virtual void Bind() const override;
	virtual void SetIndexBuffer(unsigned int * indexPtr, size_t indexesSize) override;
	virtual void DrawIndexes(size_t begin, size_t count) override;
	virtual void DrawAll(size_t count) override;
	virtual void DrawInstanced(size_t size, size_t instanceCount) override;
	virtual void UnBind() const override;
	void DoBeforeDraw(std::function<void()> const& beforeDraw);
private:
	CShaderManagerOpenGLES & m_shaderMan;
	GLuint m_vao = 0;
	GLuint m_mainVAO = 0;
	GLuint m_indexesBuffer = 0;
	std::unique_ptr<IVertexAttribCache> m_buffer;
	const float * m_vertex;
	const float * m_normals;
	const float * m_texCoords;
	size_t m_vertexCount;
	std::function<void()> m_beforeDraw;
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
		
	}
	~COpenGLESOcclusionQuery()
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
		GLuint result = 0;
		glGetQueryObjectuiv(m_id, GL_QUERY_RESULT_AVAILABLE, &result);
		if (result != 0)
		{
			glGetQueryObjectuiv(m_id, GL_QUERY_RESULT, &result);
			return result != 0;
		}
		return true;
	}
private:
	GLuint m_id = 0;
};

void COpenGLESRenderer::SetTexture(const Path& texture, bool forceLoadNow, int flags)
{
	if (forceLoadNow)
	{
		m_textureManager->LoadTextureNow(texture, nullptr, flags);
	}
	m_textureManager->SetTexture(texture, flags);
}

void COpenGLESRenderer::SetTexture(const Path& texture, TextureSlot slot, int flags /*= 0*/)
{
	m_textureManager->SetTexture(texture, slot, nullptr, flags);
}

void COpenGLESRenderer::SetTexture(const Path& texture, const std::vector<sTeamColor> * teamcolor /*= nullptr*/, int flags /*= 0*/)
{
	m_textureManager->SetTexture(texture, TextureSlot::eDiffuse, teamcolor, flags);
}

void COpenGLESRenderer::SetTexture(ICachedTexture const& texture, TextureSlot slot /*= TextureSlot::eDiffuse*/)
{
	if (slot != TextureSlot::eDiffuse) glActiveTexture(GL_TEXTURE0 + static_cast<int>(slot));
	auto& glTexture = reinterpret_cast<COpenGLESCachedTexture const&>(texture);
	glBindTexture(glTexture.GetType(), glTexture);
	if (slot != TextureSlot::eDiffuse) glActiveTexture(GL_TEXTURE0);
}

static const map<RenderMode, GLenum> renderModeMap = {
	{ RenderMode::TRIANGLES, GL_TRIANGLES },
	{ RenderMode::TRIANGLE_STRIP, GL_TRIANGLE_STRIP },
	{ RenderMode::LINES, GL_LINES },
	{ RenderMode::LINE_LOOP, GL_LINE_LOOP }
};

COpenGLESRenderer::COpenGLESRenderer()
	: m_textureManager(nullptr), m_version(3)
{
}

void COpenGLESRenderer::RenderArrays(RenderMode mode, array_view<CVector3f> const& vertices, array_view<CVector3f> const& normals, array_view<CVector2f> const& texCoords)
{
	m_matrixManager.UpdateMatrices(m_shaderManager);
	m_shaderManager.SetVertexAttribute(POSITION_KEY, 3, vertices.size(), (float*)&vertices[0].x);
	m_shaderManager.SetVertexAttribute(NORMAL_KEY, 3, normals.size(), normals.empty() ? nullptr : (float*)normals.data());
	m_shaderManager.SetVertexAttribute(TEXCOORD_KEY, 2, texCoords.size(), texCoords.empty() ? nullptr : (float*)texCoords.data());
	glDrawArrays(renderModeMap.at(mode), 0, vertices.size());
}

void COpenGLESRenderer::RenderArrays(RenderMode mode, array_view<CVector2i> const& vertices, array_view<CVector2f> const& texCoords)
{
	m_matrixManager.UpdateMatrices(m_shaderManager);
	m_shaderManager.SetVertexAttribute(POSITION_KEY, 2, vertices.size(), (int*)&vertices[0].x);
	m_shaderManager.SetVertexAttribute(NORMAL_KEY, 3, 0, (float*)nullptr, false);
	m_shaderManager.SetVertexAttribute(TEXCOORD_KEY, 2, texCoords.size(), texCoords.empty() ? nullptr : (float*)texCoords.data());
	glDrawArrays(renderModeMap.at(mode), 0, vertices.size());
}

void COpenGLESRenderer::PushMatrix()
{
	m_matrixManager.PushMatrix();
}

void COpenGLESRenderer::PopMatrix()
{
	m_matrixManager.PopMatrix();
	m_matrixManager.UpdateMatrices(m_shaderManager);
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
	m_matrixManager.Translate(dx, dy, dz);
}

void COpenGLESRenderer::Scale(double scale)
{
	m_matrixManager.Scale(static_cast<float>(scale));
}

void COpenGLESRenderer::Rotate(double angle, double x, double y, double z)
{
	m_matrixManager.Rotate(angle, static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
}

void COpenGLESRenderer::GetViewMatrix(float * matrix) const
{
	m_matrixManager.GetModelViewMatrix(matrix);
}

void COpenGLESRenderer::LookAt(CVector3f const& position, CVector3f const& direction, CVector3f const& up)
{
	m_matrixManager.LookAt(position, direction, up);
}

void COpenGLESRenderer::SetColor(const float r, const float g, const float b, const float a)
{
	float color[] = { r, g, b, a };
	SetColor(color);
}

void COpenGLESRenderer::SetColor(const int r, const int g, const int b, const int a)
{
	int color[] = { r, g, b, a };
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
	auto charToFloat = [](const int value) {return static_cast<float>(value) / UCHAR_MAX; };
	float fcolor[] = { charToFloat(color[0]), charToFloat(color[1]), charToFloat(color[2]), 1.0f };
	SetColor(fcolor);
}

void COpenGLESRenderer::RenderToTexture(std::function<void() > const& func, ICachedTexture & tex, unsigned int width, unsigned int height)
{
	//set up texture
	GLint prevTexture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevTexture);
	auto& texture = reinterpret_cast<COpenGLESCachedTexture&>(tex);
	SetTexture(texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	UnbindTexture();
	//set up buffer
	GLint prevBuffer;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevBuffer);
	GLuint framebuffer = 0;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		LogWriter::WriteLine("framebuffer error code=" + std::to_string(status));
	}
	GLint oldViewport[4];
	glGetIntegerv(GL_VIEWPORT, oldViewport);
	glViewport(0, 0, width, height);
	m_matrixManager.SaveMatrices();
	m_matrixManager.SetOrthographicProjection(0, width, 0, height);
	m_matrixManager.ResetModelView();

	glClear(GL_COLOR_BUFFER_BIT);
	func();

	m_matrixManager.RestoreMatrices();
	glViewport(oldViewport[0], oldViewport[1], oldViewport[2], oldViewport[3]);

	glBindFramebuffer(GL_FRAMEBUFFER, prevBuffer);
	glBindTexture(GL_TEXTURE_2D, prevTexture);
	glDeleteFramebuffers(1, &framebuffer);
}

std::unique_ptr<ICachedTexture> COpenGLESRenderer::CreateTexture(const void * data, unsigned int width, unsigned int height, CachedTextureType type)
{
	//tuple<format, internalFormat, type>
	static const std::map<CachedTextureType, std::tuple<GLenum, GLenum, GLenum>> formatMap = {
		{ CachedTextureType::RGBA, std::tuple<GLenum, GLenum, GLenum>{ GL_RGBA, GL_RGBA8, GL_UNSIGNED_BYTE } },
		{ CachedTextureType::RENDER_TARGET, std::tuple<GLenum, GLenum, GLenum>{ GL_RGBA, GL_RGBA8, GL_UNSIGNED_BYTE } },
		{ CachedTextureType::ALPHA, std::tuple<GLenum, GLenum, GLenum>{ GL_ALPHA, GL_ALPHA, GL_UNSIGNED_BYTE } },
		{ CachedTextureType::DEPTH, std::tuple<GLenum, GLenum, GLenum>{ GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT24, GL_UNSIGNED_INT } }
	};
	auto texture = std::make_unique<COpenGLESCachedTexture>();
	SetTexture(*texture);
	glTexImage2D(GL_TEXTURE_2D, 0, std::get<1>(formatMap.at(type)), width, height, 0, std::get<0>(formatMap.at(type)), std::get<2>(formatMap.at(type)), data);
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

ICachedTexture* COpenGLESRenderer::GetTexturePtr(const Path& texture) const
{
	return m_textureManager->GetTexturePtr(texture);
}

std::unique_ptr<IVertexBuffer> COpenGLESRenderer::CreateVertexBuffer(const float * vertex, const float * normals, const float * texcoords, size_t size, bool temp)
{
	auto buffer = std::make_unique<COpenGLESVertexBuffer>(m_shaderManager, vertex, normals, texcoords, size, temp, m_vao);
	buffer->DoBeforeDraw(std::bind(&CMatrixManagerGLM::UpdateMatrices, &m_matrixManager, std::ref(m_shaderManager)));
	return std::move(buffer);
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
	static const std::string ambientKey = "material.ambient";
	static const std::string diffuseKey = "material.diffuse";
	static const std::string specularKey = "material.specular";
	static const std::string shininessKey = "material.shininess";
	m_shaderManager.SetUniformValue(ambientKey, 4, 1, ambient);
	m_shaderManager.SetUniformValue(diffuseKey, 4, 1, diffuse);
	m_shaderManager.SetUniformValue(specularKey, 4, 1, specular);
	m_shaderManager.SetUniformValue(shininessKey, 1, 1, &shininess);
}

COpenGLESCachedTexture::COpenGLESCachedTexture(GLenum type)
	: m_type(type)
{
	glGenTextures(1, &m_id);
}

COpenGLESCachedTexture::~COpenGLESCachedTexture()
{
	glDeleteTextures(1, &m_id);
}

COpenGLESVertexBuffer::COpenGLESVertexBuffer(CShaderManagerOpenGLES & shaderMan, const float * vertex, const float * normals, const float * texcoords, size_t size, bool temp, GLuint mainVAO)
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
		std::vector<float> data(size * ((vertex ? 3 : 0) + (normals ? 3 : 0) + (texcoords ? 2 : 0)));
		size_t normalOffset = (vertex ? size * 3 : 0);
		size_t texCoordOffset = normalOffset + (normals ? size * 3 : 0);
		if (vertex) memcpy(data.data(), vertex, size * 3 * sizeof(float));
		if (normals) memcpy(data.data() + normalOffset, normals, size * 3 * sizeof(float));
		if (texcoords) memcpy(data.data() + texCoordOffset, texcoords, size * 2 * sizeof(float));
		m_buffer = m_shaderMan.CreateVertexAttribCache(data.size() * sizeof(float), data.data());
		if (vertex) m_shaderMan.SetVertexAttribute(POSITION_KEY, *m_buffer, 3, size, IShaderManager::TYPE::FLOAT32, false, 0u);
		if (normals) m_shaderMan.SetVertexAttribute(NORMAL_KEY, *m_buffer, 3, size, IShaderManager::TYPE::FLOAT32, false, normalOffset * sizeof(float));
		if (texcoords) m_shaderMan.SetVertexAttribute(TEXCOORD_KEY, *m_buffer, 2, size, IShaderManager::TYPE::FLOAT32, false, texCoordOffset * sizeof(float));
		UnBind();
	}
}

COpenGLESVertexBuffer::~COpenGLESVertexBuffer()
{
	UnBind();
	if(m_indexesBuffer) glDeleteBuffers(1, &m_indexesBuffer);
	if(m_vao) glDeleteVertexArrays(1, &m_vao);
}

void COpenGLESVertexBuffer::Bind() const
{
	if (m_vao)
	{
		glBindVertexArray(m_vao);
	}
	else
	{
		m_shaderMan.SetVertexAttribute(POSITION_KEY, 3, m_vertexCount, m_vertex);
		m_shaderMan.SetVertexAttribute(NORMAL_KEY, 3, m_vertexCount, m_normals);
		m_shaderMan.SetVertexAttribute(TEXCOORD_KEY, 2, m_vertexCount, m_texCoords);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexesBuffer);
}

void COpenGLESVertexBuffer::SetIndexBuffer(unsigned int * indexPtr, size_t indexesSize)
{
	glGenBuffers(1, &m_indexesBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexesBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexesSize * sizeof(unsigned), indexPtr, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void COpenGLESVertexBuffer::DrawIndexes(size_t begin, size_t count)
{
	if (m_beforeDraw) m_beforeDraw();
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, reinterpret_cast<void*>(begin * sizeof(unsigned int)));
}

void COpenGLESVertexBuffer::DrawAll(size_t count)
{
	if (m_beforeDraw) m_beforeDraw();
	glDrawArrays(GL_TRIANGLES, 0, count);
}

void COpenGLESVertexBuffer::DrawInstanced(size_t size, size_t instanceCount)
{
	if (m_beforeDraw) m_beforeDraw();
	glDrawArraysInstanced(GL_TRIANGLES, 0, size, instanceCount);
}

void COpenGLESVertexBuffer::UnBind() const
{
	if (m_vao)
	{
		glBindVertexArray(m_mainVAO);
	}
	else
	{
		CVector3f def;
		m_shaderMan.DisableVertexAttribute(POSITION_KEY, 3, def);
		m_shaderMan.DisableVertexAttribute(NORMAL_KEY, 3, def);
		m_shaderMan.DisableVertexAttribute(TEXCOORD_KEY, 2, def);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void COpenGLESVertexBuffer::DoBeforeDraw(std::function<void()> const& beforeDraw)
{
	m_beforeDraw = beforeDraw;
}

void COpenGLESRenderer::WindowCoordsToWorldVector(IViewport & viewport, int x, int y, CVector3f & start, CVector3f & end) const
{
	m_matrixManager.WindowCoordsToWorldVector(x, y, (float)viewport.GetX(), (float)viewport.GetY(), (float)viewport.GetWidth(), (float)viewport.GetHeight(), viewport.GetViewMatrix(), viewport.GetProjectionMatrix(), start, end);
}

void COpenGLESRenderer::WorldCoordsToWindowCoords(IViewport & viewport, CVector3f const& worldCoords, int& x, int& y) const
{
	m_matrixManager.WorldCoordsToWindowCoords(worldCoords, (float)viewport.GetX(), (float)viewport.GetY(), (float)viewport.GetWidth(), (float)viewport.GetHeight(), viewport.GetViewMatrix(), viewport.GetProjectionMatrix(), x, y);
}

void COpenGLESRenderer::SetNumberOfLights(size_t count)
{
	static const std::string numberOfLightsKey = "lightsCount";
	int number = static_cast<int>(count);
	m_shaderManager.SetUniformValue(numberOfLightsKey, 1, 1, &number);
}

void COpenGLESRenderer::SetUpLight(size_t index, CVector3f const& position, const float * ambient, const float * diffuse, const float * specular)
{
	const std::string key = "lights[" + std::to_string(index) + "].";
	m_shaderManager.SetUniformValue(key + "pos", 3, 1, position.ptr());
	m_shaderManager.SetUniformValue(key + "ambient", 4, 1, ambient);
	m_shaderManager.SetUniformValue(key + "diffuse", 4, 1, diffuse);
	m_shaderManager.SetUniformValue(key + "specular", 4, 1, specular);
}

float COpenGLESRenderer::GetMaximumAnisotropyLevel() const
{
	float aniso = 16.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
	return aniso;
}

void COpenGLESRenderer::GetProjectionMatrix(float * matrix) const
{
	m_matrixManager.GetProjectionMatrix(matrix);
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
	m_matrixManager.SetUpViewport(viewportWidth * m_vrViewport[2], viewportHeight * m_vrViewport[3], m_vrFovOverride > 0.00001f ? m_vrFovOverride : viewingAngle, nearPane, farPane);
	glViewport(viewportX + m_vrViewport[0] * viewportWidth, viewportY + m_vrViewport[1] * viewportHeight, viewportWidth * m_vrViewport[2], viewportHeight * m_vrViewport[3]);
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

void COpenGLESRenderer::UnbindTexture(TextureSlot slot)
{
	if (slot != TextureSlot::eDiffuse) glActiveTexture(GL_TEXTURE0 + static_cast<int>(slot));
	glBindTexture(GL_TEXTURE_2D, 0);
	if (slot != TextureSlot::eDiffuse) glActiveTexture(GL_TEXTURE0);
}

std::unique_ptr<ICachedTexture> COpenGLESRenderer::CreateEmptyTexture(bool cubemap)
{
	return std::make_unique<COpenGLESCachedTexture>(cubemap ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D);
}

void COpenGLESRenderer::SetTextureAnisotropy(float value)
{
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, value);
}

void COpenGLESRenderer::UploadTexture(ICachedTexture & texture, unsigned char * data, size_t width, size_t height, unsigned short /*bpp*/, int flags, TextureMipMaps const& mipmaps)
{
	SetTexture(texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (flags & TextureFlags::TEXTURE_NO_WRAP) ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (flags & TextureFlags::TEXTURE_NO_WRAP) ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (flags & TEXTURE_BUILD_MIPMAPS || !mipmaps.empty()) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	GLenum format = (flags & TEXTURE_BGRA) ? ((flags & TEXTURE_HAS_ALPHA) ? GL_BGRA_EXT : GL_BGRA_EXT) : ((flags & TEXTURE_HAS_ALPHA) ? GL_RGBA : GL_RGB);
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

void COpenGLESRenderer::UploadCompressedTexture(ICachedTexture & texture, unsigned char * data, size_t width, size_t height, size_t size, int flags, TextureMipMaps const& mipmaps)
{
	SetTexture(texture);
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipmaps.size());
}

void COpenGLESRenderer::UploadCubemap(ICachedTexture & texture, TextureMipMaps const& sides, unsigned short, int flags)
{
	SetTexture(texture);
	GLenum format = (flags & TEXTURE_BGRA) ? ((flags & TEXTURE_HAS_ALPHA) ? GL_BGRA_EXT : GL_BGRA_EXT) : ((flags & TEXTURE_HAS_ALPHA) ? GL_RGBA : GL_RGB);
	for (size_t i = 0; i < sides.size(); ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, (flags & TEXTURE_HAS_ALPHA) ? GL_RGBA : GL_RGB, static_cast<GLsizei>(sides[i].width), static_cast<GLsizei>(sides[i].height), 0, format, GL_UNSIGNED_BYTE, sides[i].data);
	}
	flags &= ~TEXTURE_BUILD_MIPMAPS;
	if (flags & TEXTURE_BUILD_MIPMAPS)
	{
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, (flags & TEXTURE_BUILD_MIPMAPS) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	UnbindTexture();
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

void ErrorCallback(GLenum /*source*/, GLenum /*type*/, GLuint /*id*/, GLenum /*severity*/, GLsizei /*length*/, const GLchar *message, const void * /*userParam*/)
{
	LogWriter::WriteLine(message);
}

void COpenGLESRenderer::Init(int width, int height)
{
#if not defined(NDEBUG) and defined(GL_ES_VERSION_3_2)
	glDebugMessageCallback(ErrorCallback, nullptr);
#endif
	glDepthFunc(GL_LESS);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glViewport(0, 0, width, height);
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	m_color[3] = 1.0f;
	m_shaderManager.DoOnProgramChange([this]() {
		m_matrixManager.UpdateMatrices(m_shaderManager);
	});
	m_defaultProgram = m_shaderManager.NewProgram();
	m_shaderManager.PushProgram(*m_defaultProgram);
	auto err = glGetError();
}

void COpenGLESRenderer::SetVrViewport(float x, float y, float width, float height, float fov)
{
	m_vrViewport[0] = x;
	m_vrViewport[1] = y;
	m_vrViewport[2] = width;
	m_vrViewport[3] = height;
	m_vrFovOverride = fov;
}

void COpenGLESRenderer::SetVrViewMatrices(std::vector<const float*> const& matrices)
{
	m_matrixManager.SetVrViewMatrices(matrices);
}

bool COpenGLESRenderer::SupportsFeature(Feature /*feature*/) const
{
	return true;
}

void COpenGLESRenderer::DrawIn2D(std::function<void()> const& drawHandler)
{
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	m_matrixManager.SaveMatrices();
	m_matrixManager.SetOrthographicProjection(viewport[0], viewport[2] + viewport[0], viewport[3] + viewport[1], viewport[1]);
	m_matrixManager.ResetModelView();

	drawHandler();

	m_matrixManager.RestoreMatrices();
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
		{ CachedTextureType::RENDER_TARGET, GL_COLOR_ATTACHMENT0 },
		{ CachedTextureType::ALPHA, GL_STENCIL_ATTACHMENT },
		{ CachedTextureType::DEPTH, GL_DEPTH_ATTACHMENT }
	};
	if (type == CachedTextureType::DEPTH)
	{
		GLenum buffers[] = { GL_NONE };
		glDrawBuffers(1, buffers);
		glReadBuffer(GL_NONE);
	}
	glFramebufferTexture2D(GL_FRAMEBUFFER, typeMap.at(type), GL_TEXTURE_2D, (COpenGLESCachedTexture&)texture, 0);
	auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		throw std::runtime_error("Error creating framebuffer");
	}
}