#include "Table.h"
#include <GL\glut.h>
#include "TextureManager.h"

void CTable::Draw()
{
	if(m_textureID == 0)
	{
		CTextureManager * texManager = CTextureManager::GetInstance();
		m_textureID = texManager->GetTexture(m_texture);
	}
	glBindTexture(GL_TEXTURE_2D, m_textureID);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-m_width, -m_height, 0.0);
	glTexCoord2f(m_width, 0.0);
	glVertex3f(m_width, -m_height, 0.0);
	glTexCoord2f(m_width, m_height);
	glVertex3f(m_width, m_height, 0.0);
	glTexCoord2f(0.0, m_height);
	glVertex3f(-m_width, m_height, 0.0);
	glEnd();
}