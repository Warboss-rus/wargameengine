#include "UIRadioGroup.h"
#include "UIText.h"

void CUIRadioGroup::Draw() const
{
	if(!m_visible)
		return;
	m_renderer.PushMatrix();
	m_renderer.Translate(GetX(), GetY(), 0);
	for(size_t i = 0; i < m_items.size(); ++i)
	{
		m_renderer.SetTexture(m_theme->texture);
		float y = m_theme->radiogroup.elementSize * i + (m_theme->radiogroup.elementSize - m_theme->radiogroup.buttonSize) / 2;
		float * texCoord = m_selected ? m_theme->radiogroup.selectedTexCoord : m_theme->radiogroup.texCoord;
		m_renderer.RenderArrays(RenderMode::TRIANGLE_STRIP,
		{ CVector2f(0.0f, y), { 0.0f, y + m_theme->radiogroup.buttonSize }, { m_theme->radiogroup.buttonSize, y }, { m_theme->radiogroup.buttonSize, y + m_theme->radiogroup.buttonSize} }, 
		{ CVector2f(texCoord), {texCoord[0], texCoord[3]}, {texCoord[2], texCoord[1]}, {texCoord[2], texCoord[3]} });
		int intSize = static_cast<int>(m_theme->radiogroup.elementSize);
		PrintText(static_cast<int>(m_theme->radiogroup.buttonSize) + 1, intSize * i, GetWidth(), intSize, m_items[i], m_theme->text);
	}
	CUIElement::Draw();
	m_renderer.PopMatrix();
}

bool CUIRadioGroup::LeftMouseButtonUp(int x, int y)
{
	if(!m_visible) return false;
	if(CUIElement::LeftMouseButtonUp(x, y))
	{
		return true;
	}
	if(PointIsOnElement(x, y))
	{
		size_t index = static_cast<size_t>((y - GetY()) / m_theme->radiogroup.elementSize);
		if(index >= 0 && index < m_items.size()) m_selected = index;
		if(m_onChange) m_onChange();
		SetFocus();
		return true;
	}
	return false;
}

void CUIRadioGroup::AddItem(std::string const& str)
{
	m_items.push_back(str);
	if(m_selected == -1)
	{
		m_selected = 0;
	}
}

std::string const CUIRadioGroup::GetText() const
{
	return m_items[m_selected];
}

void CUIRadioGroup::SetSelected(size_t index)
{
	m_selected = index;
}

void CUIRadioGroup::DeleteItem(size_t index)
{
	m_items.erase(m_items.begin() + index);
	if(m_selected == index) m_selected--;
	if(m_selected == -1 && !m_items.empty()) m_selected = 0;
}

void CUIRadioGroup::SetText(std::string const& text)
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

int CUIRadioGroup::GetSelectedIndex() const
{ 
	return m_selected; 
}

size_t CUIRadioGroup::GetItemsCount() const
{ 
	return m_items.size(); 
}

std::string CUIRadioGroup::GetItem(size_t index) const
{ 
	return m_items[index]; 
}

void CUIRadioGroup::ClearItems()
{ 
	m_items.clear(); 
	m_selected = -1; 
}

void CUIRadioGroup::SetOnChangeCallback(std::function<void()> const& onChange)
{ 
	m_onChange = onChange; 
}