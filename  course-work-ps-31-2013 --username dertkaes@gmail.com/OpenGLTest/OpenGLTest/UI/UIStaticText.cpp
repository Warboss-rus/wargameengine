#include "UIStaticText.h"
#include "UIUtils.h"
#include <GL\glut.h>

void CUIStaticText::Draw() const
{
	glPushMatrix();
	glTranslatef(m_x, m_y, 0);
	glColor3f(0.0f,0.0f,0.0f);
	int fontx = (m_width - glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, (unsigned char*)m_text.c_str())) / 2 ;
	int fonty = (m_height + 20) / 2;
	PrintText(fontx, fonty, m_text.c_str(), GLUT_BITMAP_TIMES_ROMAN_24);
	CUIElement::Draw();
	glPopMatrix();
}
