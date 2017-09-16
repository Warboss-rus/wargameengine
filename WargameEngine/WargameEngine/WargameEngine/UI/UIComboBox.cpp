#include "UIComboBox.h"

namespace wargameEngine
{
namespace ui
{
UIComboBox::UIComboBox(int x, int y, int height, int width, IUIElement* parent)
	: UICachedElement(x, y, height, width, parent)
	, m_selected(-1)
	, m_expanded(false)
	, m_pressed(false)
	, m_scrollbar(m_theme)
{
}

void UIComboBox::DoPaint(IUIRenderer& renderer) const
{
	auto& theme = m_theme->combobox;
	int elementSize = static_cast<int>(theme.elementSize * m_scale);
	renderer.DrawRect({ 0, 0, GetWidth(), GetHeaderHeight() }, m_theme->defaultColor);
	int borderSize = static_cast<int>(theme.borderSize * m_scale);
	renderer.DrawRect({ borderSize, borderSize, GetWidth() - borderSize, GetHeaderHeight() - borderSize }, m_theme->textfieldColor);

	if (m_selected >= 0)
	{
		renderer.DrawText({ borderSize, borderSize, GetWidth() - borderSize, GetHeaderHeight() - borderSize }, m_items[m_selected], theme.text, m_scale);
	}

	const float* texCoords = m_expanded ? theme.expandedTexCoord : theme.texCoord;
	int firstX = GetWidth() - static_cast<int>(GetHeaderHeight() * theme.buttonWidthCoeff);
	renderer.DrawTexturedRect({ firstX, 0, GetWidth(), GetHeaderHeight() }, texCoords, m_theme->texture);

	if (m_expanded)
	{
		ScopedTranslation translation(renderer, 0, GetHeaderHeight());
		renderer.DrawRect({ 0, 0, GetWidth(), elementSize * static_cast<int>(m_items.size()) }, m_theme->textfieldColor);

		for (size_t i = m_scrollbar.GetPosition() / elementSize; i < m_items.size(); ++i)
		{
			if (GetHeaderHeight() + elementSize * static_cast<int>(i) - m_scrollbar.GetPosition() > m_windowHeight)
				break;
			renderer.DrawText({ borderSize, elementSize * static_cast<int>(i) - m_scrollbar.GetPosition(), GetWidth() - borderSize, elementSize * static_cast<int>(i + 1) - m_scrollbar.GetPosition() }, m_items[i], theme.text, m_scale);
		}
		m_scrollbar.Draw(renderer);
	}
}

bool UIComboBox::LeftMouseButtonDown(int x, int y)
{
	if (!m_visible)
		return false;
	if (UIElement::LeftMouseButtonDown(x, y))
		return true;
	Invalidate();
	if (PointIsOnElement(x, y))
	{
		if (m_expanded)
		{
			if (m_scrollbar.LeftMouseButtonDown(x - GetX(), y - GetY() - GetHeaderHeight()))
				return true;
		}
		m_pressed = true;
		return true;
	}
	return false;
}

bool UIComboBox::LeftMouseButtonUp(int x, int y)
{
	if (!m_visible)
		return false;
	Invalidate(true);
	if (UIElement::LeftMouseButtonUp(x, y))
	{
		m_pressed = false;
		return true;
	}
	if (m_expanded && m_scrollbar.LeftMouseButtonUp(x - GetX(), y - GetY() - GetHeaderHeight()))
		return true;
	if (PointIsOnElement(x, y))
	{
		if (m_pressed)
		{
			if (m_expanded && PointIsOnElement(x, y))
			{
				int index = (y - GetHeaderHeight() - GetY() + m_scrollbar.GetPosition()) / static_cast<int>(m_theme->combobox.elementSize * m_scale);
				if (index >= 0)
					m_selected = index;
				if (m_onChange)
					m_onChange();
			}
			m_expanded = !m_expanded;
		}
		SetFocus();
		m_pressed = false;
		return true;
	}
	else
	{
		m_expanded = false;
	}
	m_pressed = false;
	return false;
}

void UIComboBox::AddItem(std::wstring const& str)
{
	m_items.push_back(str);
	if (m_selected == -1)
	{
		m_selected = 0;
	}
	int elementSize = static_cast<int>(m_theme->combobox.elementSize * m_scale);
	m_scrollbar.Update(m_windowHeight - GetX() - GetHeaderHeight(), elementSize * static_cast<int>(m_items.size() + 1), GetWidth(), elementSize);
	Invalidate(true);
}

std::wstring const UIComboBox::GetText() const
{
	return m_items[m_selected];
}

void UIComboBox::SetSelected(size_t index)
{
	m_selected = static_cast<int>(index);
	Invalidate();
}

bool UIComboBox::PointIsOnElement(int x, int y) const
{
	int height = GetHeight();
	if (x > GetX() && x < GetX() + GetWidth() && y > GetY() && y < GetY() + height)
		return true;
	return false;
}

int UIComboBox::GetHeaderHeight() const
{
	return UIElement::GetHeight();
}

void UIComboBox::DeleteItem(size_t index)
{
	m_items.erase(m_items.begin() + index);
	if (m_selected == static_cast<int>(index))
		m_selected--;
	if (m_selected == -1 && !m_items.empty())
		m_selected = 0;
	int elementSize = static_cast<int>(m_theme->combobox.elementSize * m_scale);
	m_scrollbar.Update(m_windowHeight - GetX() - GetHeight(), elementSize * static_cast<int>(m_items.size() + 1), GetWidth(), elementSize);
	Invalidate(true);
}

void UIComboBox::SetText(std::wstring const& text)
{
	Invalidate();
	for (size_t i = 0; i < m_items.size(); ++i)
	{
		if (m_items[i] == text)
		{
			m_selected = static_cast<int>(i);
			return;
		}
	}
}

void UIComboBox::Resize(int windowHeight, int windowWidth)
{
	UIElement::Resize(windowHeight, windowWidth);
	int elementSize = static_cast<int>(m_theme->combobox.elementSize * m_scale);
	m_scrollbar.Update(m_windowHeight - GetX() - GetHeight(), elementSize * static_cast<int>(m_items.size() + 1), GetWidth(), elementSize);
	Invalidate(true);
}

size_t UIComboBox::GetSelectedIndex() const
{
	return m_selected;
}

size_t UIComboBox::GetItemsCount() const
{
	return m_items.size();
}

std::wstring UIComboBox::GetItem(size_t index) const
{
	return m_items[index];
}

void UIComboBox::ClearItems()
{
	m_items.clear();
	m_selected = -1;
	Invalidate(true);
}

void UIComboBox::SetOnChangeCallback(std::function<void()> const& onChange)
{
	m_onChange = onChange;
}

void UIComboBox::SetTheme(std::shared_ptr<UITheme> const& theme)
{
	m_theme = theme;
	m_scrollbar = UIScrollBar(theme);
	Invalidate();
}

void UIComboBox::SetScale(float scale)
{
	UIElement::SetScale(scale);
	m_scrollbar.SetScale(scale);
}

int UIComboBox::GetHeight() const
{
	int realHeight = GetHeaderHeight();
	if (m_expanded)
		realHeight += static_cast<int>(m_theme->combobox.elementSize * m_items.size() * m_scale);
	return realHeight;
}

void UIComboBox::OnMouseMove(int x, int y)
{
	if (m_visible && m_focused)
		m_focused->OnMouseMove(x, y);
	if (m_scrollbar.OnMouseMove(x - GetX(), y - GetY() - GetHeaderHeight()))
	{
		Invalidate();
	}
}
}
}