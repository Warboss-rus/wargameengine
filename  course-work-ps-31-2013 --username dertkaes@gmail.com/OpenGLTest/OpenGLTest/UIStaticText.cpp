#include "UIStaticText.h"
#include "UIUtils.h"
#include <GL\glut.h>

void CUIStaticText::Draw() const
{
	CUIElement::Draw();
	glColor3f(0.0f,0.0f,0.0f);
	int fontx = m_x + (m_width - glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, (unsigned char*)m_text.c_str())) / 2 ;
	int fonty = m_y + (m_height + 20) / 2;
	PrintText(fontx, fonty, m_text.c_str(), GLUT_BITMAP_TIMES_ROMAN_24);
}
