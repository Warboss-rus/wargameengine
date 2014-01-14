#include "Table.h"
#include "gl.h"
#include "TextureManager.h"

void CTable::Draw() const
{
	CTextureManager * texManager = CTextureManager::GetInstance();
	texManager->SetTexture(m_texture);
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-m_width, -m_height, 0.0);
	glTexCoord2f(m_width, 0.0);
	glVertex3f(m_width, -m_height, 0.0);
	glTexCoord2f(0.0, m_height);
	glVertex3f(-m_width, m_height, 0.0);
	glTexCoord2f(m_width, m_height);
	glVertex3f(m_width, m_height, 0.0);
	glEnd();
	texManager->SetTexture("");
}

bool CTable::isCoordsOnTable(float worldX, float worldY) const
{
	return (worldX < GetWidth() / 2 && worldX > -GetWidth() / 2 
	   && worldY < GetHeight() / 2 && worldY > -GetHeight() / 2);
}