#include "UIStaticText.h"
#include "../view/gl.h"
#include "UIText.h"

void glTranslatei(int x, int y, int z)
{
	glTranslated(static_cast<double>(x), static_cast<double>(y), static_cast<double>(z));
}

void CUIStaticText::Draw() const
{
	if(!m_visible)
		return;
	glPushMatrix();
	glTranslatei(GetX(), GetY(), 0);
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