#include "OpenGLESRenderer.h"
#include "../LogWriter.h"
#include "TextureManager.h"
#include "ShaderManagerOpenGLES.h"
#include "../Utils.h"
#include "Matrix4.h"
#include "IViewport.h"
#include "gluLite.h"
#include <GLES2/gl2ext.h>
#include <GLES3/gl3.h>

using namespace std;

static const int positionIndex = 0;
static const int normalIndex = 2;
static const int texCoordIndex = 1;

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
	COpenGLVertexBuffer(const float * vertex = nullptr, const float * normals = nullptr, const float * texcoords = nullptr);
	~COpenGLVertexBuffer();
	virtual void Bind() const override;
	virtual void DrawIndexes(unsigned int * indexPtr, size_t count) override;
	virtual void DrawAll(size_t count) override;
	virtual void DrawInstanced(size_t size, size_t instanceCount) override;
	virtual void UnBind() const override;
private:
	const float * m_vertex;
	const float * m_normals;
	const float * m_texCoords;
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
	unsigned int m_id;
};


void COpenGLESRenderer::SetTexture(std::wstring const& texture, bool forceLoadNow, int flags)
{
	if (forceLoadNow)
	{
		m_textureManager->LoadTextureNow(texture, nullptr, flags);
	}
	m_textureManager->SetTexture(texture, nullptr, flags);
}

void COpenGLESRenderer::SetTexture(std::wstring const& texture, TextureSlot slot, int flags /*= 0*/)
{
	m_textureManager->SetTexture(texture, slot, flags);
}

void COpenGLESRenderer::SetTexture(std::wstring const& texture, const std::vector<sTeamColor> * teamcolor /*= nullptr*/, int flags /*= 0*/)
{
	m_textureManager->SetTexture(texture, teamcolor, flags);
}

static const map<RenderMode, GLenum> renderModeMap = {
	{ RenderMode::TRIANGLES, GL_TRIANGLES },
	{ RenderMode::TRIANGLE_STRIP, GL_TRIANGLE_STRIP },
	{ RenderMode::RECTANGLES, GL_TRIANGLE_STRIP },
	{ RenderMode::LINES, GL_LINES },
	{ RenderMode::LINE_LOOP, GL_LINE_LOOP }
};

COpenGLESRenderer::COpenGLESRenderer()
	:m_textureManager(nullptr), m_version(3)
{
	glDepthFunc(GL_LESS);
	glEnable(GL_TEXTURE_2D);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	m_viewMatrices.push_back(Matrix4F());
	m_projectionMatrices.push_back(Matrix4F());
}

