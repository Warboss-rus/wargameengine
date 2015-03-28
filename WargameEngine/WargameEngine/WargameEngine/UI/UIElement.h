#include "IUI.h"
#include <map>
#pragma once

class CUIElement : public IUIElement
{
public:
	CUIElement():m_x(0), m_y(0), m_height(0), m_width(0), m_visible(true), m_parent(NULL), m_focused(NULL), m_windowHeight(640), m_windowWidth(640) {}
	void Draw() const override;
	IUIElement* GetChildByName(std::string const& name) override;
	void DeleteChild(std::string const& name) override;
	void ClearChildren() override;
	bool LeftMouseButtonDown(int x, int y) override;
	bool LeftMouseButtonUp(int x, int y) override;
	bool OnKeyPress(unsigned char key) override;
	bool OnSpecialKeyPress(int key) override;
	int GetX() const override;
	int GetY() const override;
	int GetHeight() const override;
	int GetWidth() const override;
	void SetVisible(bool visible) override;
	bool GetVisible() const override { return m_visible; }
	void SetFocus(IUIElement * focus = NULL) override;
	bool IsFocused(const IUIElement * child) const override;
	void SetTheme(std::shared_ptr<CUITheme> theme) override { m_theme = theme; }
	std::shared_ptr<CUITheme> GetTheme() const override { return m_theme; }
	std::string const GetText() const override { return ""; }
	void SetText(std::string const& text) override;
	void AddItem(std::string const& str) override;
	void DeleteItem(size_t index) override;
	int GetSelectedIndex() const override;
	size_t GetItemsCount() const override;
	std::string GetItem(size_t index) const override;
	void ClearItems() override;
	void SetSelected(size_t index) override;
	void Resize(int windowHeight, int windowWidth) override;
	void SetOnChangeCallback(std::function<void()> const& onChange) override;
	void SetOnClickCallback(std::function<void()> const& onClick) override;
	void SetBackgroundImage(std::string const& image) override;
	void SetState(bool state) override;
	bool GetState() const override;

	IUIElement* AddNewButton(std::string const& name, int x, int y, int height, int width, char* text, std::function<void()> const& onClick) override;
	IUIElement* AddNewStaticText(std::string const& name, int x, int y, int height, int width, char* text) override;
	IUIElement* AddNewPanel(std::string const& name, int x, int y, int height, int width) override;
	IUIElement* AddNewCheckBox(std::string const& name, int x, int y, int height, int width, char* text, bool initState) override; 
	IUIElement* AddNewComboBox(std::string const& name, int x, int y, int height, int width, std::vector<std::string> * items = NULL) override;
	IUIElement* AddNewEdit(std::string const& name, int x, int y, int height, int width, char* text) override;
	IUIElement* AddNewList(std::string const& name, int x, int y, int height, int width) override;
	IUIElement* AddNewRadioGroup(std::string const& name, int x, int y, int height, int width) override;
protected:
	CUIElement(int x, int y, int height, int width, IUIElement * parent): m_x(x), m_y(y), m_height(height), m_width(width), 
		m_visible(true), m_parent(parent), m_focused(NULL) {}
	void AddChild(std::string const& name, std::shared_ptr<IUIElement> element);
	virtual bool PointIsOnElement(int x, int y) const;

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
	std::shared_ptr<CUITheme> m_theme;
};