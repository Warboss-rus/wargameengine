#include "OpenGLESRenderer.h"
#include <GLES2/gl2ext.h>
#include <GLES3/gl3.h>
#include "gluLite.h"
#include "../LogWriter.h"
#include "TextureManager.h"
#include "ShaderManagerOpenGLES.h"
#include "../Utils.h"
#include "Matrix4.h"
#include "IViewport.h"

using namespace std;

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
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.01f);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	glShadeModel(GL_SMOOTH);
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
}

void Bind(const void* vertices, const void* normals, const void* texCoords, GLenum vertexType, GLenum normalType, GLenum texCoordType, int vertexAxesCount)
{
	if (vertices)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(vertexAxesCount, vertexType, 0, vertices);
	}
	if (normals)
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(normalType, 0, normals);
	}
	if (texCoords)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, texCoordType, 0, texCoords);
	}
}

void Unbind()
{
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void COpenGLESRenderer::RenderArrays(RenderMode mode, std::vector<CVector3f> const& vertices, std::vector<CVector3f> const& normals, std::vector<CVector2f> const& texCoords)
{
	Bind(vertices.data(), normals.data(), texCoords.data(), GL_FLOAT, GL_FLOAT, GL_FLOAT, 3);
	glDrawArrays(renderModeMap.at(mode), 0, vertices.size());
	Unbind();
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
	Bind(vertexf.data(), normalsf.data(), texCoordsf.data(), GL_FLOAT, GL_FLOAT, GL_FLOAT, 3);
	glDrawArrays(renderModeMap.at(mode), 0, vertices.size());
	Unbind();
}

void COpenGLESRenderer::RenderArrays(RenderMode mode, std::vector<CVector2f> const& vertices, std::vector<CVector2f> const& texCoords)
{
	Bind(vertices.data(), NULL, texCoords.data(), GL_FLOAT, GL_FLOAT, GL_FLOAT, 2);
	glDrawArrays(renderModeMap.at(mode), 0, vertices.size());
	Unbind();
}

void COpenGLESRenderer::RenderArrays(RenderMode mode, std::vector<CVector2i> const& vertices, std::vector<CVector2f> const& texCoords)
{
	std::vector<GLshort> shortVertex;
	shortVertex.reserve(vertices.size() * 2);
	for (auto& v : vertices)
	{
		shortVertex.push_back(v.x);
		shortVertex.push_back(v.y);
	}
	Bind(shortVertex.data(), NULL, texCoords.data(), GL_SHORT, GL_SHORT, GL_FLOAT, 2);
	glDrawArrays(renderModeMap.at(mode), 0, vertices.size());
	Unbind();
}

void COpenGLESRenderer::PushMatrix()
{
	glPushMatrix();
}

void COpenGLESRenderer::PopMatrix()
{
	glPopMatrix();
}

void COpenGLESRenderer::Translate(const int dx, const int dy, const int dz)
{
	glTranslatef(static_cast<float>(dx), static_cast<float>(dy), static_cast<float>(dz));
}

void COpenGLESRenderer::Translate(const double dx, const double dy, const double dz)
{
	glTranslatef(static_cast<float>(dx), static_cast<float>(dy), static_cast<float>(dz));
}

void COpenGLESRenderer::Translate(const float dx, const float dy, const float dz)
{
	glTranslatef(dx, dy, dz);
}

void COpenGLESRenderer::Scale(double scale)
{
	float fscale = static_cast<float>(scale);
	glScalef(fscale, fscale, fscale);
}

void COpenGLESRenderer::Rotate(double angle, double x, double y, double z)
{
	glRotatef(static_cast<float>(angle), static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
}

void COpenGLESRenderer::GetViewMatrix(float * matrix) const
{
	glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
}

void COpenGLESRenderer::ResetViewMatrix()
{
	glLoadIdentity();
}

void COpenGLESRenderer::LookAt(CVector3d const& position, CVector3d const& direction, CVector3d const& up)
{
	gluLookAt(position[0], position[1], position[2], direction[0], direction[1], direction[2], up[0], up[1], up[2]);
}

void COpenGLESRenderer::SetColor(const float r, const float g, const float b)
{
	glColor4f(r, g, b, 1.0f);
}

float ToFloat(int color)
{
	return static_cast<float>(color) / 255.0f;
}

void COpenGLESRenderer::SetColor(const int r, const int g, const int b)
{
	glColor4f(ToFloat(r), ToFloat(g), ToFloat(b), 1.0f);
}

void COpenGLESRenderer::SetColor(const float * color)
{
	glColor4f(color[0], color[1], color[2], 1.0f);
}

void COpenGLESRenderer::SetColor(const int * color)
{
	glColor4f(ToFloat(color[0]), ToFloat(color[1]), ToFloat(color[2]), 1.0f);
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
	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glScalef(1.0f, -1.0f, 1.0f);
	glOrthof(0, width, height, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClear(GL_COLOR_BUFFER_BIT);
	func();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
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
	return std::make_unique<CShaderManagerOpenGLES>();
}

void COpenGLESRenderer::SetTextureManager(CTextureManager & textureManager)
{
	m_textureManager = &textureManager;
}

void COpenGLESRenderer::SetMaterial(const float * ambient, const float * diffuse, const float * specular, const float shininess)
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
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, m_vertex);
	}
	if (m_normals)
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, 0, m_normals);
	}
	if (m_texCoords)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, m_texCoords);
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

void COpenGLVertexBuffer::UnBind() const
{
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
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

void COpenGLESRenderer::SetLightColor(size_t index, LightningType type, float * values)
{
	glLightfv(GL_LIGHT0 + index, lightningTypesMap.at(type), values);
}

void COpenGLESRenderer::SetLightPosition(size_t index, float* pos)
{
	glLightfv(GL_LIGHT0 + index, GL_POSITION, pos);
}

float COpenGLESRenderer::GetMaximumAnisotropyLevel() const
{
	float aniso = 1.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
	return aniso;
}

void COpenGLESRenderer::EnableVertexLightning(bool enable)
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

void COpenGLESRenderer::GetProjectionMatrix(float * matrix) const
{
	glGetFloatv(GL_PROJECTION_MATRIX, matrix);
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
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	double aspect = (double)viewportWidth / (double)viewportHeight;
	gluPerspective(viewingAngle, aspect, nearPane, farPane);
	glMatrixMode(GL_MODELVIEW);
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

void COpenGLESRenderer::EnableMultisampling(bool enable)
{
	if (enable)
		glEnable(GL_MULTISAMPLE);
	else
		glDisable(GL_MULTISAMPLE);
	
}

void COpenGLESRenderer::SetVersion(int version)
{
	m_version = version;
}

void COpenGLESRenderer::DrawIn2D(std::function<void()> const& drawHandler)
{
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glEnable(GL_BLEND);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrthof(viewport[0], viewport[2], viewport[3], viewport[1], 1.0f, -1.0f);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	drawHandler();

	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
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
		//glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}
	glFramebufferTexture2D(GL_FRAMEBUFFER, typeMap.at(type), GL_TEXTURE_2D, (COpenGlCachedTexture&)texture, 0);
	auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		throw std::runtime_error("Error creating framebuffer");
	}
}