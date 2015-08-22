#include "UIEdit.h"
#include "UIText.h"
#include "..\view\KeyDefines.h"

CUIEdit::CUIEdit(int x, int y, int height, int width, std::wstring const& text, IUIElement * parent, IRenderer & renderer) :
	CUIElement(x, y, height, width, parent, renderer), m_isPressed(false), m_pos(0), m_beginSelection(0), m_text(text)
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
		m_cache = move(m_renderer.RenderToTexture([this]() {
			m_renderer.SetColor(m_theme->defaultColor);
			m_renderer.RenderArrays(RenderMode::RECTANGLES, { CVector2i(0, 0), { 0, GetHeight() }, { GetWidth(), GetHeight() }, { GetWidth(), 0 } }, {});
			m_renderer.SetColor(m_theme->textfieldColor);
			int borderSize = m_theme->edit.borderSize;
			m_renderer.RenderArrays(RenderMode::RECTANGLES, { CVector2i(borderSize, borderSize), {borderSize, GetHeight() - borderSize}, {GetWidth() - borderSize, GetHeight() - borderSize}, {GetWidth() - borderSize, borderSize} }, {});
			int fonty = (GetHeight() + m_theme->edit.text.fontSize) / 2;
			if (IsFocused(nullptr))
			{
				int cursorpos = m_theme->edit.borderSize + GetStringWidth(m_theme->edit.text, m_text.substr(0, m_pos));
				m_renderer.SetColor(0, 0, 0);
				m_renderer.RenderArrays(RenderMode::LINES, { CVector2i(cursorpos, fonty), { cursorpos, fonty - static_cast<int>(m_theme->edit.text.fontSize) } }, {});
			}
			if (m_pos != m_beginSelection)
			{
				m_renderer.SetColor(m_theme->edit.selectionColor);
				int selectionBegin = GetStringWidth(m_theme->edit.text, m_text.substr(0, m_beginSelection));
				int selectionEnd = GetStringWidth(m_theme->edit.text, m_text.substr(0, m_pos));
				int fontHeight = GetStringHeight(m_theme->edit.text, m_text);
				m_renderer.RenderArrays(RenderMode::RECTANGLES, { CVector2i(borderSize + selectionBegin, fonty),
				{ borderSize + selectionBegin, fonty - fontHeight }, {borderSize + selectionEnd, fonty - fontHeight }, {borderSize + selectionEnd, fonty} }, {});
			}
			PrintText(m_theme->edit.borderSize, m_theme->edit.borderSize, m_width - 2 * m_theme->edit.borderSize, m_height - 2 * m_theme->edit.borderSize, m_text, m_theme->text);
		}, GetWidth(), GetHeight()));
	}
	m_cache->Bind();
	m_renderer.RenderArrays(RenderMode::TRIANGLE_STRIP,
	{ CVector2i(0, 0),{ GetWidth(), 0 },{ 0, GetHeight() },{ GetWidth(), GetHeight() } },
	{ CVector2f(0.0f, 0.0f),{ 1.0f, 0.0f },{ 0.0f, 1.0f },{ 1.0f, 1.0f } });
	m_renderer.SetTexture("");

	CUIElement::Draw();
	m_renderer.PopMatrix();
}

bool CUIEdit::OnKeyPress(unsigned char key)
{
	if (!m_visible) return false;
	if (CUIElement::OnKeyPress(key))
		return true;
	Invalidate();
	if (!IsFocused(NULL))
	{
		return false;
	}
	if (key < 32 && key != 8)
		return false;
	if (key != 127 && key != 8)
	{
		wchar_t str[2];
		char ckey = key;
		mbstowcs(str, &ckey, 1);
		str[1] = L'\0';
		m_text.insert(m_pos, str);
		m_pos++;
		m_beginSelection++;
	}
	if (key == 8 && m_pos > 0)
	{
		m_text.erase(m_pos - 1, 1);
		m_pos--;
		m_beginSelection--;
	}
	if (key == 127)
	{
		if (m_pos != m_beginSelection)
		{
			size_t begin = (m_pos < m_beginSelection) ? m_pos : m_beginSelection;
			size_t count = (m_pos - m_beginSelection > 0) ? m_pos - m_beginSelection : m_beginSelection - m_pos;
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
	return true;
}

bool CUIEdit::OnSpecialKeyPress(int key)
{
	if (!m_visible) return false;
	Invalidate();
	if (CUIElement::OnSpecialKeyPress(key))
		return true;
	if (!IsFocused(NULL))
	{
		return false;
	}
	switch (key)
	{
	case KEY_LEFT:
	{
		if (m_pos > 0) m_pos--;
		if (m_beginSelection > 0) m_beginSelection--;
		return true;
	}break;
	case KEY_RIGHT:
	{
		if (m_pos < m_text.size()) m_pos++;
		if (m_beginSelection < m_text.size()) m_beginSelection++;
		return true;
	}break;
	case KEY_HOME:
	{
		m_pos = 0;
		return true;
	}break;
	case KEY_END:
	{
		m_pos = m_text.size();
		return true;
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
	if (pos > GetStringWidth(m_theme->edit.text, m_text))
	{
		m_pos = m_text.size();
		return;
	}
	size_t begin = 0;
	size_t end = m_text.size();
	while (begin < end - 1)
	{
		size_t divider = (end + begin) / 2;
		if (GetStringWidth(m_theme->edit.text, m_text.substr(0, divider)) > pos)
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