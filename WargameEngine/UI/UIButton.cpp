#include "UIButton.h"

namespace wargameEngine
{
namespace ui
{

UIButton::UIButton(int x, int y, int height, int width, std::wstring const& text, std::function<void()> const& onClick, IUIElement* parent)
	: UICachedElement(x, y, height, width, parent)
	, m_text(text)
	, m_onClick(onClick)
	, m_isPressed(false)
{
}

void UIButton::DoPaint(IUIRenderer& renderer) const
{
	if (m_backgroundImage.empty())
	{
		float* texCoord = m_isPressed ? m_theme->button.pressedTexCoord : m_theme->button.texCoord;
		renderer.DrawTexturedRect(RectI{ 0, 0, GetWidth(), GetHeight() }, texCoord, m_theme->texture);
	}
	else
	{
		renderer.DrawTexturedRect(RectI{ 0, 0, GetWidth(), GetHeight() }, RectF{ 0.0f, 0.0f, 1.0f, 1.0f }, m_backgroundImage);
	}
	renderer.DrawText(RectI{ 0, 0, GetWidth(), GetHeight() }, m_text, m_theme->button.text, m_scale);
}

bool UIButton::LeftMouseButtonUp(int x, int y)
{
	if (!m_visible)
		return false;
	Invalidate();
	if (UIElement::LeftMouseButtonUp(x, y))
	{
		m_isPressed = false;
		return true;
	}
	if (m_isPressed && PointIsOnElement(x, y))
	{
		m_onClick();
		SetFocus();
		m_isPressed = false;
		return true;
	}
	m_isPressed = false;
	return false;
}

bool UIButton::LeftMouseButtonDown(int x, int y)
{
	if (!m_visible)
		return false;
	Invalidate();
	if (UIElement::LeftMouseButtonDown(x, y))
	{
		return true;
	}
	if (PointIsOnElement(x, y))
	{
		m_isPressed = true;
		return true;
	}
	return false;
}

std::wstring const UIButton::GetText() const
{
	return m_text;
}

void UIButton::SetText(std::wstring const& text)
{
	m_text = text;
	Invalidate();
}

void UIButton::SetOnClickCallback(std::function<void()> const& onClick)
{
	m_onClick = onClick;
}

void UIButton::SetBackgroundImage(Path const& image)
{
	m_backgroundImage = image;
	Invalidate();
}
}
}