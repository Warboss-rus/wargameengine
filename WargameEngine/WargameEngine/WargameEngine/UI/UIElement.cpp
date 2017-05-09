#include "UIElement.h"
#include "UIButton.h"
#include "UIStaticText.h"
#include "UIPanel.h"
#include "UICheckBox.h"
#include "UIComboBox.h"
#include "UIEdit.h"
#include "UIList.h"
#include "UIRadioGroup.h"
#include <algorithm>
#include "UIWindow.h"
#include "../view/IRenderer.h"

void CUIElement::Draw() const
{
	m_invalidated = false;
	if(!m_visible || m_children.empty())
		return;
	if (!m_childrenCache)
	{
		m_childrenCache = m_renderer.CreateTexture(nullptr, GetWidth(), GetHeight(), CachedTextureType::RENDER_TARGET);
		m_childrenInvalidated = true;
	}
	if(m_childrenInvalidated)
	{
		//m_renderer.RenderToTexture([this] {
			for (auto i = m_children.begin(); i != m_children.end(); ++i)
			{
				if (i->second && i->second.get() != m_focused)
				{
					i->second->Draw();
				}
			}
			if (m_focused)
			{
				m_focused->Draw();
			}
		//}, *m_childrenCache, GetWidth(), GetHeight());
		//m_childrenInvalidated = false;
	}
	/*m_renderer.SetTexture(*m_childrenCache);
	m_renderer.RenderArrays(RenderMode::TRIANGLE_STRIP,
	{ CVector2i(0, 0),{ GetWidth(), 0 },{ 0,GetHeight() },{ GetWidth(), GetHeight() } },
	{ CVector2f(0.0f, 0.0f),{ 1.0f, 0.0f },{ 0.0f, 1.0f },{ 1.0f, 1.0f } });*/
}

void CUIElement::SetVisible(bool visible)
{
	m_visible = visible;
}

bool CUIElement::GetVisible() const
{
	return m_visible;
}

CUIElement::CUIElement(int x, int y, int height, int width, IUIElement * parent, IRenderer & renderer, ITextWriter & textWriter) 
	: m_x(x), m_y(y), m_height(height), m_width(width), m_parent(parent), m_renderer(renderer), m_textWriter(textWriter)
{

}

CUIElement::CUIElement(IRenderer & renderer, ITextWriter & textWriter) 
	: m_x(0), m_y(0), m_height(640), m_width(640), m_parent(nullptr), m_renderer(renderer), m_textWriter(textWriter)
{

}

CUIElement::~CUIElement()
{
}

void CUIElement::AddChild(std::string const& name, std::shared_ptr<IUIElement> const& element)
{
	m_children[name] = element;
	element->SetTheme(m_theme);
	element->Resize(m_windowHeight, m_windowWidth);
	element->SetTargetSize(m_uiWidth, m_uiHeight);
	element->SetScale(m_scale);
	InvalidateChildren();
}

IUIElement* CUIElement::GetChildByName(std::string const& name)
{
	return m_children[name].get();
}

void CUIElement::DeleteChild(std::string const& name)
{
	if (m_children.find(name)->second.get() == m_focused)
	{
		m_focused = NULL;
	}
	m_children.erase(m_children.find(name));
	InvalidateChildren();
}

void CUIElement::DeleteChild(IUIElement * element)
{
	auto it = std::find_if(m_children.begin(), m_children.end(), [element](auto& child) {return child.second.get() == element;});
	if (it != m_children.end())
	{
		m_children.erase(it);
	}
	InvalidateChildren();
}

bool CUIElement::PointIsOnElement(int x, int y) const
{
	if(x > GetX() && x < GetX() + GetWidth()	&& y > GetY() && y < GetY() + GetHeight())
		return true;
	return false;
}

void CUIElement::Invalidate(bool resetTexture) const
{
	m_invalidated = true;
	if(resetTexture) m_cache.reset();
	if (m_parent) m_parent->InvalidateChildren();
}

void CUIElement::InvalidateChildren(bool resetTexture) const
{
	m_childrenInvalidated = true;
	if(resetTexture) m_childrenCache.reset();
	if (m_parent) m_parent->InvalidateChildren();
}

