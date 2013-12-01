#include "UIStaticText.h"
#include <GL\glut.h>
#include "UIText.h"

void CUIStaticText::Draw() const
{
	if(!m_visible)
		return;
	glPushMatrix();
	glTranslatef(m_x, m_y, 0);
	PrintText(0, 0, m_width, m_height, m_text, m_theme.text);
	CUIElement::Draw();
	glPopMatrix();
}
