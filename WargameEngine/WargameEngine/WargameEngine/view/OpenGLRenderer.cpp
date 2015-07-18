#include "OpenGLRenderer.h"
#include "TextureManager.h"
#include "gl.h"

using namespace std;

void COpenGLRenderer::SetTexture(std::string const& texture)
{
	CTextureManager::GetInstance()->SetTexture(texture);
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

