#pragma once
#include "IUI.h"
#include <unordered_map>
#include "IUIRenderer.h"

namespace wargameEngine
{
namespace ui
{
class UIElement : public IUIElement
{
public:
	UIElement() = default;
	void Draw(IUIRenderer& renderer) const override;
	IUIElement* GetChildByName(std::string const& name) override;
	void DeleteChild(std::string const& name) override;
	void DeleteChild(IUIElement* element) override;
	void ClearChildren() override;
	bool LeftMouseButtonDown(int x, int y) override;
	bool LeftMouseButtonUp(int x, int y) override;
	bool OnCharacterInput(wchar_t key) override;
	bool OnKeyPress(view::VirtualKey key, int modifiers) override;
	void OnMouseMove(int x, int y) override;
	int GetX() const override;
	int GetY() const override;
	int GetHeight() const override;
	int GetWidth() const override;
	void SetVisible(bool visible) override;
	bool GetVisible() const override;
	void SetFocus(IUIElement* focus = nullptr) override;
	bool IsFocused(const IUIElement* child) const override;
	void SetTheme(std::shared_ptr<UITheme> const& theme) override;
	std::shared_ptr<UITheme> GetTheme() const override;
	std::wstring const GetText() const override;
	void SetText(std::wstring const& text) override;
	void AddItem(std::wstring const& str) override;
	void DeleteItem(size_t index) override;
	size_t GetSelectedIndex() const override;
	size_t GetItemsCount() const override;
	std::wstring GetItem(size_t index) const override;
	void ClearItems() override;
	void SetSelected(size_t index) override;
	void Resize(int windowHeight, int windowWidth) override;
	void SetOnChangeCallback(std::function<void()> const& onChange) override;
	void SetOnClickCallback(std::function<void()> const& onClick) override;
	void SetBackgroundImage(Path const& image) override;
	void SetState(bool state) override;
	bool GetState() const override;
	void Invalidate(bool resetTexture = false) const override;
	void InvalidateChildren(bool resetTexture = false) const override;
	void SetTargetSize(int width, int height) override;
	void SetScale(float scale) override;

	IUIElement* AddNewButton(std::string const& name, int x, int y, int height, int width, std::wstring const& text, std::function<void()> const& onClick) override;
	IUIElement* AddNewStaticText(std::string const& name, int x, int y, int height, int width, std::wstring const& text) override;
	IUIElement* AddNewPanel(std::string const& name, int x, int y, int height, int width) override;
	IUIElement* AddNewCheckBox(std::string const& name, int x, int y, int height, int width, std::wstring const& text, bool initState) override;
	IUIElement* AddNewComboBox(std::string const& name, int x, int y, int height, int width, std::vector<std::wstring>* items = nullptr) override;
	IUIElement* AddNewEdit(std::string const& name, int x, int y, int height, int width, std::wstring const& text, IUITextHelper& textHelper) override;
	IUIElement* AddNewList(std::string const& name, int x, int y, int height, int width) override;
	IUIElement* AddNewRadioGroup(std::string const& name, int x, int y, int height, int width) override;
	IUIElement* AddNewWindow(std::string const& name, int height, int width, std::wstring const& headerText) override;

protected:
	using CachedTexture = IUIRenderer::CachedTexture;

	UIElement(int x, int y, int height, int width, IUIElement* parent);
	void AddChild(std::string const& name, std::shared_ptr<IUIElement> const& element);
	virtual bool PointIsOnElement(int x, int y) const;

	std::unordered_map<std::string, std::shared_ptr<IUIElement>> m_children;
	int m_x = 0;
	int m_y = 0;
	int m_height = 640;
	int m_width = 640;
	int m_windowHeight = 640;
	int m_windowWidth = 640;
	int m_uiWidth = 600;
	int m_uiHeight = 600;
	float m_scale = 1.0f;
	bool m_visible = true;
	mutable bool m_invalidated = true;
	mutable bool m_childrenInvalidated = true;
	IUIElement* m_parent = nullptr;
	IUIElement* m_focused = nullptr;
	std::shared_ptr<UITheme> m_theme;
	mutable CachedTexture m_cache;
	mutable CachedTexture m_childrenCache;
};
}
}