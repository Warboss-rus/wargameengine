#pragma once
#include <memory>
#include <string>
#include <vector>
#include <functional>

class CUITheme;

class IUIElement
{
public:
	virtual ~IUIElement() {}
	virtual void Draw() const = 0;
	virtual void AddChild(std::string const& name, std::shared_ptr<IUIElement> element) = 0;
	virtual IUIElement* GetChildByName(std::string const& name) = 0;
	virtual void DeleteChild(std::string const& name) = 0;
	virtual void DeleteChild(IUIElement * element) = 0;
	virtual void ClearChildren() = 0;
	virtual bool LeftMouseButtonDown(int x, int y) = 0;
	virtual bool LeftMouseButtonUp(int x, int y) = 0;
	virtual bool OnCharacterInput(unsigned int key) = 0;
	virtual bool OnKeyPress(int key, int modifiers) = 0;
	virtual void OnMouseMove(int x, int y) = 0;
	virtual int GetX() const = 0;
	virtual int GetY() const = 0;
	virtual int GetHeight() const = 0;
	virtual int GetWidth() const = 0;
	virtual void SetVisible(bool visible) = 0;
	virtual bool GetVisible() const = 0;
	virtual bool IsFocused(const IUIElement * child) const = 0;
	virtual void SetTheme(std::shared_ptr<CUITheme> theme) = 0;
	virtual std::shared_ptr<CUITheme> GetTheme() const = 0;
	virtual std::wstring const GetText() const = 0;
	virtual void SetText(std::wstring const& text) = 0;
	virtual void AddItem(std::wstring const& str) = 0;
	virtual void DeleteItem(size_t index) = 0;
	virtual void ClearItems() = 0;
	virtual size_t GetSelectedIndex() const = 0;
	virtual size_t GetItemsCount() const = 0;
	virtual std::wstring GetItem(size_t index) const = 0;
	virtual void SetSelected(size_t index) = 0;
	virtual void Resize(int windowHeight, int windowWidth) = 0;
	virtual void SetOnChangeCallback(std::function<void()> const& onChange) = 0;
	virtual void SetOnClickCallback(std::function<void()> const& onClick) = 0;
	virtual void SetBackgroundImage(std::string const& image) = 0;
	virtual void SetState(bool state) = 0;
	virtual bool GetState() const = 0;
	virtual void Invalidate() const = 0;

	virtual IUIElement* AddNewButton(std::string const& name, int x, int y, int height, int width, std::wstring const& text, std::function<void()> const& onClick) = 0;
	virtual IUIElement* AddNewStaticText(std::string const& name, int x, int y, int height, int width, std::wstring const& text) = 0;
	virtual IUIElement* AddNewPanel(std::string const& name, int x, int y, int height, int width) = 0;
	virtual IUIElement* AddNewCheckBox(std::string const& name, int x, int y, int height, int width, std::wstring const& text, bool initState) = 0;
	virtual IUIElement* AddNewComboBox(std::string const& name, int x, int y, int height, int width, std::vector<std::wstring> * items = nullptr) = 0;
	virtual IUIElement* AddNewEdit(std::string const& name, int x, int y, int height, int width, std::wstring const& text) = 0;
	virtual IUIElement* AddNewList(std::string const& name, int x, int y, int height, int width) = 0;
	virtual IUIElement* AddNewRadioGroup(std::string const& name, int x, int y, int height, int width) = 0;
	virtual IUIElement* AddNewWindow(std::string const& name, int height, int width, std::wstring const& headerText) = 0;
	virtual void SetFocus(IUIElement * focus = nullptr) = 0;
};