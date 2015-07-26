#include "UIList.h"
#include "UIText.h"

CUIList::CUIList(int x, int y, int height, int width, IUIElement * parent, IRenderer & renderer)
	: CUIElement(x, y, height, width, parent, renderer), m_selected(-1), m_scrollbar(m_theme, m_renderer)
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
		m_cache = move(m_renderer.RenderToTexture([this]() {
			m_renderer.SetColor(m_theme->defaultColor[0], m_theme->defaultColor[1], m_theme->defaultColor[2]);
			m_renderer.RenderArrays(RenderMode::RECTANGLES,
			{ CVector2i(0, 0), { 0, GetHeight() }, { GetWidth(), GetHeight() }, { GetWidth(), 0 } }, {});
			m_renderer.SetColor(m_theme->textfieldColor[0], m_theme->textfieldColor[1], m_theme->textfieldColor[2]);
			int borderSize = m_theme->list.borderSize;
			m_renderer.RenderArrays(RenderMode::RECTANGLES,
			{ CVector2i(borderSize, borderSize), {borderSize, GetHeight() - borderSize}, {GetWidth() - borderSize, GetHeight() - borderSize}, {GetWidth() - borderSize, borderSize} }, {});
			if (m_selected > -1)
			{
				m_renderer.SetColor(0.2f, 0.2f, 1.0f);
				int elementSize = m_theme->list.elementSize;
				m_renderer.RenderArrays(RenderMode::RECTANGLES, { CVector2i(borderSize, borderSize + elementSize * m_selected), {borderSize, 2 * borderSize + elementSize * (m_selected + 1)},
					{GetWidth() - borderSize, 2 * borderSize + elementSize * (m_selected + 1) }, { GetWidth() - borderSize, borderSize + elementSize * m_selected } }, {});
			}
			m_renderer.SetColor(m_theme->text.color[0], m_theme->text.color[1], m_theme->text.color[2]);
			for (size_t i = m_scrollbar.GetPosition() / m_theme->list.elementSize; i < m_items.size(); ++i)
			{
				if (m_theme->list.borderSize + m_theme->list.elementSize * (static_cast<int>(i) - m_scrollbar.GetPosition() / m_theme->list.elementSize) > GetHeight()) break;
				PrintText(m_theme->list.borderSize, m_theme->list.borderSize + m_theme->list.elementSize * (i - m_scrollbar.GetPosition() / m_theme->list.elementSize), GetWidth(), m_theme->list.text.fontSize, m_items[i], m_theme->text);
			}
			m_scrollbar.Draw();
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
		int index = (y - GetY()) / m_theme->list.elementSize;
		if(index >= 0 && index < m_items.size()) m_selected = index;
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
	m_scrollbar.Update(GetHeight(), m_theme->list.elementSize * m_items.size(), GetWidth(), m_theme->list.elementSize);
	Invalidate();
}

std::wstring const CUIList::GetText() const
{
	if (m_selected >= 0)
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
	m_scrollbar.Update(GetHeight(), m_theme->list.elementSize * m_items.size(), GetWidth(), m_theme->list.elementSize);
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
	m_scrollbar.Update(GetHeight(), m_theme->list.elementSize * m_items.size(), GetWidth(), m_theme->list.elementSize);
	Invalidate();
}

int CUIList::GetSelectedIndex() const
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
	m_selected = -1; 
	Invalidate();
}

void CUIList::SetOnChangeCallback(std::function<void()> const& onChange)
{ 
	m_onChange = onChange; 
}

void CUIList::SetTheme(std::shared_ptr<CUITheme> theme)
{ 
	m_theme = theme; 
	m_scrollbar = CUIScrollBar(theme, m_renderer); 
	Invalidate();
}

void CUIList::OnMouseMove(int x, int y)
{
	if (m_visible && m_focused) m_focused->OnMouseMove(x, y);
	if (m_scrollbar.OnMouseMove(x, y))
	{
		Invalidate();
	}
}