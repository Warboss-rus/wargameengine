#include "UIElement.h"
#include "UIButton.h"
#include "UIStaticText.h"
#include "UIPanel.h"
#include "UICheckBox.h"
#include "UIListBox.h"
#include "UIEdit.h"

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
	if(x > m_x && x < m_x + GetWidth()	&& y > m_y && y < m_y + GetHeight())
		return true;
	return false;
}

bool CUIElement::LeftMouseButtonDown(int x, int y)
{
	if(m_focused && m_focused->LeftMouseButtonDown(x - m_x, y - m_y))
		return true;
	for(auto i = m_children.begin(); i != m_children.end(); ++i)
	{
		if(i->second->LeftMouseButtonDown(x - m_x, y - m_y))
		{
			return true;
		}
	}
	return false;
}

bool CUIElement::LeftMouseButtonUp(int x, int y)
{
	if(m_focused && m_focused->LeftMouseButtonUp(x - m_x, y - m_y))
		return true;
	for(auto i = m_children.begin(); i != m_children.end(); ++i)
	{
		if(i->second->LeftMouseButtonUp(x - m_x, y - m_y))
		{
			return true;
		}
	}
	SetFocus();
	return false;
}

bool CUIElement::OnKeyPress(unsigned char key)
{
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
	for(auto i = m_children.begin(); i != m_children.end(); ++i)
	{
		if (i->second->OnSpecialKeyPress(key))
		{
			return true;
		}
	}
	return false;
}

IUIElement* CUIElement::AddNewButton(std::string const& name, int x, int y, int height, int width, char* text, void (onClick)())
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

IUIElement* CUIElement::AddNewListBox(std::string const& name, int x, int y, int height, int width, std::vector<std::string> items)
{
	std::shared_ptr<IUIElement> item = std::shared_ptr<IUIElement>(new CUIListBox(x, y, height, width, this));
	CUIListBox * listbox = (CUIListBox *)item.get();
	for(auto i = items.begin(); i != items.end(); i++)
	{
		listbox->AddItem(*i);
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