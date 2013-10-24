#include "UIElement.h"
#include "UIButton.h"
#include "UIStaticText.h"
#include "UIPanel.h"
#include "UICheckBox.h"
#include "UIListBox.h"

void CUIElement::Draw() const
{
	if(!m_visible)
		return;
	for(auto i = m_children.begin(); i != m_children.end(); ++i)
	{
		i->second->Draw();
	}
}

void CUIElement::SetVisible(bool visible)
{
	m_visible = visible;
}

void CUIElement::AddChild(std::string const& name, std::shared_ptr<IUIElement> element)
{
	m_children[name] = element;
}

std::shared_ptr<IUIElement> CUIElement::GetChildByName(std::string const& name)
{
	return m_children[name];
}

void CUIElement::DeleteChild(std::string const& name)
{
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
	for(auto i = m_children.begin(); i != m_children.end(); ++i)
	{
		if(i->second->LeftMouseButtonUp(x - m_x, y - m_y))
		{
			return true;
		}
	}
	return false;
}

std::shared_ptr<IUIElement> CUIElement::AddNewButton(std::string const& name, int x, int y, int height, int width, char* text, void (onClick)())
{
	std::shared_ptr<IUIElement> item = std::shared_ptr<IUIElement>(new CUIButton(x, y, height, width, text, onClick));
	AddChild(name, item);
	return item;
}

std::shared_ptr<IUIElement> CUIElement::AddNewStaticText(std::string const& name, int x, int y, int height, int width, char* text)
{
	std::shared_ptr<IUIElement> item = std::shared_ptr<IUIElement>(new CUIStaticText(x, y, height, width, text));
	AddChild(name, item);
	return item;
}

std::shared_ptr<IUIElement> CUIElement::AddNewPanel(std::string const& name, int x, int y, int height, int width)
{
	std::shared_ptr<IUIElement> item = std::shared_ptr<IUIElement>(new CUIPanel(x, y, height, width));
	AddChild(name, item);
	return item;
}

std::shared_ptr<IUIElement> CUIElement::AddNewCheckBox(std::string const& name, int x, int y, int height, int width, char* text, bool initState)
{
	std::shared_ptr<IUIElement> item = std::shared_ptr<IUIElement>(new CUICheckBox(x, y, height, width, text, initState));
	AddChild(name, item);
	return item;
}

std::shared_ptr<IUIElement> CUIElement::AddNewListBox(std::string const& name, int x, int y, int height, int width)
{
	std::shared_ptr<IUIElement> item = std::shared_ptr<IUIElement>(new CUIListBox(x, y, height, width));
	AddChild(name, item);
	return item;
}

std::shared_ptr<IUIElement> CUIElement::AddNewListBox(std::string const& name, int x, int y, int height, int width, std::vector<std::string> items)
{
	std::shared_ptr<IUIElement> item = std::shared_ptr<IUIElement>(new CUIListBox(x, y, height, width));
	CUIListBox * listbox = (CUIListBox *)item.get();
	for(auto i = items.begin(); i != items.end(); i++)
	{
		listbox->AddItem(*i);
	}
	AddChild(name, item);
	return item;
}