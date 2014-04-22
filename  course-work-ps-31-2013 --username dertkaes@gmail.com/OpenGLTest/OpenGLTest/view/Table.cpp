#include "Table.h"
#include <GL\glut.h>
#include "TextureManager.h"

void CTable::Draw() const
{
	float vert[] = {
		-m_width, -m_height,
		m_width, -m_height,
		-m_width, m_height,
		m_width, m_height,};
	float tex[] = {
		0.0f, 0.0f,
		m_width, 0.0f,
		0.0f, m_height,
		m_width, m_height };
	CTextureManager * texManager = CTextureManager::GetInstance();
	texManager->SetTexture(m_texture);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, vert);
	glTexCoordPointer(2, GL_FLOAT, 0, tex);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	texManager->SetTexture("");
}

bool CTable::isCoordsOnTable(double worldX, double worldY) const
{
	return (worldX < GetWidth() / 2 && worldX > -GetWidth() / 2 
	   && worldY < GetHeight() / 2 && worldY > -GetHeight() / 2);
}