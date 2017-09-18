#include "UIRadioGroup.h"

namespace wargameEngine
{
namespace ui
{
UIRadioGroup::UIRadioGroup(int x, int y, int height, int width, IUIElement* parent)
	: UICachedElement(x, y, height, width, parent)
	, m_selected(0)
{
}

void UIRadioGroup::DoPaint(IUIRenderer& renderer) const
{
	for (size_t i = 0; i < m_items.size(); ++i)
	{
		auto& theme = m_theme->radiogroup;
		int buttonSize = static_cast<int>(theme.buttonSize);
		int y = static_cast<int>(theme.elementSize * i + (theme.elementSize - buttonSize) / 2);
		const float* texCoord = (i == m_selected) ? theme.selectedTexCoord : theme.texCoord;
		renderer.DrawTexturedRect({ 0, y, buttonSize, y + buttonSize }, texCoord, m_theme->texture);
		int intSize = static_cast<int>(theme.elementSize);
		renderer.DrawText({ static_cast<int>(buttonSize) + 1, intSize * static_cast<int>(i), GetWidth(), intSize * static_cast<int>(i + 1) }, m_items[i], m_theme->text, m_scale);
	}
}

bool UIRadioGroup::LeftMouseButtonUp(int x, int y)
{
	if (!m_visible)
		return false;
	if (UIElement::LeftMouseButtonUp(x, y))
	{
		return true;
	}
	Invalidate();
	if (PointIsOnElement(x, y))
	{
		size_t index = static_cast<size_t>((y - GetY()) / m_theme->radiogroup.elementSize);
		if (index < m_items.size())
			m_selected = index;
		if (m_onChange)
			m_onChange();
		SetFocus();
		return true;
	}
	return false;
}

void UIRadioGroup::AddItem(std::wstring const& str)
{
	m_items.push_back(str);
	Invalidate();
}

std::wstring const UIRadioGroup::GetText() const
{
	if (m_selected > m_items.size())
	{
		return L"";
	}
	return m_items[m_selected];
}

void UIRadioGroup::SetSelected(size_t index)
{
	m_selected = index;
	Invalidate();
}

void UIRadioGroup::DeleteItem(size_t index)
{
	m_items.erase(m_items.begin() + index);
	if (m_selected == index && index != 0)
		m_selected--;
	Invalidate();
}

void UIRadioGroup::SetText(std::wstring const& text)
{
	Invalidate();
	for (size_t i = 0; i < m_items.size(); ++i)
	{
		if (m_items[i] == text)
		{
			m_selected = i;
			return;
		}
	}
}

size_t UIRadioGroup::GetSelectedIndex() const
{
	return m_selected;
}

size_t UIRadioGroup::GetItemsCount() const
{
	return m_items.size();
}

std::wstring UIRadioGroup::GetItem(size_t index) const
{
	return m_items[index];
}

void UIRadioGroup::ClearItems()
{
	m_items.clear();
	m_selected = 0;
	Invalidate();
}

void UIRadioGroup::SetOnChangeCallback(std::function<void()> const& onChange)
{
	m_onChange = onChange;
}
}
}