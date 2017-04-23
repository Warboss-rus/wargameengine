#include "UIEdit.h"
#include "UIText.h"
#include "../view/KeyDefines.h"
#include "../view/ITextWriter.h"
#include "../view/IRenderer.h"

CUIEdit::CUIEdit(int x, int y, int height, int width, std::wstring const& text, IUIElement * parent, IRenderer & renderer, ITextWriter & textWriter) :
	CUIElement(x, y, height, width, parent, renderer, textWriter), m_text(text), m_pos(0), m_beginSelection(0)
{

}

void CUIEdit::Draw() const
{
	if (!m_visible)
		return;
	m_renderer.PushMatrix();
	m_renderer.Translate(GetX(), GetY(), 0);
	if (!m_cache)
	{
		m_cache = m_renderer.CreateTexture(nullptr, GetWidth(), GetHeight(), CachedTextureType::RENDER_TARGET);
	}
	if(m_invalidated)
	{
		m_renderer.RenderToTexture([this]() {
			m_renderer.SetColor(m_theme->defaultColor);
			m_renderer.RenderArrays(RenderMode::TRIANGLE_STRIP, { CVector2i(0, 0), { 0, GetHeight() },{ GetWidth(), 0 }, { GetWidth(), GetHeight() } }, {});
			m_renderer.SetColor(m_theme->textfieldColor);
			auto& theme = m_theme->edit;
			auto& textTheme = theme.text;
			int borderSize = theme.borderSize;
			m_renderer.RenderArrays(RenderMode::TRIANGLE_STRIP, { CVector2i(borderSize, borderSize), {borderSize, GetHeight() - borderSize}, { GetWidth() - borderSize, borderSize }, {GetWidth() - borderSize, GetHeight() - borderSize} }, {});
			int fonty = (GetHeight() + textTheme.fontSize) / 2;
			if (IsFocused(nullptr))
			{
				int cursorpos = borderSize + m_textWriter.GetStringWidth(textTheme.font, textTheme.fontSize, m_text.substr(0, m_pos));
				m_renderer.SetColor(0, 0, 0);
				m_renderer.RenderArrays(RenderMode::LINES, { CVector2i(cursorpos, fonty), { cursorpos, fonty - static_cast<int>(textTheme.fontSize) } }, {});
			}
			if (m_pos != m_beginSelection)
			{
				m_renderer.SetColor(theme.selectionColor);
				int selectionBegin = m_textWriter.GetStringWidth(textTheme.font, textTheme.fontSize, m_text.substr(0, m_beginSelection));
				int selectionEnd = m_textWriter.GetStringWidth(textTheme.font, textTheme.fontSize, m_text.substr(0, m_pos));
				int fontHeight = m_textWriter.GetStringHeight(textTheme.font, textTheme.fontSize, m_text);
				m_renderer.RenderArrays(RenderMode::TRIANGLE_STRIP, { CVector2i(borderSize + selectionBegin, fonty),
				{ borderSize + selectionBegin, fonty - fontHeight },{ borderSize + selectionEnd, fonty }, {borderSize + selectionEnd, fonty - fontHeight } }, {});
			}
			PrintText(m_renderer, m_textWriter, borderSize, borderSize, m_width - 2 * borderSize, m_height - 2 * borderSize, m_text, textTheme, m_scale);
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

bool CUIEdit::OnCharacterInput(wchar_t key)
{
	if (!m_visible) return false;
	if (CUIElement::OnCharacterInput(key))
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

bool CUIEdit::OnKeyPress(VirtualKey key, int modifiers)
{
	if (!m_visible) return false;
	Invalidate();
	if (CUIElement::OnKeyPress(key, modifiers))
		return true;
	if (!IsFocused(NULL))
	{
		return false;
	}
	switch (key)
	{
	case VirtualKey::KEY_LEFT:
	{
		if (m_pos > 0) m_pos--;
		if (m_beginSelection > 0) m_beginSelection--;
		return true;
	}break;
	case VirtualKey::KEY_RIGHT:
	{
		if (m_pos < m_text.size()) m_pos++;
		if (m_beginSelection < m_text.size()) m_beginSelection++;
		return true;
	}break;
	case VirtualKey::KEY_HOME:
	{
		m_pos = 0;
		return true;
	}break;
	case VirtualKey::KEY_END:
	{
		m_pos = m_text.size();
		return true;
	}break;
	case VirtualKey::KEY_BACKSPACE:
	{
		if (m_pos > 0)
		{
			m_text.erase(m_pos - 1, 1);
			m_pos--;
			m_beginSelection--;
		}
	}break;
	case VirtualKey::KEY_DELETE:
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
	}break;
	}
	return false;
}

bool CUIEdit::LeftMouseButtonUp(int x, int y)
{
	if (!m_visible) return false;
	if (CUIElement::LeftMouseButtonUp(x, y))
		return true;
	Invalidate();
	if (PointIsOnElement(x, y))
	{
		SetCursorPos(x);
		if (m_beginSelection > m_pos) std::swap(m_beginSelection, m_pos);
		SetFocus();
		return true;
	}
	else
	{
		m_beginSelection = m_pos;
	}
	return false;
}

bool CUIEdit::LeftMouseButtonDown(int x, int y)
{
	if (!m_visible) return false;
	if (CUIElement::LeftMouseButtonDown(x, y))
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

void CUIEdit::SetCursorPos(int x)
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

std::wstring const CUIEdit::GetText() const
{
	return m_text;
}

void CUIEdit::SetText(std::wstring const& text)
{
	m_text = text;
	Invalidate();
}