#include "UIElement.h"
#include "UIButton.h"
#include "UIStaticText.h"
#include "UIPanel.h"
#include "UICheckBox.h"
#include "UIComboBox.h"
#include "UIEdit.h"
#include "UIList.h"
#include "UIRadioGroup.h"

void CUIElement::Draw() const
{
	if(!m_visible)
		return;
	for(auto i = m_children.begin(); i != m_children.end(); ++i)
	{
		i->second->Draw();
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

bool CUIElement::PointIsOnElement(int x, int y) const
{
	if(x > GetX() && x < GetX() + GetWidth()	&& y > GetY() && y < GetY() + GetHeight())
		return true;
	return false;
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

IUIElement* CUIElement::AddNewButton(std::string const& name, int x, int y, int height, int width, char* text, callback(onClick))
{
	std::shared_ptr<IUIElement> item = std::shared_ptr<IUIElement>(new CUIButton(x, y, height, width, text, onClick, this));
	AddChild(name, item);
	return item.get();
}

IUIElement* CUIElement::AddNewStaticText(std::string const& name, int x, int y, int height, int width, char* text)
{
	std::shared_ptr<IUIElement> item = std::shared_ptr<IUIElement>(new CUIStaticText(x, y, height, width, text, this));
	AddChild(name, item);
	return item.get();
}

IUIElement* CUIElement::AddNewPanel(std::string const& name, int x, int y, int height, int width)
{
	std::shared_ptr<IUIElement> item = std::shared_ptr<IUIElement>(new CUIPanel(x, y, height, width, this));
	AddChild(name, item);
	return item.get();
}

IUIElement* CUIElement::AddNewCheckBox(std::string const& name, int x, int y, int height, int width, char* text, bool initState)
{
	std::shared_ptr<IUIElement> item = std::shared_ptr<IUIElement>(new CUICheckBox(x, y, height, width, text, initState, this));
	AddChild(name, item);
	return item.get();
}

IUIElement* CUIElement::AddNewComboBox(std::string const& name, int x, int y, int height, int width, std::vector<std::string> * items)
{
	std::shared_ptr<IUIElement> item = std::shared_ptr<IUIElement>(new CUIComboBox(x, y, height, width, this));
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

IUIElement* CUIElement::AddNewEdit(std::string const& name, int x, int y, int height, int width, char* text)
{
	std::shared_ptr<IUIElement> item = std::shared_ptr<IUIElement>(new CUIEdit(x, y, height, width, text, this));
	AddChild(name, item);
	return item.get();
}

IUIElement* CUIElement::AddNewList(std::string const& name, int x, int y, int height, int width)
{
	std::shared_ptr<IUIElement> item = std::shared_ptr<IUIElement>(new CUIList(x, y, height, width, this));
	AddChild(name, item);
	return item.get();
}

IUIElement* CUIElement::AddNewRadioGroup(std::string const& name, int x, int y, int height, int width)
{
	std::shared_ptr<IUIElement> item = std::shared_ptr<IUIElement>(new CUIRadioGroup(x, y, height, width, this));
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

void CUIElement::ClearChildren()
{
	m_children.clear();
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
}