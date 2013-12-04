#include "IUI.h"
#include <map>
#pragma once

class CUIElement : public IUIElement
{
public:
	CUIElement():m_x(0), m_y(0), m_height(0), m_width(0), m_visible(true), m_parent(NULL), m_focused(NULL), m_theme(CUITheme::defaultTheme), m_windowHeight(640), m_windowWidth(640) {}
	void Draw() const;
	IUIElement* GetChildByName(std::string const& name);
	void DeleteChild(std::string const& name);
	void ClearChildren();
	bool LeftMouseButtonDown(int x, int y);
	bool LeftMouseButtonUp(int x, int y);
	bool OnKeyPress(unsigned char key);
	bool OnSpecialKeyPress(int key);
	int GetX() const;
	int GetY() const;
	int GetHeight() const;
	int GetWidth() const;
	bool PointIsOnElement(int x, int y) const;
	void SetVisible(bool visible);
	bool GetVisible() { return m_visible; }
	void SetFocus(IUIElement * focus = NULL);
	bool IsFocused(const IUIElement * child) const;
	void SetTheme(CUITheme const& theme) { m_theme = theme; }
	CUITheme & GetTheme() { return m_theme; }
	std::string const GetText() const { return ""; }
	void SetText(std::string const& text) {}
	void AddItem(std::string const& str) {}
	void DeleteItem(size_t index) {}
	int GetSelectedIndex() const { return -1; }
	size_t GetItemsCount() const { return 0; }
	std::string GetItem(size_t index) const { return ""; }
	void SetSelected(size_t index) {}
	void Resize(int windowHeight, int windowWidth);

	IUIElement* AddNewButton(std::string const& name, int x, int y, int height, int width, char* text, callback(onClick));
	IUIElement* AddNewStaticText(std::string const& name, int x, int y, int height, int width, char* text);
	IUIElement* AddNewPanel(std::string const& name, int x, int y, int height, int width);
	IUIElement* AddNewCheckBox(std::string const& name, int x, int y, int height, int width, char* text, bool initState);
	IUIElement* AddNewComboBox(std::string const& name, int x, int y, int height, int width, std::vector<std::string> * items = NULL);
	IUIElement* AddNewEdit(std::string const& name, int x, int y, int height, int width, char* text);
	IUIElement* AddNewList(std::string const& name, int x, int y, int height, int width);
protected:
	CUIElement(int x, int y, int height, int width, IUIElement * parent): m_x(x), m_y(y), m_height(height), m_width(width), 
		m_visible(true), m_parent(parent), m_focused(NULL) {}
	void AddChild(std::string const& name, std::shared_ptr<IUIElement> element);
	std::map<std::string, std::shared_ptr<IUIElement>> m_children;
	int m_x;
	int m_y;
	int m_height;
	int m_width;
	int m_windowHeight;
	int m_windowWidth;
	bool m_visible;
	IUIElement * m_parent;
	IUIElement * m_focused;
	CUITheme m_theme;
};