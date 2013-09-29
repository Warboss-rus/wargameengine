#include "Table.h"
#include <GL\glut.h>

void CTable::Draw()
{
	glBegin(GL_QUADS);
	glVertex3d(-m_width, -m_height, 0.0);
	glVertex3d(m_width, -m_height, 0.0);
	glVertex3d(m_width, m_height, 0.0);
	glVertex3d(-m_width, m_height, 0.0);
	glEnd();
}