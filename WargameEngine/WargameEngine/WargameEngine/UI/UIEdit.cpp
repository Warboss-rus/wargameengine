#include "UIEdit.h"
#include "../view/IRenderer.h"
#include "../view/ITextWriter.h"
#include "../view/KeyDefines.h"
#include "UIText.h"

namespace wargameEngine
{
namespace ui
{
UIEdit::UIEdit(int x, int y, int height, int width, std::wstring const& text, IUIElement* parent, view::ITextWriter& textWriter)
	: UIElement(x, y, height, width, parent, textWriter)
	, m_text(text)
	, m_pos(0)
	, m_beginSelection(0)
{
}

void UIEdit::Draw(view::IRenderer& renderer) const
{
	if (!m_visible)
		return;
	renderer.PushMatrix();
	renderer.Translate(GetX(), GetY(), 0);
	if (!m_cache)
	{
		m_cache = renderer.CreateTexture(nullptr, GetWidth(), GetHeight(), CachedTextureType::RENDER_TARGET);
	}
	if (m_invalidated)
	{
		renderer.RenderToTexture([this, &renderer]() {
			renderer.UnbindTexture();
			renderer.SetColor(m_theme->defaultColor);
			renderer.RenderArrays(RenderMode::TRIANGLE_STRIP, { CVector2i(0, 0), { 0, GetHeight() }, { GetWidth(), 0 }, { GetWidth(), GetHeight() } }, {});
			renderer.SetColor(m_theme->textfieldColor);
			auto& theme = m_theme->edit;
			auto& textTheme = theme.text;
			int borderSize = theme.borderSize;
			renderer.RenderArrays(RenderMode::TRIANGLE_STRIP, { CVector2i(borderSize, borderSize), { borderSize, GetHeight() - borderSize }, { GetWidth() - borderSize, borderSize }, { GetWidth() - borderSize, GetHeight() - borderSize } }, {});
			int fonty = (GetHeight() + textTheme.fontSize) / 2;
			if (IsFocused(nullptr))
			{
				int cursorpos = borderSize + m_textWriter.GetStringWidth(textTheme.font, textTheme.fontSize, m_text.substr(0, m_pos));
				renderer.SetColor(0, 0, 0);
				renderer.RenderArrays(RenderMode::LINES, { CVector2i(cursorpos, fonty), { cursorpos, fonty - static_cast<int>(textTheme.fontSize) } }, {});
			}
			if (m_pos != m_beginSelection)
			{
				renderer.SetColor(theme.selectionColor);
				int selectionBegin = m_textWriter.GetStringWidth(textTheme.font, textTheme.fontSize, m_text.substr(0, m_beginSelection));
				int selectionEnd = m_textWriter.GetStringWidth(textTheme.font, textTheme.fontSize, m_text.substr(0, m_pos));
				int fontHeight = m_textWriter.GetStringHeight(textTheme.font, textTheme.fontSize, m_text);
				renderer.RenderArrays(RenderMode::TRIANGLE_STRIP, { CVector2i(borderSize + selectionBegin, fonty), { borderSize + selectionBegin, fonty - fontHeight }, { borderSize + selectionEnd, fonty }, { borderSize + selectionEnd, fonty - fontHeight } }, {});
			}
			PrintText(renderer, m_textWriter, borderSize, borderSize, m_width - 2 * borderSize, m_height - 2 * borderSize, m_text, textTheme, m_scale);
		}, *m_cache, GetWidth(), GetHeight());
	}
	renderer.SetTexture(*m_cache);
	renderer.RenderArrays(RenderMode::TRIANGLE_STRIP,
		{ CVector2i(0, 0), { GetWidth(), 0 }, { 0, GetHeight() }, { GetWidth(), GetHeight() } },
		{ CVector2f(0.0f, 0.0f), { 1.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } });

	UIElement::Draw(renderer);
	renderer.PopMatrix();
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
	if (pos > m_textWriter.GetStringWidth(text.font, text.fontSize, m_text))
	{
		m_pos = m_text.size();
		return;
	}
	size_t begin = 0;
	size_t end = m_text.size();
	while (begin < end - 1)
	{
		size_t divider = (end + begin) / 2;
		if (m_textWriter.GetStringWidth(text.font, text.fontSize, m_text.substr(0, divider)) > pos)
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