#include "UIList.h"
#include "UIText.h"

void CUIList::Draw() const
{
	if(!m_visible)
		return;
	m_renderer.PushMatrix();
	m_renderer.Translate(GetX(), GetY(), 0);
	m_renderer.SetColor(m_theme->defaultColor[0], m_theme->defaultColor[1], m_theme->defaultColor[2]);
	m_renderer.RenderArrays(RenderMode::RECTANGLES,
	{ CVector2i(0, 0), { 0, GetHeight() }, { GetWidth(), GetHeight() }, { GetWidth(), 0 } }, {});
	m_renderer.SetColor(m_theme->textfieldColor[0], m_theme->textfieldColor[1], m_theme->textfieldColor[2]);
	int borderSize = m_theme->list.borderSize;
	m_renderer.RenderArrays(RenderMode::RECTANGLES,
	{ CVector2i(borderSize, borderSize), {borderSize, GetHeight() - borderSize}, {GetWidth() - borderSize, GetHeight() - borderSize}, {GetWidth() - borderSize, borderSize} }, {});
	if(m_selected > -1)
	{
		m_renderer.SetColor(0.2f, 0.2f, 1.0f);
		int elementSize = m_theme->list.elementSize;
		m_renderer.RenderArrays(RenderMode::RECTANGLES,	{ CVector2i(borderSize, borderSize + elementSize * m_selected), {borderSize, 2 * borderSize + elementSize * (m_selected + 1)}, 
			{GetWidth() - borderSize, 2 * borderSize + elementSize * (m_selected + 1) }, { GetWidth() - borderSize, borderSize + elementSize * m_selected } }, {});
	}
	m_renderer.SetColor(m_theme->text.color[0], m_theme->text.color[1], m_theme->text.color[2]);
	for (size_t i = m_scrollbar.GetPosition() / m_theme->list.elementSize; i < m_items.size(); ++i)
	{
		if (m_theme->list.borderSize + m_theme->list.elementSize * (static_cast<int>(i) - m_scrollbar.GetPosition() / m_theme->list.elementSize) > GetHeight()) break;
		PrintText(m_theme->list.borderSize, m_theme->list.borderSize + m_theme->list.elementSize * (i - m_scrollbar.GetPosition() / m_theme->list.elementSize), GetWidth(), m_theme->list.text.fontSize, m_items[i], m_theme->text);
	}
	m_scrollbar.Draw();
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
	return m_scrollbar.LeftMouseButtonDown(x - GetX(), y - GetY());
}

bool CUIList::LeftMouseButtonUp(int x, int y)
{
	if(!m_visible) return false;
	if(CUIElement::LeftMouseButtonUp(x, y))
	{
		return true;
	}
	if (m_scrollbar.LeftMouseButtonUp(x - GetX(), y - GetY())) return true;
	if(PointIsOnElement(x, y))
	{
		unsigned int index = (y - GetY()) / m_theme->list.elementSize;
		if(index >= 0 && index < m_items.size()) m_selected = index;
		if(m_onChange) m_onChange();
		SetFocus();
		return true;
	}
	return false;
}

void CUIList::AddItem(std::string const& str)
{
	m_items.push_back(str);
	if(m_selected == -1)
	{
		m_selected = 0;
	}
	m_scrollbar.Update(GetHeight(), m_theme->list.elementSize * m_items.size(), GetWidth(), m_theme->list.elementSize);
}

std::string const CUIList::GetText() const
{
	return m_items[m_selected];
}

void CUIList::SetSelected(size_t index)
{
	m_selected = index;
}

void CUIList::DeleteItem(size_t index)
{
	m_items.erase(m_items.begin() + index);
	if(m_selected == index) m_selected--;
	if(m_selected == -1 && !m_items.empty()) m_selected = 0;
	m_scrollbar.Update(GetHeight(), m_theme->list.elementSize * m_items.size(), GetWidth(), m_theme->list.elementSize);
}

void CUIList::SetText(std::string const& text)
{
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
	m_scrollbar.Update(GetHeight(), m_theme->list.elementSize * m_items.size(), GetWidth(), m_theme->list.elementSize);
}

int CUIList::GetSelectedIndex() const
{ 
	return m_selected; 
}

size_t CUIList::GetItemsCount() const
{ 
	return m_items.size(); 
}

std::string CUIList::GetItem(size_t index) const
{ 
	return m_items[index]; 
}

void CUIList::ClearItems()
{ 
	m_items.clear(); 
	m_selected = -1; 
}

void CUIList::SetOnChangeCallback(std::function<void()> const& onChange)
{ 
	m_onChange = onChange; 
}

void CUIList::SetTheme(std::shared_ptr<CUITheme> theme)
{ 
	m_theme = theme; 
	m_scrollbar = CUIScrollBar(theme, m_renderer); 
}