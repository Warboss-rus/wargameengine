#include "UIList.h"
#include "UIText.h"
#include "../view/IRenderer.h"

CUIList::CUIList(int x, int y, int height, int width, IUIElement * parent, IRenderer & renderer, ITextWriter & textWriter)
	: CUIElement(x, y, height, width, parent, renderer, textWriter), m_selected(0), m_scrollbar(m_theme, m_renderer)
{
}

void CUIList::Draw() const
{
	if(!m_visible)
		return;
	m_renderer.PushMatrix();
	m_renderer.Translate(GetX(), GetY(), 0);
	if (!m_cache)
	{
		m_cache = m_renderer.CreateTexture(nullptr, GetWidth(), GetHeight(), CachedTextureType::RENDER_TARGET);
	}
	if (m_invalidated)
	{
		m_renderer.RenderToTexture([this]() {
			m_renderer.SetColor(m_theme->defaultColor);
			m_renderer.RenderArrays(RenderMode::TRIANGLE_STRIP,
			{ CVector2i(0, 0), { 0, GetHeight() },{ GetWidth(), 0 }, { GetWidth(), GetHeight() } }, {});
			m_renderer.SetColor(m_theme->textfieldColor);
			auto& theme = m_theme->list;
			int borderSize = static_cast<int>(theme.borderSize * m_scale);
			int elementSize = static_cast<int>(theme.elementSize * m_scale);
			m_renderer.RenderArrays(RenderMode::TRIANGLE_STRIP,
			{ CVector2i(borderSize, borderSize), {borderSize, GetHeight() - borderSize},{ GetWidth() - borderSize, borderSize }, {GetWidth() - borderSize, GetHeight() - borderSize} }, {});
			if (m_items.size() > 0)
			{
				m_renderer.SetColor(theme.selectionColor);
				int intSelected = static_cast<int>(m_selected);
				m_renderer.RenderArrays(RenderMode::TRIANGLE_STRIP, { CVector2i(borderSize, borderSize + elementSize * intSelected), {borderSize, 2 * borderSize + elementSize * (intSelected + 1)},
				{ GetWidth() - borderSize, borderSize + elementSize * intSelected }, {GetWidth() - borderSize, 2 * borderSize + elementSize * (intSelected + 1) } }, {});
			}
			m_renderer.SetColor(theme.text.color);
			for (size_t i = m_scrollbar.GetPosition() / elementSize; i < m_items.size(); ++i)
			{
				if (borderSize + elementSize * (static_cast<int>(i) - m_scrollbar.GetPosition() / elementSize) > GetHeight()) break;
				PrintText(m_renderer, m_textWriter, borderSize, borderSize + elementSize * (static_cast<int>(i) - m_scrollbar.GetPosition() / elementSize), GetWidth(), static_cast<int>(theme.text.fontSize * m_scale), m_items[i], theme.text, m_scale);
			}
			m_scrollbar.Draw();
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

bool CUIList::LeftMouseButtonDown(int x, int y)
{
	if (!m_visible) return false;
	if (CUIElement::LeftMouseButtonDown(x, y))
	{
		return true;
	}
	Invalidate();
	return m_scrollbar.LeftMouseButtonDown(x - GetX(), y - GetY());
}

bool CUIList::LeftMouseButtonUp(int x, int y)
{
	if(!m_visible) return false;
	if(CUIElement::LeftMouseButtonUp(x, y))
	{
		return true;
	}
	Invalidate();
	if (m_scrollbar.LeftMouseButtonUp(x - GetX(), y - GetY())) return true;
	if(PointIsOnElement(x, y))
	{
		int index = (y - GetY()) / static_cast<int>(m_theme->list.elementSize * m_scale);
		if(index >= 0 && static_cast<size_t>(index) < m_items.size()) m_selected = static_cast<size_t>(index);
		if(m_onChange) m_onChange();
		SetFocus();
		return true;
	}
	return false;
}

void CUIList::AddItem(std::wstring const& str)
{
	m_items.push_back(str);
	if(m_selected == -1)
	{
		m_selected = 0;
	}
	int elementSize = static_cast<int>(m_theme->list.elementSize * m_scale);
	m_scrollbar.Update(GetHeight(), elementSize * static_cast<int>(m_items.size()), GetWidth(), elementSize);
	Invalidate();
}

std::wstring const CUIList::GetText() const
{
	if (m_selected < m_items.size())
	{
		return m_items[m_selected];
	}
	return L"";
}

void CUIList::SetSelected(size_t index)
{
	m_selected = index;
	Invalidate();
}

void CUIList::DeleteItem(size_t index)
{
	m_items.erase(m_items.begin() + index);
	if(m_selected == index) m_selected--;
	if(m_selected == -1 && !m_items.empty()) m_selected = 0;
	int elementSize = static_cast<int>(m_theme->list.elementSize * m_scale);
	m_scrollbar.Update(GetHeight(), elementSize * static_cast<int>(m_items.size()), GetWidth(), elementSize);
	Invalidate();
}

void CUIList::SetText(std::wstring const& text)
{
	Invalidate();
	for(size_t i = 0; i < m_items.size(); ++i)
	{
		if(m_items[i] == text)
		{
			m_selected = i;
			return;
		}
	}
}

void CUIList::Resize(int windowHeight, int windowWidth)
{
	CUIElement::Resize(windowHeight, windowWidth);
	int elementSize = static_cast<int>(m_theme->list.elementSize * m_scale);
	m_scrollbar.Update(GetHeight(), elementSize * static_cast<int>(m_items.size()), GetWidth(), elementSize);
	Invalidate();
}

size_t CUIList::GetSelectedIndex() const
{ 
	return m_selected; 
}

size_t CUIList::GetItemsCount() const
{ 
	return m_items.size(); 
}

std::wstring CUIList::GetItem(size_t index) const
{ 
	return m_items[index]; 
}

void CUIList::ClearItems()
{ 
	m_items.clear(); 
	m_selected = 0; 
	Invalidate();
}

void CUIList::SetOnChangeCallback(std::function<void()> const& onChange)
{ 
	m_onChange = onChange; 
}

void CUIList::SetTheme(std::shared_ptr<CUITheme> const& theme)
{ 
	m_theme = theme; 
	m_scrollbar = CUIScrollBar(theme, m_renderer); 
	Invalidate();
}

void CUIList::SetScale(float scale)
{
	CUIElement::SetScale(scale);
	m_scrollbar.SetScale(scale);
}

void CUIList::OnMouseMove(int x, int y)
{
	if (m_visible && m_focused) m_focused->OnMouseMove(x, y);
	if (m_scrollbar.OnMouseMove(x, y))
	{
		Invalidate();
	}
}