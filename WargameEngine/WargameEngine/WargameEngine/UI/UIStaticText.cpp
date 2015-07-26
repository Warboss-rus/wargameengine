#include "UIStaticText.h"
#include "UIText.h"

CUIStaticText::CUIStaticText(int x, int y, int height, int width, std::wstring const& text, IUIElement * parent, IRenderer & renderer)
	: CUIElement(x, y, height, width, parent, renderer), m_text(text)
{
}

void CUIStaticText::Draw() const
{
	if(!m_visible)
		return;
	m_renderer.PushMatrix();
	m_renderer.Translate(GetX(), GetY(), 0);
	if (!m_cache)
	{
		m_cache = move(m_renderer.RenderToTexture([this]() {
			PrintText(0, 0, GetWidth(), GetHeight(), m_text, m_theme->text);
		}, GetWidth(), GetHeight()));
	}
	m_cache->Bind();
	m_renderer.RenderArrays(RenderMode::TRIANGLE_STRIP,
	{ CVector2i(0, 0),{ GetWidth(), 0 },{ 0, GetHeight() },{ GetWidth(), GetHeight() } },
	{ CVector2f(0.0f, 0.0f),{ 1.0f, 0.0f },{ 0.0f, 1.0f },{ 1.0f, 1.0f } });
	m_renderer.SetTexture("");
	CUIElement::Draw();
	m_renderer.PopMatrix();
}

std::wstring const CUIStaticText::GetText() const
{ 
	return m_text; 
}

void CUIStaticText::SetText(std::wstring const& text)
{ 
	m_text = text; 
}