#include "Table.h"
#include <GL\glut.h>

void CTable::Draw()
{
	if(m_textureID == 0)
	{
		m_textureID = m_textureManager.GetTexture(m_texture);
	}
	glBindTexture(GL_TEXTURE_2D, m_textureID);
	glBegin(GL_TRIANGLES);
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