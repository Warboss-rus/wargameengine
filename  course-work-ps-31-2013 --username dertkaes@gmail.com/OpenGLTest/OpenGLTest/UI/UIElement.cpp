#include "UIElement.h"

void CUIElement::Draw() const
{
	for(auto i = m_children.begin(); i != m_children.end(); ++i)
	{
		i->second->Draw();
	}
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

bool IsPointOnUIElement(std::shared_ptr<IUIElement> element, int x, int y)
{
	if(x > element->GetX() && x < element->GetX() + element->GetWidth() 
		&& y > element->GetY() && y < element->GetY() + element->GetHeight())
		return true;
	return false;
}

bool CUIElement::LeftMouseButtonDown(int x, int y)
{
	for(auto i = m_children.begin(); i != m_children.end(); ++i)
	{
		if(IsPointOnUIElement(i->second, x, y))
		{
			if(i->second->LeftMouseButtonDown(x, y))
			{
				return true;
			}
		}
	}
	return false;
}

bool CUIElement::LeftMouseButtonUp(int x, int y)
{
	for(auto i = m_children.begin(); i != m_children.end(); ++i)
	{
		if(IsPointOnUIElement(i->second, x, y))
		{
			if(i->second->LeftMouseButtonUp(x, y))
			{
				return true;
			}
		}
	}
	return false;
}