#include "UICheckBox.h"
#include "UIText.h"

CUICheckBox::CUICheckBox(int x, int y, int height, int width, std::wstring const& text, bool initState, IUIElement * parent, IRenderer & renderer) :
	CUIElement(x, y, height, width, parent, renderer), m_text(text), m_state(initState), m_pressed(false)
{

}

void CUICheckBox::Draw() const
{
	if(!m_visible)
		return;
	m_renderer.PushMatrix();
	m_renderer.Translate(GetX(), GetY(), 0);
	if (!m_cache)
	{
		m_cache = move(m_renderer.RenderToTexture([this]() {
			m_renderer.SetTexture(m_theme->texture, true);
			float * texCoords = m_state ? m_theme->checkbox.checkedTexCoord : m_theme->checkbox.texCoord;
			float size = GetHeight() * m_theme->checkbox.checkboxSizeCoeff;
			m_renderer.RenderArrays(RenderMode::RECTANGLES,
			{ CVector2f(0.0f, 0.0f), {0.0f, size}, {size, size}, {size, 0.0f} },
			{ CVector2f(texCoords), {texCoords[0], texCoords[3]}, {texCoords[2], texCoords[3]}, {texCoords[2], texCoords[1]} });
			PrintText(m_renderer, static_cast<int>(size) + 1, 0, GetWidth(), GetHeight(), m_text, m_theme->text);
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

bool CUICheckBox::LeftMouseButtonUp(int x, int y)
{
	if(!m_visible) return false;
	Invalidate();
	if(CUIElement::LeftMouseButtonUp(x, y))
	{
		m_pressed = false;
		return true;
	}
	if(m_pressed && PointIsOnElement(x, y))
	{
		m_state = !m_state;
		m_pressed = false;
		return true;
	}
	m_pressed = false;
	return false;
}

bool CUICheckBox::LeftMouseButtonDown(int x, int y)
{
	if(!m_visible) return false;
	Invalidate();
	if (CUIElement::LeftMouseButtonDown(x, y))
	{
		return true;
	}
	if(PointIsOnElement(x, y))
	{
		m_pressed = true;
		return true;
	}
	return false;
}

void CUICheckBox::SetState(bool state)
{
	m_state = state;
	Invalidate();
}

bool CUICheckBox::GetState() const
{ 
	return m_state; 
}

std::wstring const CUICheckBox::GetText() const
{ 
	return m_text; 
}

void CUICheckBox::SetText(std::wstring const& text)
{ 
	m_text = text; 
	Invalidate();
}