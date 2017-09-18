#include "UICheckBox.h"

namespace wargameEngine
{
namespace ui
{
UICheckBox::UICheckBox(int x, int y, int height, int width, std::wstring const& text, bool initState, IUIElement* parent)
	: UICachedElement(x, y, height, width, parent)
	, m_text(text)
	, m_state(initState)
	, m_pressed(false)
{
}

void UICheckBox::DoPaint(IUIRenderer& renderer) const
{
	auto& theme = m_theme->checkbox;
	float* texCoords = m_state ? theme.checkedTexCoord : theme.texCoord;
	int size = static_cast<int>(GetHeight() * theme.checkboxSizeCoeff);
	renderer.DrawTexturedRect(RectI{ 0, 0, size, size }, texCoords, m_theme->texture);
	renderer.DrawText(RectI{ static_cast<int>(size) + 1, 0, GetWidth(), GetHeight() }, m_text, m_theme->text, m_scale);
}

bool UICheckBox::LeftMouseButtonUp(int x, int y)
{
	if (!m_visible)
		return false;
	Invalidate();
	if (UIElement::LeftMouseButtonUp(x, y))
	{
		m_pressed = false;
		return true;
	}
	if (m_pressed && PointIsOnElement(x, y))
	{
		m_state = !m_state;
		m_pressed = false;
		return true;
	}
	m_pressed = false;
	return false;
}

bool UICheckBox::LeftMouseButtonDown(int x, int y)
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
		m_pressed = true;
		return true;
	}
	return false;
}

void UICheckBox::SetState(bool state)
{
	m_state = state;
	Invalidate();
}

bool UICheckBox::GetState() const
{
	return m_state;
}

std::wstring const UICheckBox::GetText() const
{
	return m_text;
}

void UICheckBox::SetText(std::wstring const& text)
{
	m_text = text;
	Invalidate();
}
}
}