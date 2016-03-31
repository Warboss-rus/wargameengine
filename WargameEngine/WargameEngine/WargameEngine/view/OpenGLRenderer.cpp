#include "OpenGLRenderer.h"
#include <GL/glew.h>
#include "gl.h"
#include "../LogWriter.h"
#include "TextureManager.h"
#include "ShaderManagerOpenGL.h"

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
	COpenGLDrawingList(unsigned int id);
	~COpenGLDrawingList();

	virtual void Draw() const override;
private:
	unsigned int m_id;
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


void COpenGLRenderer::SetTexture(std::string const& texture, bool forceLoadNow, int flags)
{
	if (forceLoadNow)
	{
		m_textureManager->LoadTextureNow(texture, nullptr, flags);
	}
	m_textureManager->SetTexture(texture, nullptr, flags);
}

void COpenGLRenderer::SetTexture(std::string const& texture, TextureSlot slot, int flags /*= 0*/)
{
	m_textureManager->SetTexture(texture, slot, flags);
}

void COpenGLRenderer::SetTexture(std::string const& texture, const std::vector<sTeamColor> * teamcolor /*= nullptr*/, int flags /*= 0*/)
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

void RenderVertex(CVector3f const& vector)
{
	glVertex3f(vector.x, vector.y, vector.z);
}

void RenderVertex(CVector3d const& vector)
{
	glVertex3d(vector.x, vector.y, vector.z);
}

void RenderVertex(CVector3<int> const& vector)
{
	glVertex3i(vector.x, vector.y, vector.z);
}

void RenderVertex(CVector2f const& vector)
{
	glVertex2f(vector.x, vector.y);
}

void RenderVertex(CVector2d const& vector)
{
	glVertex2d(vector.x, vector.y);
}

void RenderVertex(CVector2i const& vector)
{
	glVertex2i(vector.x, vector.y);
}

void RenderNormal(CVector3f const& vector)
{
	glNormal3f(vector.x, vector.y, vector.z);
}

void RenderNormal(CVector3d const& vector)
{
	glNormal3d(vector.x, vector.y, vector.z);
}

void RenderNormal(CVector3i const& vector)
{
	glNormal3i(vector.x, vector.y, vector.z);
}

void RenderTexCoord(CVector2f const& vector)
{
	glTexCoord2f(vector.x, vector.y);
}

void RenderTexCoord(CVector2d const& vector)
{
	glTexCoord2d(vector.x, vector.y);
}

template<class X, class Y, class Z>
void Render(RenderMode mode, std::vector<X> const& vertices, std::vector<Y> const& normals, std::vector<Z> const& texCoords)
{
	glBegin(renderModeMap.at(mode));
	for (size_t i = 0; i < vertices.size(); ++i)
	{
		if (i < texCoords.size()) RenderTexCoord(texCoords[i]);
		if (i < normals.size()) RenderNormal(normals[i]);
		RenderVertex(vertices[i]);
	}
	glEnd();
}

template<class X, class Y>
void Render(RenderMode mode, std::vector<X> const& vertices, std::vector<Y> const& texCoords)
{
	glBegin(renderModeMap.at(mode));
	for (size_t i = 0; i < vertices.size(); ++i)
	{
		if (i < texCoords.size()) RenderTexCoord(texCoords[i]);
		RenderVertex(vertices[i]);
	}
	glEnd();
}

COpenGLRenderer::COpenGLRenderer()
	:m_textureManager(nullptr)
{
	glDepthFunc(GL_LESS);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.01f);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glewInit();
}

void COpenGLRenderer::RenderArrays(RenderMode mode, std::vector<CVector3f> const& vertices, std::vector<CVector3f> const& normals, std::vector<CVector2f> const& texCoords)
{
	Render(mode, vertices, normals, texCoords);
}

void COpenGLRenderer::RenderArrays(RenderMode mode, std::vector<CVector3d> const& vertices, std::vector<CVector3d> const& normals, std::vector<CVector2d> const& texCoords)
{
	Render(mode, vertices, normals, texCoords);
}

void COpenGLRenderer::RenderArrays(RenderMode mode, std::vector<CVector2f> const& vertices, std::vector<CVector2f> const& texCoords)
{
	Render(mode, vertices, texCoords);
}

