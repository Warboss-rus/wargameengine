#include "UIRadioGroup.h"
#include "UIText.h"
#include "../view/IRenderer.h"

CUIRadioGroup::CUIRadioGroup(int x, int y, int height, int width, IUIElement * parent, IRenderer & renderer, ITextWriter & textWriter)
	: CUIElement(x, y, height, width, parent, renderer, textWriter), m_selected(0)
{
}

void CUIRadioGroup::Draw() const
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
			for (size_t i = 0; i < m_items.size(); ++i)
			{
				m_renderer.SetTexture(m_theme->texture, true);
				auto& theme = m_theme->radiogroup;
				int buttonSize = static_cast<int>(theme.buttonSize);
				int y = static_cast<int>(theme.elementSize * i + (theme.elementSize - buttonSize) / 2);
				float * texCoord = (i == m_selected) ? theme.selectedTexCoord : theme.texCoord;
				m_renderer.RenderArrays(RenderMode::TRIANGLE_STRIP,
				{ CVector2i(0, y), { 0, y + buttonSize }, { buttonSize, y }, { buttonSize, y + buttonSize} },
				{ CVector2f(texCoord), {texCoord[0], texCoord[3]}, {texCoord[2], texCoord[1]}, {texCoord[2], texCoord[3]} });
				int intSize = static_cast<int>(theme.elementSize);
				PrintText(m_renderer, m_textWriter, static_cast<int>(buttonSize) + 1, intSize * static_cast<int>(i), GetWidth(), intSize, m_items[i], m_theme->text, m_scale);
			}
		}, *m_cache, GetWidth(), GetHeight());
	}
	m_renderer.SetTexture(*m_cache);
	m_renderer.RenderArrays(RenderMode::TRIANGLE_STRIP,
	{ CVector2i(0, 0),{ GetWidth(), 0 },{ 0, GetHeight() },{ GetWidth(), GetHeight() } },
	{ CVector2f(0.0f, 0.0f),{ 1.0f, 0.0f },{ 0.0f, 1.0f },{ 1.0f, 1.0f } });
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
	Invalidate();
	if(PointIsOnElement(x, y))
	{
		size_t index = static_cast<size_t>((y - GetY()) / m_theme->radiogroup.elementSize);
		if(index < m_items.size()) m_selected = index;
		if(m_onChange) m_onChange();
		SetFocus();
		return true;
	}
	return false;
}

void CUIRadioGroup::AddItem(std::wstring const& str)
{
	m_items.push_back(str);
	Invalidate();
}

std::wstring const CUIRadioGroup::GetText() const
{
	if (m_selected > m_items.size())
	{
		return L"";
	}
	return m_items[m_selected];
}

void CUIRadioGroup::SetSelected(size_t index)
{
	m_selected = index;
	Invalidate();
}

void CUIRadioGroup::DeleteItem(size_t index)
{
	m_items.erase(m_items.begin() + index);
	if(m_selected == index && index != 0) m_selected--;
	Invalidate();
}

void CUIRadioGroup::SetText(std::wstring const& text)
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

size_t CUIRadioGroup::GetSelectedIndex() const
{ 
	return m_selected; 
}

size_t CUIRadioGroup::GetItemsCount() const
{ 
	return m_items.size(); 
}

std::wstring CUIRadioGroup::GetItem(size_t index) const
{ 
	return m_items[index]; 
}

void CUIRadioGroup::ClearItems()
{ 
	m_items.clear(); 
	m_selected = 0; 
	Invalidate();
}

void CUIRadioGroup::SetOnChangeCallback(std::function<void()> const& onChange)
{ 
	m_onChange = onChange; 
}