void COpenGLESRenderer::RenderArrays(RenderMode mode, std::vector<CVector3f> const& vertices, std::vector<CVector3f> const& normals, std::vector<CVector2f> const& texCoords)
{
	m_shaderManager->SetVertexAttribute("Position", 3, vertices.size(), (float*)&vertices[0].x);
	m_shaderManager->SetVertexAttribute("Normal", 3, normals.size(), (float*)&normals[0].x);
	m_shaderManager->SetVertexAttribute("TexCoord", 2, texCoords.size(), (float*)&texCoords[0].x);
	glDrawArrays(renderModeMap.at(mode), 0, vertices.size());
	float def[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_shaderManager->DisableVertexAttribute("Position", 3, def);
	m_shaderManager->DisableVertexAttribute("Normal", 3, def);
	m_shaderManager->DisableVertexAttribute("TexCoord", 2, def);
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

void COpenGLESRenderer::RenderArrays(RenderMode mode, std::vector<CVector3d> const& vertices, std::vector<CVector3d> const& normals, std::vector<CVector2d> const& texCoords)
{
	auto vertexf = TransformDoubleToFloat(vertices.data(), vertices.size() * 3);
	auto normalsf = TransformDoubleToFloat(normals.data(), normals.size() * 3);
	auto texCoordsf = TransformDoubleToFloat(texCoords.data(), texCoords.size() * 2);
	m_shaderManager->SetVertexAttribute("Position", 3, vertices.size(), vertexf.data());
	m_shaderManager->SetVertexAttribute("Normal", 3, normals.size(), normalsf.data());
	m_shaderManager->SetVertexAttribute("TexCoord", 2, texCoords.size(), texCoordsf.data());
	glDrawArrays(renderModeMap.at(mode), 0, vertices.size());
	float def[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_shaderManager->DisableVertexAttribute("Position", 3, def);
	m_shaderManager->DisableVertexAttribute("Normal", 3, def);
	m_shaderManager->DisableVertexAttribute("TexCoord", 2, def);
}

void COpenGLESRenderer::RenderArrays(RenderMode mode, std::vector<CVector2f> const& vertices, std::vector<CVector2f> const& texCoords)
{
	m_shaderManager->SetVertexAttribute("Position", 2, vertices.size(), (int*)&vertices[0].x);
	m_shaderManager->SetVertexAttribute("TexCoord", 2, texCoords.size(), (float*)&texCoords[0].x);
	glDrawArrays(renderModeMap.at(mode), 0, vertices.size());
	float def[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_shaderManager->DisableVertexAttribute("Position", 3, def);
	m_shaderManager->DisableVertexAttribute("TexCoord", 2, def);
}

void COpenGLESRenderer::RenderArrays(RenderMode mode, std::vector<CVector2i> const& vertices, std::vector<CVector2f> const& texCoords)
{
	m_shaderManager->SetVertexAttribute("Position", 2, vertices.size(), (int*)&vertices[0].x);
	m_shaderManager->SetVertexAttribute("TexCoord", 2, texCoords.size(), (float*)&texCoords[0].x);
	glDrawArrays(renderModeMap.at(mode), 0, vertices.size());
	float def[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_shaderManager->DisableVertexAttribute("Position", 3, def);
	m_shaderManager->DisableVertexAttribute("TexCoord", 2, def);
}

void COpenGLESRenderer::PushMatrix()
{
	m_viewMatrices.push_back(m_viewMatrices.back());
	UpdateUniforms();
}

void COpenGLESRenderer::PopMatrix()
{
	m_viewMatrices.pop_back();
	UpdateUniforms();
}

void COpenGLESRenderer::Translate(const int dx, const int dy, const int dz)
{
	m_viewMatrices.back().Translate(static_cast<float>(dx), static_cast<float>(dy), static_cast<float>(dz));
	UpdateUniforms();
}

void COpenGLESRenderer::Translate(const double dx, const double dy, const double dz)
{
	m_viewMatrices.back().Translate(static_cast<float>(dx), static_cast<float>(dy), static_cast<float>(dz));
	UpdateUniforms();
}

void COpenGLESRenderer::Translate(const float dx, const float dy, const float dz)
{
	m_viewMatrices.back().Translate(dx, dy, dz);
	UpdateUniforms();
}

void COpenGLESRenderer::Scale(double scale)
{
	m_viewMatrices.back().Scale(static_cast<float>(scale));
	UpdateUniforms();
}

void COpenGLESRenderer::Rotate(double angle, double x, double y, double z)
{
	//m_viewMatrices.back().Rotate(static_cast<float>(angle), static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
	UpdateUniforms();
}

void COpenGLESRenderer::GetViewMatrix(float * matrix) const
{
	memcpy(matrix, m_viewMatrices.back(), sizeof(Matrix4F));
}

void COpenGLESRenderer::ResetViewMatrix()
{
	m_viewMatrices.back() = Matrix4F();
	UpdateUniforms();
}

void COpenGLESRenderer::LookAt(CVector3d const& position, CVector3d const& direction, CVector3d const& up)
{
	m_viewMatrices.back() = Matrix4F();
	//gluLookAt(position[0], position[1], position[2], direction[0], direction[1], direction[2], up[0], up[1], up[2]);
	UpdateUniforms();
}

void COpenGLESRenderer::SetColor(const float r, const float g, const float b)
{
	m_color = { r, g, b, 1.0f };
}

float ToFloat(int color)
{
	return static_cast<float>(color) / 255.0f;
}

void COpenGLESRenderer::SetColor(const int r, const int g, const int b)
{
	m_color = { ToFloat(r), ToFloat(g), ToFloat(b), 1.0f };
}

void COpenGLESRenderer::SetColor(const float * color)
{
	m_color = { color[0], color[1], color[2], 1.0f };
}

void COpenGLESRenderer::SetColor(const int * color)
{
	m_color = { ToFloat(color[0]), ToFloat(color[1]), ToFloat(color[2]), 1.0f };
}

Matrix4F MakeOrhto(float left, float right, float bottom, float top, float near, float far)
{
	float a = 2.0f / (right - left);
	float b = 2.0f / (top - bottom);
	float c = -2.0f / (far - near);

	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	float tz = -(far + near) / (far - near);

	float ortho[16] = {
		a, 0, 0, 0,
		0, b, 0, 0,
		0, 0, c, 0,
		tx, ty, tz, 1
	};
	return ortho;
}

std::unique_ptr<ICachedTexture> COpenGLESRenderer::RenderToTexture(std::function<void() > const& func, unsigned int width, unsigned int height)
{
	//set up texture
	auto texture = std::make_unique<COpenGlCachedTexture>();
	texture->Bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	UnbindTexture();
	//set up buffer
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
	m_projectionMatrices.push_back(MakeOrhto(0, width, 0, height, -1, 1));
	m_viewMatrices.push_back(Matrix4F());

	glClear(GL_COLOR_BUFFER_BIT);
	func();

	m_projectionMatrices.pop_back();
	m_viewMatrices.pop_back();
	UpdateUniforms();
	glViewport(oldViewport[0], oldViewport[1], oldViewport[2], oldViewport[3]);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

std::unique_ptr<IDrawingList> COpenGLESRenderer::CreateDrawingList(std::function<void() > const& func)
{
	return std::make_unique<COpenGLDrawingList>(func);
}

std::unique_ptr<IVertexBuffer> COpenGLESRenderer::CreateVertexBuffer(const float * vertex /*= nullptr*/, const float * normals /*= nullptr*/, const float * texcoords /*= nullptr*/, size_t /*size*/)
{
	return std::make_unique<COpenGLVertexBuffer>(vertex, normals, texcoords);
}

std::unique_ptr<IFrameBuffer> COpenGLESRenderer::CreateFramebuffer() const
{
	return std::make_unique<COpenGLESFrameBuffer>();
}

std::unique_ptr<IShaderManager> COpenGLESRenderer::CreateShaderManager() const
{
	auto shaderManager = std::make_unique<CShaderManagerOpenGLES>(this);
	m_shaderManager = shaderManager.get();
	return std::move(shaderManager);
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

COpenGLVertexBuffer::COpenGLVertexBuffer(const float * vertex /*= nullptr*/, const float * normals /*= nullptr*/, const float * texcoords /*= nullptr*/)
	:m_vertex(vertex), m_normals(normals), m_texCoords(texcoords)
{
}

COpenGLVertexBuffer::~COpenGLVertexBuffer()
{
	UnBind();
}

void COpenGLVertexBuffer::Bind() const
{
	if (m_vertex)
	{
		glEnableVertexAttribArray(positionIndex);
		glVertexAttribPointer(positionIndex, 3, GL_FLOAT, GL_FALSE, 0, m_vertex);
	}
	if (m_normals)
	{
		glEnableVertexAttribArray(normalIndex);
		glVertexAttribPointer(normalIndex, 3, GL_FLOAT, GL_FALSE, 0, m_normals);
	}
	if (m_texCoords)
	{
		glEnableVertexAttribArray(texCoordIndex);
		glVertexAttribPointer(texCoordIndex, 2, GL_FLOAT, GL_FALSE, 0, m_texCoords);
	}
}

void COpenGLVertexBuffer::DrawIndexes(unsigned int * indexPtr, size_t count)
{
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, indexPtr);
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
	glDisableVertexAttribArray(texCoordIndex);
	glDisableVertexAttribArray(normalIndex);
	glDisableVertexAttribArray(positionIndex);
}

void COpenGLESRenderer::WindowCoordsToWorldVector(IViewport & viewport, int x, int y, CVector3d & start, CVector3d & end) const
{
	int viewportData[4] = { viewport.GetX(), viewport.GetY(), viewport.GetWidth(), viewport.GetHeight() };
	//Set OpenGL Windows coordinates
	double winX = (double)x;
	double winY = viewportData[3] - (double)y;

	//Cast a ray from eye to mouse cursor;
	gluUnProject(winX, winY, 0.0, viewport.GetViewMatrix(), viewport.GetProjectionMatrix(),
		viewportData, &start.x, &start.y, &start.z);
	gluUnProject(winX, winY, 1.0, viewport.GetViewMatrix(), viewport.GetProjectionMatrix(),
		viewportData, &end.x, &end.y, &end.z);
}

void COpenGLESRenderer::WorldCoordsToWindowCoords(IViewport & viewport, CVector3d const& worldCoords, int& x, int& y) const
{
	int viewportData[4] = { viewport.GetX(), viewport.GetY(), viewport.GetWidth(), viewport.GetHeight() };
	CVector3d windowPos;
	gluProject(worldCoords.x, worldCoords.y, worldCoords.z, viewport.GetViewMatrix(), viewport.GetProjectionMatrix(), viewportData, &windowPos.x, &windowPos.y, &windowPos.z);
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

void COpenGLESRenderer::EnableVertexLightning(bool enable)
{
	/*glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, enable ? GL_MODULATE : GL_REPLACE);
	if (enable)
	{
		glEnable(GL_LIGHTING);
		glEnable(GL_NORMALIZE);
	}
	else
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_NORMALIZE);
	}*/
}

void COpenGLESRenderer::GetProjectionMatrix(float * matrix) const
{
	memcpy(matrix, m_projectionMatrices.back(), sizeof(Matrix4F));
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

void COpenGLESRenderer::SetUpViewport(unsigned int viewportX, unsigned int viewportY, unsigned int viewportWidth, unsigned int viewportHeight, double viewingAngle, double nearPane, double farPane)
{
	double aspect = (double)viewportWidth / (double)viewportHeight;
	gluPerspective(viewingAngle, aspect, nearPane, farPane);
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

void COpenGLESRenderer::UploadTexture(ICachedTexture & texture, unsigned char * data, unsigned int width, unsigned int height, unsigned short bpp, int flags, TextureMipMaps const& mipmaps)
{
	texture.Bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (flags & TextureFlags::TEXTURE_NO_WRAP) ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (flags & TextureFlags::TEXTURE_NO_WRAP) ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (flags & TEXTURE_BUILD_MIPMAPS || !mipmaps.empty()) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	GLenum format = (flags & TEXTURE_BGRA) ? GL_BGRA_EXT : ((flags & TEXTURE_HAS_ALPHA) ? GL_RGBA : GL_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, bpp / 8, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	for (size_t i = 0; i < mipmaps.size(); i++)
	{
		auto& mipmap = mipmaps[i];
		glTexImage2D(GL_TEXTURE_2D, i + 1, bpp / 8, mipmap.width, mipmap.height, 0, format, GL_UNSIGNED_BYTE, mipmap.data);
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

std::string COpenGLESRenderer::GetName() const
{
	return "OpenGLES";
}

void COpenGLESRenderer::SetVersion(int version)
{
	m_version = version;
}

void COpenGLESRenderer::Init()
{
	glDepthFunc(GL_LESS);
	glEnable(GL_TEXTURE_2D);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void COpenGLESRenderer::BindShaderManager(const CShaderManagerOpenGLES * shaderManager) const
{
	m_shaderManager = shaderManager;
	UpdateUniforms();
}

bool COpenGLESRenderer::SupportsFeature(Feature /*feature*/) const
{
	return true;
}

void COpenGLESRenderer::UpdateUniforms() const
{
	Matrix4F m = m_projectionMatrices.back();
	m *= m_viewMatrices.back();
	m_shaderManager->SetUniformMatrix4("mvp_matrix", 1, m);
	m_shaderManager->SetUniformValue4("color", 1, m_color.data());
}

void COpenGLESRenderer::DrawIn2D(std::function<void()> const& drawHandler)
{
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glEnable(GL_BLEND);
	m_projectionMatrices.push_back(MakeOrhto(viewport[0], viewport[2], viewport[3], viewport[1], 1.0f, -1.0f));
	m_viewMatrices.push_back(Matrix4F());
	UpdateUniforms();

	drawHandler();

	m_viewMatrices.pop_back();
	m_projectionMatrices.pop_back();
	UpdateUniforms();
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
	/*if (type == CachedTextureType::DEPTH)
	{
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}*/
	glFramebufferTexture2D(GL_FRAMEBUFFER, typeMap.at(type), GL_TEXTURE_2D, (COpenGlCachedTexture&)texture, 0);
	auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		throw std::runtime_error("Error creating framebuffer");
	}
}