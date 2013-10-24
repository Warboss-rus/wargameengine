#include "IUI.h"
#include <map>
#pragma once

class CUIElement : public IUIElement
{
public:
	CUIElement():m_x(0), m_y(0), m_height(0), m_width(0), m_visible(true) {}
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
	bool PointIsOnElement(int x, int y) const;
	void SetVisible(bool visible);
	bool GetVisible() { return m_visible; }

	std::shared_ptr<IUIElement> AddNewButton(std::string const& name, int x, int y, int height, int width, char* text, void (onClick)());
	std::shared_ptr<IUIElement> AddNewStaticText(std::string const& name, int x, int y, int height, int width, char* text);
	std::shared_ptr<IUIElement> AddNewPanel(std::string const& name, int x, int y, int height, int width);
	std::shared_ptr<IUIElement> AddNewCheckBox(std::string const& name, int x, int y, int height, int width, char* text, bool initState);
	std::shared_ptr<IUIElement> AddNewListBox(std::string const& name, int x, int y, int height, int width);
	std::shared_ptr<IUIElement> AddNewListBox(std::string const& name, int x, int y, int height, int width, std::vector<std::string> items);
protected:
	CUIElement(int x, int y, int height, int width): m_x(x), m_y(y), m_height(height), m_width(width), m_visible(true) {}
	std::map<std::string, std::shared_ptr<IUIElement>> m_children;
	int m_x;
	int m_y;
	int m_height;
	int m_width;
	bool m_visible;
};