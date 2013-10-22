#include "IUI.h"
#include <map>
#pragma once

class CUIElement : public IUIElement
{
public:
	CUIElement():m_x(0), m_y(0), m_height(0), m_width(0) {}
	void Draw() const;
	void AddChild(std::string const& name, std::shared_ptr<IUIElement> element);
	std::shared_ptr<IUIElement> GetChildByName(std::string const& name);
	void DeleteChild(std::string const& name);
	bool LeftMouseButtonDown(int x, int y);
	bool LeftMouseButtonUp(int x, int y);
	int GetX() const { return m_x; }
	int GetY() const { return m_y; }
	int GetHeight() const { return m_height; }
	int GetWidth() const { return m_width; }
protected:
	std::map<std::string, std::shared_ptr<IUIElement>> m_children;
	int m_x;
	int m_y;
	int m_height;
	int m_width;
};