#include "OpenGLRenderer.h"
#include <GL\glew.h>
#include "gl.h"
#include "..\LogWriter.h"
#include "TextureManager.h"

using namespace std;

void COpenGLRenderer::SetTexture(std::string const& texture, bool forceLoadNow, int flags)
{
	auto texMan = CTextureManager::GetInstance();
	if (forceLoadNow)
	{
		texMan->LoadTextureNow(texture, nullptr, flags);
	}
	texMan->SetTexture(texture, nullptr, flags);
}

static const map<RenderMode, GLenum> renderModeMap = {
	{ RenderMode::TRIANGLES, GL_TRIANGLES },
	{ RenderMode::TRIANGLE_STRIP, GL_TRIANGLE_STRIP },
	{ RenderMode::RECTANGLES, GL_QUADS },
	{ RenderMode::LINES, GL_LINES }
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

void COpenGLRenderer::Translate(int dx, int dy, int dz)
{
	glTranslated(static_cast<double>(dx), static_cast<double>(dy), static_cast<double>(dz));
}

void COpenGLRenderer::Translate(double dx, double dy, double dz)
{
	glTranslated(dx, dy, dz);
}

void COpenGLRenderer::Translate(float dx, float dy, float dz)
{
	glTranslatef(dx, dy, dz);
}

void COpenGLRenderer::SetColor(float r, float g, float b)
{
	glColor3f(r, g, b);
}

void COpenGLRenderer::SetColor(int r, int g, int b)
{
	glColor3i(r, g, b);
}

void COpenGLRenderer::SetColor(float * color)
{
	glColor3fv(color);
}

void COpenGLRenderer::SetColor(int * color)
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

std::unique_ptr<ICachedTexture> COpenGLRenderer::CreateTexture(void * data, unsigned int width, unsigned int height, CachedTextureType type)
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

void COpenGLRenderer::GetViewMatrix(float * matrix) const
{
	glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
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
