#include "UIStaticText.h"
#include "UIUtils.h"
#include <GL\glut.h>

void CUIStaticText::Draw() const
{
	if(!m_visible)
		return;
	glPushMatrix();
	glTranslatef(m_x, m_y, 0);
	m_text.Draw();
	CUIElement::Draw();
	glPopMatrix();
}
