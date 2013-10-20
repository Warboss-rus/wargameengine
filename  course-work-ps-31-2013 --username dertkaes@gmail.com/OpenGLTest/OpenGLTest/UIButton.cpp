#include "UIButton.h"
#include <GL\glut.h>
#include "UIUtils.h"

void CUIButton::Draw() const
{
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
	int fontx = (m_width - glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, (unsigned char*)m_text.c_str())) / 2 ;
	int fonty = (m_height + 20) / 2;
	PrintText(fontx, fonty, m_text.c_str(), GLUT_BITMAP_TIMES_ROMAN_24);
	CUIElement::Draw();
	glPopMatrix();
}

bool CUIButton::LeftMouseButtonUp(int x, int y)
{
	if(!CUIElement::LeftMouseButtonUp(x, y))
	{
		m_onClick();
	}
	return true;
}

bool CUIButton::LeftMouseButtonDown(int x, int y)
{
	CUIElement::LeftMouseButtonDown(x, y);
	return true;
}