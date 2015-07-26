#include "IUI.h"
#include <map>
#include "..\view\IRenderer.h"
#pragma once

class CUIElement : public IUIElement
{
public:
	CUIElement(IRenderer & renderer);
	virtual void Draw() const override;
	IUIElement* GetChildByName(std::string const& name) override;
	virtual void DeleteChild(std::string const& name) override;
	virtual void ClearChildren() override;
	virtual bool LeftMouseButtonDown(int x, int y) override;
	virtual bool LeftMouseButtonUp(int x, int y) override;
	virtual bool OnKeyPress(unsigned char key) override;
	virtual bool OnSpecialKeyPress(int key) override;
	virtual void OnMouseMove(int x, int y) override;
	virtual int GetX() const override;
	virtual int GetY() const override;
	virtual int GetHeight() const override;
	virtual int GetWidth() const override;
	virtual void SetVisible(bool visible) override;
	virtual bool GetVisible() const override;
	virtual void SetFocus(IUIElement * focus = nullptr) override;
	virtual bool IsFocused(const IUIElement * child) const override;
	virtual void SetTheme(std::shared_ptr<CUITheme> theme) override;
	virtual std::shared_ptr<CUITheme> GetTheme() const override;
	virtual std::wstring const GetText() const override;
	virtual void SetText(std::wstring const& text) override;
	virtual void AddItem(std::wstring const& str) override;
	virtual void DeleteItem(size_t index) override;
	virtual int GetSelectedIndex() const override;
	virtual size_t GetItemsCount() const override;
	virtual std::wstring GetItem(size_t index) const override;
	virtual void ClearItems() override;
	virtual void SetSelected(size_t index) override;
	virtual void Resize(int windowHeight, int windowWidth) override;
	virtual void SetOnChangeCallback(std::function<void()> const& onChange) override;
	virtual void SetOnClickCallback(std::function<void()> const& onClick) override;
	virtual void SetBackgroundImage(std::string const& image) override;
	virtual void SetState(bool state) override;
	virtual bool GetState() const override;
	virtual void Invalidate() const override;

	IUIElement* AddNewButton(std::string const& name, int x, int y, int height, int width, std::wstring const& text, std::function<void()> const& onClick) override;
	IUIElement* AddNewStaticText(std::string const& name, int x, int y, int height, int width, std::wstring const& text) override;
	IUIElement* AddNewPanel(std::string const& name, int x, int y, int height, int width) override;
	IUIElement* AddNewCheckBox(std::string const& name, int x, int y, int height, int width, std::wstring const& text, bool initState) override; 
	IUIElement* AddNewComboBox(std::string const& name, int x, int y, int height, int width, std::vector<std::wstring> * items = nullptr) override;
	IUIElement* AddNewEdit(std::string const& name, int x, int y, int height, int width, std::wstring const& text) override;
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
	mutable std::unique_ptr<ICachedTexture> m_cache;
};