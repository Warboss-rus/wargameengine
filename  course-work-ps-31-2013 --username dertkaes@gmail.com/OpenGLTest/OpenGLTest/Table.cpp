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
	glVertex3f(-m_width, -m_height, 0.0);
	glTexCoord2f(-m_width, -m_height);
	glVertex3f(m_width, -m_height, 0.0);
	glTexCoord2f(m_width, -m_height);
	glVertex3f(m_width, m_height, 0.0);
	glTexCoord2f(m_width, m_height);
	glVertex3f(-m_width, m_height, 0.0);
	glTexCoord2f(-m_width, m_height);
	glEnd();
}