#include "UIPanel.h"
#include <GL\glut.h>

void CUIPanel::Draw() const
{
	if(!m_visible)
		return;
	glPushMatrix();
	glTranslatef(m_x, m_y, 0);
	glColor3f(0.6f,0.6f,0.6f);
	glBegin(GL_QUADS);
		glVertex2d(0, 0);
		glVertex2d(0, m_height);
		glVertex2d(m_width, m_height);
		glVertex2d(m_width, 0);
	glEnd();
	glColor3f(0.0f,0.0f,0.0f);
	CUIElement::Draw();
	glPopMatrix();
}