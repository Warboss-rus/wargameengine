#include "UIWindow.h"

namespace wargameEngine
{
namespace ui
{
CUIWindow::CUIWindow(int width, int height, std::wstring const& headerText, IUIElement* parent)
	: UICachedElement(parent->GetWidth() / 2, parent->GetHeight() / 2, height, width, parent)
	, m_headerText(headerText)
	, m_dragging(false)
{
	m_x = (600 - m_width) / 2;
	m_y = (600 - m_height) / 2;
}

void CUIWindow::DoPaint(IUIRenderer& renderer) const
{
	auto& theme = m_theme->window;
	int headerHeight = static_cast<int>(theme.headerHeight * m_scale);
	renderer.DrawRect({ 0, 0, GetWidth(), headerHeight }, theme.headerColor);
	int buttonSize = static_cast<int>(theme.buttonSize * m_scale);
	renderer.DrawTexturedRect({ GetWidth() - buttonSize, 0, GetWidth(), buttonSize }, theme.closeButtonTexCoord, m_theme->texture);
	renderer.DrawRect({ 0, headerHeight, GetWidth(), GetHeight() }, m_theme->defaultColor);
	renderer.DrawText({ 0, 0, GetWidth() - buttonSize, headerHeight }, m_headerText, theme.headerText, m_scale);
}

bool CUIWindow::LeftMouseButtonDown(int x, int y)
{
	if (!m_visible || !PointIsOnElement(x, y))
		return UIElement::LeftMouseButtonDown(x, y - m_theme->window.headerHeight);
	int relativeY = y - m_theme->window.headerHeight;
	if (relativeY - GetY() < 0)
	{
		m_dragging = true;
		m_prevX = x;
		m_prevY = y;
		m_parent->SetFocus(this);
	}
	else
	{
		UIElement::LeftMouseButtonDown(x, relativeY);
	}
	return true;
}

bool CUIWindow::LeftMouseButtonUp(int x, int y)
{
	auto& theme = m_theme->window;
	if (!m_visible || !PointIsOnElement(x, y))
		return UIElement::LeftMouseButtonUp(x, y - theme.headerHeight);
	int relativeY = y - theme.headerHeight;
	if (relativeY - GetY() < 0)
	{
		if (GetWidth() - (x - GetX()) < theme.buttonSize)
		{
			m_parent->SetFocus();
			m_parent->DeleteChild(this); //may cause problems
			return true;
		}
	}
	else
	{
		UIElement::LeftMouseButtonUp(x, relativeY);
	}
	m_dragging = false;
	return true;
}

void CUIWindow::OnMouseMove(int x, int y)
{
	if (m_dragging)
	{
		m_x += x - m_prevX;
		m_y += y - m_prevY;
		m_prevX = x;
		m_prevY = y;
		Invalidate();
	}
	UIElement::OnMouseMove(x, y);
}

int CUIWindow::GetHeight() const
{
	return UIElement::GetWidth() + m_theme->window.headerHeight * m_windowWidth / 600;
}

int CUIWindow::GetX() const
{
	return m_x;
}

int CUIWindow::GetY() const
{
	return m_y;
}
}
}