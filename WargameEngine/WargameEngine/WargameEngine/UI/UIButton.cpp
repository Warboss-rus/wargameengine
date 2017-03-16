#include "UIButton.h"
#include "UIText.h"
#include "../view/IRenderer.h"

CUIButton::CUIButton(int x, int y, int height, int width, std::wstring const& text, std::function<void()> const& onClick, IUIElement * parent, IRenderer & renderer, ITextWriter & textWriter) :
	CUIElement(x, y, height, width, parent, renderer, textWriter), m_text(text), m_onClick(onClick), m_isPressed(false)
{
}

void CUIButton::Draw() const
{
	if(!m_visible)
		return;
	m_renderer.PushMatrix();
	m_renderer.Translate(GetX(), GetY(), 0);
	if (!m_cache)
	{
		m_cache = m_renderer.CreateTexture(nullptr, GetWidth(), GetHeight());
	}
	if(m_invalidated)
	{
		m_renderer.RenderToTexture([this]() {
			if (m_backgroundImage.empty())
			{
				m_renderer.SetTexture(m_theme->texture, true);
				float * texCoord = m_isPressed ? m_theme->button.pressedTexCoord : m_theme->button.texCoord;
				m_renderer.RenderArrays(RenderMode::TRIANGLE_STRIP,
				{ CVector2i(0, 0),{ GetWidth(), 0 },{ 0, GetHeight() },{ GetWidth(), GetHeight() } },
				{ CVector2f(texCoord),{ texCoord[2], texCoord[1] },{ texCoord[0], texCoord[3] },{ texCoord[2], texCoord[3] } });
			}
			else
			{
				m_renderer.SetTexture(m_theme->texture, true);
				m_renderer.RenderArrays(RenderMode::TRIANGLE_STRIP,
				{ CVector2i(0, 0),{ GetWidth(), 0 },{ 0, GetHeight() },{ GetWidth(), GetHeight() } },
				{ CVector2f(0.0f, 0.0f),{ 1.0f, 0.0f },{ 0.0f, 1.0f },{ 1.0f, 1.0f } });
			}
			PrintText(m_renderer, m_textWriter, 0, 0, GetWidth(), GetHeight(), m_text, m_theme->button.text, m_scale);
		}, *m_cache, GetWidth(), GetHeight());
	}
	m_renderer.SetTexture(*m_cache);
	m_renderer.RenderArrays(RenderMode::TRIANGLE_STRIP,
		{ CVector2i(0, 0),{ GetWidth(), 0 },{ 0, GetHeight() },{ GetWidth(), GetHeight() } },
		{ CVector2f(0.0f, 0.0f),{ 1.0f, 0.0f },{ 0.0f, 1.0f },{ 1.0f, 1.0f } });
	m_renderer.UnbindTexture();
	CUIElement::Draw();
	m_renderer.PopMatrix();
}

bool CUIButton::LeftMouseButtonUp(int x, int y)
{
	if(!m_visible) return false;
	Invalidate();
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
	Invalidate();
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

std::wstring const CUIButton::GetText() const
{ 
	return m_text; 
}

void CUIButton::SetText(std::wstring const& text)
{ 
	m_text = text; 
	Invalidate();
}

void CUIButton::SetOnClickCallback(std::function<void()> const& onClick)
{ 
	m_onClick = onClick; 
}

void CUIButton::SetBackgroundImage(std::string const& image)
{ 
	m_backgroundImage = image; 
	Invalidate();
}