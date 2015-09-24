#include "OpenGLRenderer.h"
#include <GL/glew.h>
#include "gl.h"
#include "../LogWriter.h"
#include "TextureManager.h"

using namespace std;

void COpenGLRenderer::SetTexture(std::string const& texture, bool forceLoadNow, int flags)
{
	if (forceLoadNow)
	{
		m_textureManager.LoadTextureNow(texture, nullptr, flags);
	}
	m_textureManager.SetTexture(texture, nullptr, flags);
}

void COpenGLRenderer::SetTexture(std::string const& texture, TextureSlot slot, int flags /*= 0*/)
{
	m_textureManager.SetTexture(texture, slot, flags);
}

void COpenGLRenderer::SetTexture(std::string const& texture, const std::vector<sTeamColor> * teamcolor /*= nullptr*/, int flags /*= 0*/)
{
	m_textureManager.SetTexture(texture, teamcolor, flags);
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
		{ CachedTextureType::ALPHA, GL_ALPHA }
	};
	auto texture = std::make_unique<COpenGlCachedTexture>();
	texture->Bind();
	glTexImage2D(GL_TEXTURE_2D, 0, typeMap.at(type), width, height, 0, typeMap.at(type), GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE_EXT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE_EXT);
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

std::unique_ptr<IVertexBuffer> COpenGLRenderer::CreateVertexBuffer(const float * vertex /*= nullptr*/, const float * normals /*= nullptr*/, const float * texcoords /*= nullptr*/)
{
	return std::make_unique<COpenGLVertexBuffer>(vertex, normals, texcoords);
}

CTextureManager & COpenGLRenderer::GetTextureManager()
{
	return m_textureManager;
}

CTextureManager const& COpenGLRenderer::GetTextureManager() const
{
	return m_textureManager;
}

void COpenGLRenderer::SetMaterial(const float * ambient, const float * diffuse, const float * specular, const float shininess)
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

void COpenGLRenderer::GetViewMatrix(float * matrix) const
{
	glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
}

void COpenGLRenderer::ResetViewMatrix()
{
	glLoadIdentity();
}

void COpenGLRenderer::Scale(double scale)
{
	glScaled(scale, scale, scale);
}

void COpenGLRenderer::Rotate(double angle, double x, double y, double z)
{
	glRotated(angle, x, y, z);
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