void COpenGLRenderer::RenderArrays(RenderMode mode, std::vector<CVector2i> const& vertices, std::vector<CVector2f> const& texCoords)
{
	Render(mode, vertices, texCoords);
}

void COpenGLRenderer::PushMatrix()
{
	glPushMatrix();
}

void COpenGLRenderer::PopMatrix()
{
	glPopMatrix();
}

void COpenGLRenderer::Translate(const int dx, const int dy, const int dz)
{
	glTranslated(static_cast<double>(dx), static_cast<double>(dy), static_cast<double>(dz));
}

void COpenGLRenderer::Translate(const double dx, const double dy, const double dz)
{
	glTranslated(dx, dy, dz);
}

void COpenGLRenderer::Translate(const float dx, const float dy, const float dz)
{
	glTranslatef(dx, dy, dz);
}

void COpenGLRenderer::Scale(double scale)
{
	glScaled(scale, scale, scale);
}

void COpenGLRenderer::Rotate(double angle, double x, double y, double z)
{
	glRotated(angle, x, y, z);
}

void COpenGLRenderer::GetViewMatrix(float * matrix) const
{
	glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
}

void COpenGLRenderer::ResetViewMatrix()
{
	glLoadIdentity();
}

void COpenGLRenderer::LookAt(CVector3d const& position, CVector3d const& direction, CVector3d const& up)
{
	gluLookAt(position[0], position[1], position[2], direction[0], direction[1], direction[2], up[0], up[1], up[2]);
}

void COpenGLRenderer::SetColor(const float r, const float g, const float b)
{
	glColor3f(r, g, b);
}

void COpenGLRenderer::SetColor(const int r, const int g, const int b)
{
	glColor3i(r, g, b);
}

void COpenGLRenderer::SetColor(const float * color)
{
	glColor3fv(color);
}

void COpenGLRenderer::SetColor(const int * color)
{
	glColor3iv(color);
}

std::unique_ptr<ICachedTexture> COpenGLRenderer::RenderToTexture(std::function<void() > const& func, unsigned int width, unsigned int height)
{
	//set up texture
	auto texture = std::make_unique<COpenGlCachedTexture>();
	texture->Bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE_EXT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE_EXT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	SetTexture("");
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
	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, width, height);
	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glScalef(1.0f, -1.0f, 1.0f);
	glOrtho(0, width, height, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClear(GL_COLOR_BUFFER_BIT);
	func();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glPopAttrib();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &framebuffer);
	return move(texture);
}

std::unique_ptr<ICachedTexture> COpenGLRenderer::CreateTexture(const void * data, unsigned int width, unsigned int height, CachedTextureType type)
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE_EXT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE_EXT);
	if (type == CachedTextureType::DEPTH)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	}
	return move(texture);
}

std::unique_ptr<IDrawingList> COpenGLRenderer::CreateDrawingList(std::function<void() > const& func)
{
	unsigned int list = glGenLists(1);
	glNewList(list, GL_COMPILE);
	func();
	glEndList();
	return std::make_unique<COpenGLDrawingList>(list);
}

std::unique_ptr<IVertexBuffer> COpenGLRenderer::CreateVertexBuffer(const float * vertex /*= nullptr*/, const float * normals /*= nullptr*/, const float * texcoords /*= nullptr*/, size_t /*size*/)
{
	return std::make_unique<COpenGLVertexBuffer>(vertex, normals, texcoords);
}

std::unique_ptr<IFrameBuffer> COpenGLRenderer::CreateFramebuffer() const
{
	return std::make_unique<COpenGLFrameBuffer>();
}

