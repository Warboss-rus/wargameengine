#include "IUI.h"
#include <map>
#include "..\view\IRenderer.h"
#pragma once

class CUIElement : public IUIElement
{
public:
	CUIElement(IRenderer & renderer);
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
	bool GetVisible() const override;
	void SetFocus(IUIElement * focus = nullptr) override;
	bool IsFocused(const IUIElement * child) const override;
	void SetTheme(std::shared_ptr<CUITheme> theme) override;
	std::shared_ptr<CUITheme> GetTheme() const override;
	std::string const GetText() const override;
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
	IUIElement* AddNewComboBox(std::string const& name, int x, int y, int height, int width, std::vector<std::string> * items = nullptr) override;
	IUIElement* AddNewEdit(std::string const& name, int x, int y, int height, int width, char* text) override;
	IUIElement* AddNewList(std::string const& name, int x, int y, int height, int width) override;
	IUIElement* AddNewRadioGroup(std::string const& name, int x, int y, int height, int width) override;
protected:
	CUIElement(int x, int y, int height, int width, IUIElement * parent, IRenderer & renderer);
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
	IRenderer & m_renderer;
};