void CUIElement::SetTargetSize(int width, int height)
{
	m_uiWidth = width;
	m_uiHeight = height;
	for (auto& child : m_children)
	{
		child.second->SetTargetSize(width, height);
	}
}

void CUIElement::SetScale(float scale)
{
	m_scale = scale;
	Invalidate();
	for (auto& child : m_children)
	{
		child.second->SetScale(scale);
	}
}

void CUIElement::SetState(bool)
{
	throw std::runtime_error("This UI element doesn't have state");
}

bool CUIElement::GetState() const
{
	throw std::runtime_error("This UI element doesn't have state");
}

bool CUIElement::LeftMouseButtonDown(int x, int y)
{
	if(!m_visible) return false;
	if(m_focused && m_focused->LeftMouseButtonDown(x - GetX(), y - GetY()))
		return true;
	for(auto i = m_children.begin(); i != m_children.end(); ++i)
	{
		if(i->second->LeftMouseButtonDown(x - GetX(), y - GetY()))
		{
			return true;
		}
	}
	return false;
}

bool CUIElement::LeftMouseButtonUp(int x, int y)
{
	if(!m_visible) return false;
	if(m_focused && m_focused->LeftMouseButtonUp(x - GetX(), y - GetY()))
		return true;
	for(auto i = m_children.begin(); i != m_children.end(); ++i)
	{
		if(i->second->LeftMouseButtonUp(x - GetX(), y - GetY()))
		{
			return true;
		}
	}
	SetFocus();
	return false;
}

bool CUIElement::OnCharacterInput(wchar_t key)
{
	if(!m_visible) return false;
	for(auto i = m_children.begin(); i != m_children.end(); ++i)
	{
		if (i->second->OnCharacterInput(key))
		{
			return true;
		}
	}
	return false;
}

bool CUIElement::OnKeyPress(VirtualKey key, int modifiers)
{
	if(!m_visible) return false;
	for(auto i = m_children.begin(); i != m_children.end(); ++i)
	{
		if (i->second->OnKeyPress(key, modifiers))
		{
			return true;
		}
	}
	return false;
}

void CUIElement::OnMouseMove(int x, int y)
{
	if(m_visible && m_focused) m_focused->OnMouseMove(x, y);
}

IUIElement* CUIElement::AddNewButton(std::string const& name, int x, int y, int height, int width, std::wstring const& text, std::function<void()> const& onClick)
{
	std::shared_ptr<IUIElement> item = std::make_shared<CUIButton>(x, y, height, width, text, onClick, this, m_renderer, m_textWriter);
	AddChild(name, item);
	return item.get();
}

IUIElement* CUIElement::AddNewStaticText(std::string const& name, int x, int y, int height, int width, std::wstring const& text)
{
	std::shared_ptr<IUIElement> item = std::make_shared<CUIStaticText>(x, y, height, width, text, this, m_renderer, m_textWriter);
	AddChild(name, item);
	return item.get();
}

IUIElement* CUIElement::AddNewPanel(std::string const& name, int x, int y, int height, int width)
{
	std::shared_ptr<IUIElement> item = std::make_shared<CUIPanel>(x, y, height, width, this, m_renderer, m_textWriter);
	AddChild(name, item);
	return item.get();
}

IUIElement* CUIElement::AddNewCheckBox(std::string const& name, int x, int y, int height, int width, std::wstring const& text, bool initState)
{
	std::shared_ptr<IUIElement> item = std::make_shared<CUICheckBox>(x, y, height, width, text, initState, this, m_renderer, m_textWriter);
	AddChild(name, item);
	return item.get();
}

IUIElement* CUIElement::AddNewComboBox(std::string const& name, int x, int y, int height, int width, std::vector<std::wstring> * items /*= nullptr*/)
{
	std::shared_ptr<IUIElement> item = std::make_shared<CUIComboBox>(x, y, height, width, this, m_renderer, m_textWriter);
	if(items)
	{
		for(auto i = items->begin(); i != items->end(); ++i)
		{
			item->AddItem(*i);
		}
	}
	AddChild(name, item);
	return item.get();
}

