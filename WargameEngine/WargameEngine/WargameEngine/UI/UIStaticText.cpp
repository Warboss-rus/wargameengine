#include "UIStaticText.h"
#include "UIText.h"

void CUIStaticText::Draw() const
{
	if(!m_visible)
		return;
	m_renderer.PushMatrix();
	m_renderer.Translate(GetX(), GetY(), 0);
	PrintText(0, 0, GetWidth(), GetHeight(), m_text, m_theme->text);
	CUIElement::Draw();
	m_renderer.PopMatrix();
}

std::string const CUIStaticText::GetText() const
{ 
	return m_text; 
}

void CUIStaticText::SetText(std::string const& text)
{ 
	m_text = text; 
}