std::unique_ptr<IShaderManager> COpenGLRenderer::CreateShaderManager() const
{
	return std::make_unique<CShaderManagerOpenGL>();
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

COpenGlCachedTexture::operator unsigned int()
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

void COpenGLRenderer::WindowCoordsToWorldVector(int x, int y, CVector3d & start, CVector3d & end) const
{
	//Get model, projection and viewport matrices
	double matModelView[16], matProjection[16];
	int viewport[4];
	glGetDoublev(GL_MODELVIEW_MATRIX, matModelView);
	glGetDoublev(GL_PROJECTION_MATRIX, matProjection);
	glGetIntegerv(GL_VIEWPORT, viewport);
	//Set OpenGL Windows coordinates
	double winX = (double)x;
	double winY = viewport[3] - (double)y;

	//Cast a ray from eye to mouse cursor;
	gluUnProject(winX, winY, 0.0, matModelView, matProjection,
		viewport, &start.x, &start.y, &start.z);
	gluUnProject(winX, winY, 1.0, matModelView, matProjection,
		viewport, &end.x, &end.y, &end.z);
}

void COpenGLRenderer::WorldCoordsToWindowCoords(CVector3d const& worldCoords, int& x, int& y) const
{
	double matModelView[16], matProjection[16];
	int viewport[4];
	glGetDoublev(GL_MODELVIEW_MATRIX, matModelView);
	glGetDoublev(GL_PROJECTION_MATRIX, matProjection);
	glGetIntegerv(GL_VIEWPORT, viewport);
	CVector3d windowPos;
	gluProject(worldCoords.x, worldCoords.y, worldCoords.z, matModelView, matProjection, viewport, &windowPos.x, &windowPos.y, &windowPos.z);
	x = static_cast<int>(windowPos.x);
	y = static_cast<int>(viewport[3] - windowPos.y);
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
	glGetFloatv(GL_PROJECTION_MATRIX, matrix);
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

void COpenGLRenderer::SetUpViewport(CVector3d const& position, CVector3d const& target, unsigned int viewportWidth, unsigned int viewportHeight, double viewingAngle, double nearPane, double farPane)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	GLdouble aspect = (GLdouble)viewportWidth / (GLdouble)viewportHeight;
	gluPerspective(viewingAngle, aspect, nearPane, farPane);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	LookAt(position, target, { 0.0, 1.0, 0.0 });
	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, viewportWidth, viewportHeight);
}

void COpenGLRenderer::RestoreViewport()
{
	glPopAttrib();
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
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

void COpenGLRenderer::UploadTexture(ICachedTexture & texture, unsigned char * data, unsigned int width, unsigned int height, unsigned short bpp, int flags, TextureMipMaps const& mipmaps)
{
	texture.Bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (flags & TextureFlags::TEXTURE_NO_WRAP) ? GL_CLAMP_TO_EDGE_EXT : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (flags & TextureFlags::TEXTURE_NO_WRAP) ? GL_CLAMP_TO_EDGE_EXT : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (flags & TEXTURE_BUILD_MIPMAPS || !mipmaps.empty()) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	GLenum format = (flags & TEXTURE_BGRA) ? ((flags & TEXTURE_HAS_ALPHA) ? GL_BGRA_EXT : GL_BGR_EXT) : ((flags & TEXTURE_HAS_ALPHA) ? GL_RGBA : GL_RGB);
	if (flags & TEXTURE_BUILD_MIPMAPS)
	{
		gluBuild2DMipmaps(GL_TEXTURE_2D, bpp / 8, width, height, format, GL_UNSIGNED_BYTE, data);
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, bpp / 8, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	}
	for (size_t i = 0; i < mipmaps.size(); i++)
	{
		auto& mipmap = mipmaps[i];
		glTexImage2D(GL_TEXTURE_2D, i + 1, bpp / 8, mipmap.width, mipmap.height, 0, format, GL_UNSIGNED_BYTE, mipmap.data);
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (flags & TextureFlags::TEXTURE_NO_WRAP) ? GL_CLAMP_TO_EDGE_EXT : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (flags & TextureFlags::TEXTURE_NO_WRAP) ? GL_CLAMP_TO_EDGE_EXT : GL_REPEAT);
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

std::string COpenGLRenderer::GetName() const
{
	return "OpenGL";
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

void COpenGLRenderer::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLdouble aspect = (GLdouble)width / (GLdouble)height;
	gluPerspective(60, aspect, 0.5, 1000.0);
	glMatrixMode(GL_MODELVIEW);
}

void COpenGLRenderer::SetUpViewport2D()
{
	glEnable(GL_BLEND);
	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glPushAttrib(GL_VIEWPORT_BIT);
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glOrtho(0, viewport[2] - viewport[0], viewport[3] - viewport[1], 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

COpenGLFrameBuffer::COpenGLFrameBuffer()
{
	if (!GLEW_EXT_framebuffer_object)
	{
		throw std::runtime_error("GL_EXT_framebuffer_object is not supported");
	}
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