IUIElement* CUIElement::AddNewEdit(std::string const& name, int x, int y, int height, int width, std::wstring const& text)
{
	std::shared_ptr<IUIElement> item = std::make_shared<CUIEdit>(x, y, height, width, text, this, m_renderer, m_textWriter);
	AddChild(name, item);
	return item.get();
}

IUIElement* CUIElement::AddNewList(std::string const& name, int x, int y, int height, int width)
{
	std::shared_ptr<IUIElement> item = std::make_shared<CUIList>(x, y, height, width, this, m_renderer, m_textWriter);
	AddChild(name, item);
	return item.get();
}

IUIElement* CUIElement::AddNewRadioGroup(std::string const& name, int x, int y, int height, int width)
{
	std::shared_ptr<IUIElement> item = std::make_shared<CUIRadioGroup>(x, y, height, width, this, m_renderer, m_textWriter);
	AddChild(name, item);
	return item.get();
}

IUIElement* CUIElement::AddNewWindow(std::string const& name, int height, int width, std::wstring const& headerText)
{
	std::shared_ptr<IUIElement> item = std::make_shared<CUIWindow>(height, width, headerText, this, m_renderer, m_textWriter);
	AddChild(name, item);
	return item.get();
}

void CUIElement::SetFocus(IUIElement * focus)
{
	m_focused = focus;
	if(m_parent) m_parent->SetFocus(this);
}

bool CUIElement::IsFocused(const IUIElement * child) const
{
	if(m_focused != child)
		return false;
	if(m_parent != NULL)
		return m_parent->IsFocused(this);
	return true;
}

void CUIElement::SetTheme(std::shared_ptr<CUITheme> const& theme)
{
	m_theme = theme;
	Invalidate();
}

std::shared_ptr<CUITheme> CUIElement::GetTheme() const
{
	return m_theme;
}

std::wstring const CUIElement::GetText() const
{
	return L"";
}

void CUIElement::SetText(std::wstring const&)
{
	throw std::runtime_error("This UI element has no text");
}

void CUIElement::AddItem(std::wstring const&)
{
	throw std::runtime_error("This UI element has no items");
}

void CUIElement::DeleteItem(size_t)
{
	throw std::runtime_error("This UI element has no items");
}

size_t CUIElement::GetSelectedIndex() const
{
	throw std::runtime_error("This UI element has no items");
}

size_t CUIElement::GetItemsCount() const
{
	throw std::runtime_error("This UI element has no items");
}

std::wstring CUIElement::GetItem(size_t) const
{
	throw std::runtime_error("This UI element has no items");
}

void CUIElement::ClearItems()
{
	throw std::runtime_error("This UI element has no items");
}

void CUIElement::SetSelected(size_t)
{
	throw std::runtime_error("This UI element has no items");
}

void CUIElement::ClearChildren()
{
	m_children.clear();
	m_focused = NULL;
	InvalidateChildren();
}

int CUIElement::GetX() const
{
	return m_x * m_windowWidth / m_uiWidth;
}

int CUIElement::GetY() const
{
	return m_y * m_windowHeight / m_uiHeight;
}

int CUIElement::GetHeight() const
{
	return m_height * m_windowHeight / m_uiHeight;
}

int CUIElement::GetWidth() const
{
	return m_width * m_windowWidth / m_uiWidth;
}

void CUIElement::Resize(int windowHeight, int windowWidth)
{
	m_windowHeight = windowHeight;
	m_windowWidth = windowWidth;
	for(auto i = m_children.begin(); i != m_children.end(); ++i)
	{
		i->second->Resize(windowHeight, windowWidth);
	}
	Invalidate(true);
}

void CUIElement::SetOnChangeCallback(std::function<void()> const&)
{
	throw std::runtime_error("This UI element has no OnChange event");
}

void CUIElement::SetOnClickCallback(std::function<void()> const&)
{
	throw std::runtime_error("This UI element has no OnChange event");
}

void CUIElement::SetBackgroundImage(std::string const&)
{
	throw std::runtime_error("This UI element has no background image");
}
