#include "UIEdit.h"

namespace wargameEngine
{
namespace ui
{
UIEdit::UIEdit(int x, int y, int height, int width, std::wstring const& text, IUIElement* parent, IUITextHelper& textHelper)
	: UICachedElement(x, y, height, width, parent)
	, m_textHelper(textHelper)
	, m_text(text)
	, m_pos(0)
	, m_beginSelection(0)
{
}

void UIEdit::DoPaint(IUIRenderer& renderer) const
{
	renderer.DrawRect({ 0, 0, GetWidth(), GetHeight() }, m_theme->defaultColor);

	auto& theme = m_theme->edit;
	auto& textTheme = theme.text;
	int borderSize = theme.borderSize;
	renderer.DrawRect({ borderSize, borderSize, GetWidth() - borderSize, GetHeight() - borderSize }, m_theme->textfieldColor);

	int fonty = (GetHeight() + textTheme.fontSize) / 2;
	if (IsFocused(nullptr))
	{
		int cursorpos = borderSize + renderer.GetStringWidth(m_text.substr(0, m_pos), textTheme.font, textTheme.fontSize);
		constexpr float color[] = { 0, 0, 0 };
		renderer.DrawLine({ cursorpos, fonty }, { cursorpos, fonty - static_cast<int>(textTheme.fontSize) }, color);
	}
	if (m_pos != m_beginSelection)
	{
		int selectionBegin = renderer.GetStringWidth(m_text.substr(0, m_beginSelection), textTheme.font, textTheme.fontSize);
		int selectionEnd = renderer.GetStringWidth(m_text.substr(0, m_pos), textTheme.font, textTheme.fontSize);
		int fontHeight = renderer.GetStringHeight(m_text, textTheme.font, textTheme.fontSize);
		renderer.DrawRect({ borderSize + selectionBegin, fonty - fontHeight, borderSize + selectionEnd, fonty }, theme.selectionColor);
	}
	renderer.DrawText({ borderSize, borderSize, m_width - 2 * borderSize, m_height - 2 * borderSize }, m_text, textTheme, m_scale);
}

bool UIEdit::OnCharacterInput(wchar_t key)
{
	if (!m_visible)
		return false;
	if (UIElement::OnCharacterInput(key))
		return true;
	Invalidate();
	if (!IsFocused(NULL))
	{
		return false;
	}
	m_text.insert(m_pos, 1, key);
	m_pos++;
	m_beginSelection++;
	return true;
}

bool UIEdit::OnKeyPress(view::VirtualKey key, int modifiers)
{
	if (!m_visible)
		return false;
	Invalidate();
	if (UIElement::OnKeyPress(key, modifiers))
		return true;
	if (!IsFocused(NULL))
	{
		return false;
	}
	switch (key)
	{
	case view::VirtualKey::KEY_LEFT:
	{
		if (m_pos > 0)
			m_pos--;
		if (m_beginSelection > 0)
			m_beginSelection--;
		return true;
	}
	break;
	case view::VirtualKey::KEY_RIGHT:
	{
		if (m_pos < m_text.size())
			m_pos++;
		if (m_beginSelection < m_text.size())
			m_beginSelection++;
		return true;
	}
	break;
	case view::VirtualKey::KEY_HOME:
	{
		m_pos = 0;
		return true;
	}
	break;
	case view::VirtualKey::KEY_END:
	{
		m_pos = m_text.size();
		return true;
	}
	break;
	case view::VirtualKey::KEY_BACKSPACE:
	{
		if (m_pos > 0)
		{
			m_text.erase(m_pos - 1, 1);
			m_pos--;
			m_beginSelection--;
		}
	}
	break;
	case view::VirtualKey::KEY_DELETE:
	{
		if (m_pos != m_beginSelection)
		{
			size_t begin = (m_pos < m_beginSelection) ? m_pos : m_beginSelection;
			size_t count = (m_pos > m_beginSelection) ? m_pos - m_beginSelection : m_beginSelection - m_pos;
			m_text.erase(begin, count);
			m_pos = begin;
			m_beginSelection = begin;
		}
		else
		{
			if (m_pos < m_text.size())
			{
				m_text.erase(m_pos, 1);
			}
		}
	}
	break;
	default:
		break;
	}
	return false;
}

bool UIEdit::LeftMouseButtonUp(int x, int y)
{
	if (!m_visible)
		return false;
	if (UIElement::LeftMouseButtonUp(x, y))
		return true;
	Invalidate();
	if (PointIsOnElement(x, y))
	{
		SetCursorPos(x);
		if (m_beginSelection > m_pos)
			std::swap(m_beginSelection, m_pos);
		SetFocus();
		return true;
	}
	else
	{
		m_beginSelection = m_pos;
	}
	return false;
}

bool UIEdit::LeftMouseButtonDown(int x, int y)
{
	if (!m_visible)
		return false;
	if (UIElement::LeftMouseButtonDown(x, y))
		return true;
	Invalidate();
	if (PointIsOnElement(x, y))
	{
		SetCursorPos(x);
		m_beginSelection = m_pos;
		SetFocus();
		return true;
	}
	return false;
}

void UIEdit::SetCursorPos(int x)
{
	int pos = x - GetX();
	auto& text = m_theme->edit.text;
	if (pos > m_textHelper.GetStringWidth(m_text, text.font, text.fontSize))
	{
		m_pos = m_text.size();
		return;
	}
	size_t begin = 0;
	size_t end = m_text.size();
	while (begin < end - 1)
	{
		size_t divider = (end + begin) / 2;
		if (m_textHelper.GetStringWidth(m_text.substr(0, divider), text.font, text.fontSize) > pos)
		{
			end = divider;
		}
		else
		{
			begin = divider;
		}
	}
	m_pos = begin;
}

std::wstring const UIEdit::GetText() const
{
	return m_text;
}

void UIEdit::SetText(std::wstring const& text)
{
	m_text = text;
	Invalidate();
}
}
}