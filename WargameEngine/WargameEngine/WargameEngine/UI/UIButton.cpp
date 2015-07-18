#include "UIButton.h"
#include "UIText.h"

void CUIButton::Draw() const
{
	if(!m_visible)
		return;
	m_renderer.PushMatrix();
	m_renderer.Translate(GetX(), GetY(), 0);
	if (m_backgroundImage.empty())
	{
		m_renderer.SetTexture(m_theme->texture);
		float * texCoord = m_isPressed ? m_theme->button.pressedTexCoord : m_theme->button.texCoord;
		m_renderer.RenderArrays(RenderMode::TRIANGLE_STRIP,
		{ CVector2i(0, 0), {GetWidth(), 0}, {0, GetHeight()}, {GetWidth(), GetHeight()} },
		{ CVector2f(texCoord), {texCoord[2], texCoord[1]}, {texCoord[0], texCoord[3]}, {texCoord[2], texCoord[3]} });
	}
	else
	{
		m_renderer.SetTexture(m_theme->texture);
		m_renderer.RenderArrays(RenderMode::TRIANGLE_STRIP,
		{ CVector2i(0, 0), {GetWidth(), 0 }, {0, GetHeight()}, {GetWidth(), GetHeight()} },
		{ CVector2f(0.0f, 0.0f), {1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f} });
	}
	PrintText(0, 0, GetWidth(), GetHeight(), m_text, m_theme->button.text);
	CUIElement::Draw();
	m_renderer.PopMatrix();
}

bool CUIButton::LeftMouseButtonUp(int x, int y)
{
	if(!m_visible) return false;
	if(CUIElement::LeftMouseButtonUp(x, y))
	{
		m_isPressed = false;
		return true;
	}
	if(m_isPressed && PointIsOnElement(x, y))
	{
		m_onClick();
		SetFocus();
		m_isPressed = false;
		return true;
	}
	m_isPressed = false;
	return false;
}

bool CUIButton::LeftMouseButtonDown(int x, int y)
{
	if(!m_visible) return false;
	if (CUIElement::LeftMouseButtonDown(x, y))
	{
		return true;
	}
	if(PointIsOnElement(x, y))
	{
		m_isPressed = true;
		return true;
	}
	return false;
}

std::string const CUIButton::GetText() const
{ 
	return m_text; 
}

void CUIButton::SetText(std::string const& text)
{ 
	m_text = text; 
}

void CUIButton::SetOnClickCallback(std::function<void()> const& onClick)
{ 
	m_onClick = onClick; 
}

void CUIButton::SetBackgroundImage(std::string const& image)
{ 
	m_backgroundImage = image; 
}