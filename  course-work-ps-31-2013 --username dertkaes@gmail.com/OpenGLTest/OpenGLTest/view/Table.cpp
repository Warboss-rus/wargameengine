#include "Table.h"
#include <GL\glut.h>
#include "TextureManager.h"

CTable::CTable(float width, float height, std::string const& texture) :m_height(height / 2), m_width(width / 2) 
{
	m_list = glGenLists(1);
	glNewList(m_list, GL_COMPILE);
	float vert[] = {
		-m_width, -m_height,
		m_width, -m_height,
		-m_width, m_height,
		m_width, m_height, };
	float tex[] = {
		0.0f, 0.0f,
		m_width, 0.0f,
		0.0f, m_height,
		m_width, m_height };
	CTextureManager * texManager = CTextureManager::GetInstance();
	texManager->SetTexture(texture);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, vert);
	glTexCoordPointer(2, GL_FLOAT, 0, tex);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	texManager->SetTexture("");
	glEndList();
}

void CTable::Draw() const
{
	glCallList(m_list);
}

bool CTable::isCoordsOnTable(double worldX, double worldY) const
{
	return (worldX < GetWidth() / 2 && worldX > -GetWidth() / 2 
	   && worldY < GetHeight() / 2 && worldY > -GetHeight() / 2);
}

CTable::~CTable()
{
	glDeleteLists(m_list, 1);
}