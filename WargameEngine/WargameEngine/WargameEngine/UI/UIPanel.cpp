#include "UIPanel.h"
#include "../view/gl.h"

void CUIPanel::Draw() const
{
	if(!m_visible)
		return;
	glPushMatrix();
	glTranslatef(GetX(), GetY(), 0.0f);
	glColor3f(0.6f,0.6f,0.6f);
	glBegin(GL_QUADS);
		glVertex2i(0, 0);
		glVertex2i(0, GetHeight());
		glVertex2i(GetWidth(), GetHeight());
		glVertex2i(GetWidth(), 0);
	glEnd();
	glColor3f(0.0f,0.0f,0.0f);
	CUIElement::Draw();
	glPopMatrix();
}