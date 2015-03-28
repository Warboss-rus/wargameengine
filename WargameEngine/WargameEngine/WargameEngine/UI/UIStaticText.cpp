#include "UIStaticText.h"
#include "../view/gl.h"
#include "UIText.h"

void CUIStaticText::Draw() const
{
	if(!m_visible)
		return;
	glPushMatrix();
	glTranslatef(GetX(), GetY(), 0.0f);
	PrintText(0, 0, GetWidth(), GetHeight(), m_text, m_theme->text);
	CUIElement::Draw();
	glPopMatrix();
}

std::string const CUIStaticText::GetText() const
{ 
	return m_text; 
}

void CUIStaticText::SetText(std::string const& text)
{ 
	m_text = text; 
}