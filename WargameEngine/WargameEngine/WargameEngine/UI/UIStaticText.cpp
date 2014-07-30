#include "UIStaticText.h"
#include <GL/glut.h>
#include "UIText.h"

void CUIStaticText::Draw() const
{
	if(!m_visible)
		return;
	glPushMatrix();
	glTranslatef(GetX(), GetY(), 0.0f);
	PrintText(0, 0, GetWidth(), GetHeight(), m_text, m_theme.text);
	CUIElement::Draw();
	glPopMatrix();
}
