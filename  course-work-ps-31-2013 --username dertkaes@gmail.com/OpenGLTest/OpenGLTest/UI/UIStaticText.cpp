#include "UIStaticText.h"
#include "UIUtils.h"
#include <GL\glut.h>
#include "UIConfig.h"

void CUIStaticText::Draw() const
{
	if(!m_visible)
		return;
	glPushMatrix();
	glTranslatef(m_x, m_y, 0);
	glColor3f(CUIConfig::textColor[0], CUIConfig::textColor[1], CUIConfig::textColor[2]);
	int fontx = (m_width - glutBitmapLength(CUIConfig::font, (unsigned char*)m_text.c_str())) / 2 ;
	int fonty = (m_height + CUIConfig::fontHeight) / 2;
	PrintText(fontx, fonty, m_text.c_str(), CUIConfig::font);
	CUIElement::Draw();
	glPopMatrix();
}
