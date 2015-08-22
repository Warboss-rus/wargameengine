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

void CUIElement::Draw() const
{
	if(!m_visible)
		return;
	for(auto i = m_children.begin(); i != m_children.end(); ++i)
	{
		if (i->second && i->second.get() != m_focused)
		{
			i->second->Draw();
		}
	}
	if(m_focused)
	{
		m_focused->Draw();
	}
}

void CUIElement::SetVisible(bool visible)
{
	m_visible = visible;
}

bool CUIElement::GetVisible() const
{
	return m_visible;
}

CUIElement::CUIElement(int x, int y, int height, int width, IUIElement * parent, IRenderer & renderer) : m_x(x), m_y(y), m_height(height), m_width(width),
m_visible(true), m_parent(parent), m_focused(nullptr), m_renderer(renderer)
{

}

CUIElement::CUIElement(IRenderer & renderer) :m_x(0), m_y(0), m_height(0), m_width(0), m_visible(true), m_parent(nullptr), m_focused(nullptr), m_windowHeight(640), m_windowWidth(640), m_renderer(renderer)
{

}

void CUIElement::AddChild(std::string const& name, std::shared_ptr<IUIElement> element)
{
	m_children[name] = element;
	element->SetTheme(m_theme);
	element->Resize(m_windowHeight, m_windowWidth);
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
}

void CUIElement::DeleteChild(IUIElement * element)
{
	auto it = std::find_if(m_children.begin(), m_children.end(), [element](auto& child) {return child.second.get() == element;});
	if (it != m_children.end())
	{
		m_children.erase(it);
	}
}

bool CUIElement::PointIsOnElement(int x, int y) const
{
	if(x > GetX() && x < GetX() + GetWidth()	&& y > GetY() && y < GetY() + GetHeight())
		return true;
	return false;
}

void CUIElement::Invalidate() const
{
	m_cache.reset();
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

bool CUIElement::OnKeyPress(unsigned char key)
{
	if(!m_visible) return false;
	for(auto i = m_children.begin(); i != m_children.end(); ++i)
	{
		if (i->second->OnKeyPress(key))
		{
			return true;
		}
	}
	return false;
}

bool CUIElement::OnSpecialKeyPress(int key)
{
	if(!m_visible) return false;
	for(auto i = m_children.begin(); i != m_children.end(); ++i)
	{
		if (i->second->OnSpecialKeyPress(key))
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
	std::shared_ptr<IUIElement> item = std::shared_ptr<IUIElement>(new CUIButton(x, y, height, width, text, onClick, this, m_renderer));
	AddChild(name, item);
	return item.get();
}

IUIElement* CUIElement::AddNewStaticText(std::string const& name, int x, int y, int height, int width, std::wstring const& text)
{
	std::shared_ptr<IUIElement> item = std::shared_ptr<IUIElement>(new CUIStaticText(x, y, height, width, text, this, m_renderer));
	AddChild(name, item);
	return item.get();
}

IUIElement* CUIElement::AddNewPanel(std::string const& name, int x, int y, int height, int width)
{
	std::shared_ptr<IUIElement> item = std::shared_ptr<IUIElement>(new CUIPanel(x, y, height, width, this, m_renderer));
	AddChild(name, item);
	return item.get();
}

IUIElement* CUIElement::AddNewCheckBox(std::string const& name, int x, int y, int height, int width, std::wstring const& text, bool initState)
{
	std::shared_ptr<IUIElement> item = std::shared_ptr<IUIElement>(new CUICheckBox(x, y, height, width, text, initState, this, m_renderer));
	AddChild(name, item);
	return item.get();
}

IUIElement* CUIElement::AddNewComboBox(std::string const& name, int x, int y, int height, int width, std::vector<std::wstring> * items /*= nullptr*/)
{
	std::shared_ptr<IUIElement> item = std::shared_ptr<IUIElement>(new CUIComboBox(x, y, height, width, this, m_renderer));
	if(items)
	{
		for(auto i = items->begin(); i != items->end(); i++)
		{
			item->AddItem(*i);
		}
	}
	AddChild(name, item);
	return item.get();
}

IUIElement* CUIElement::AddNewEdit(std::string const& name, int x, int y, int height, int width, std::wstring const& text)
{
	std::shared_ptr<IUIElement> item = std::shared_ptr<IUIElement>(new CUIEdit(x, y, height, width, text, this, m_renderer));
	AddChild(name, item);
	return item.get();
}

IUIElement* CUIElement::AddNewList(std::string const& name, int x, int y, int height, int width)
{
	std::shared_ptr<IUIElement> item = std::shared_ptr<IUIElement>(new CUIList(x, y, height, width, this, m_renderer));
	AddChild(name, item);
	return item.get();
}

IUIElement* CUIElement::AddNewRadioGroup(std::string const& name, int x, int y, int height, int width)
{
	std::shared_ptr<IUIElement> item = std::shared_ptr<IUIElement>(new CUIRadioGroup(x, y, height, width, this, m_renderer));
	AddChild(name, item);
	return item.get();
}

IUIElement* CUIElement::AddNewWindow(std::string const& name, int height, int width, std::wstring const& headerText)
{
	std::shared_ptr<IUIElement> item = std::shared_ptr<IUIElement>(new CUIWindow(height, width, headerText, this, m_renderer));
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

void CUIElement::SetTheme(std::shared_ptr<CUITheme> theme)
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

int CUIElement::GetSelectedIndex() const
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
}

int CUIElement::GetX() const
{
	return m_x * m_windowWidth / 600;
}

int CUIElement::GetY() const
{
	return m_y * m_windowHeight / 600;
}

int CUIElement::GetHeight() const
{
	return m_height * m_windowHeight / 600;
}

int CUIElement::GetWidth() const
{
	return m_width * m_windowWidth / 600;
}

void CUIElement::Resize(int windowHeight, int windowWidth)
{
	m_windowHeight = windowHeight;
	m_windowWidth = windowWidth;
	for(auto i = m_children.begin(); i != m_children.end(); ++i)
	{
		i->second->Resize(windowHeight, windowWidth);
	}
	Invalidate();
}

void CUIElement::SetOnChangeCallback(std::function<void()> const&)
{
	throw new std::runtime_error("This UI element has no OnChange event");
}

void CUIElement::SetOnClickCallback(std::function<void()> const&)
{
	throw new std::runtime_error("This UI element has no OnChange event");
}

void CUIElement::SetBackgroundImage(std::string const&)
{
	throw new std::runtime_error("This UI element has no background image");
}
