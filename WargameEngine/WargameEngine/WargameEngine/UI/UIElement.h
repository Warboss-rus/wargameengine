#pragma once
#include "IUI.h"
#include <map>
#include "..\view\IRenderer.h"

namespace wargameEngine
{
namespace view
{
class ITextWriter;
}

namespace ui
{
class UIElement : public IUIElement
{
public:
	UIElement(view::ITextWriter& textWriter);
	~UIElement();
	void Draw(view::IRenderer& renderer) const override;
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
	void SetBackgroundImage(std::string const& image) override;
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
	IUIElement* AddNewEdit(std::string const& name, int x, int y, int height, int width, std::wstring const& text) override;
	IUIElement* AddNewList(std::string const& name, int x, int y, int height, int width) override;
	IUIElement* AddNewRadioGroup(std::string const& name, int x, int y, int height, int width) override;
	IUIElement* AddNewWindow(std::string const& name, int height, int width, std::wstring const& headerText) override;

protected:
	using CachedTextureType = view::IRenderer::CachedTextureType;
	using RenderMode = view::IRenderer::RenderMode;

	UIElement(int x, int y, int height, int width, IUIElement* parent, view::ITextWriter& textWriter);
	void AddChild(std::string const& name, std::shared_ptr<IUIElement> const& element);
	virtual bool PointIsOnElement(int x, int y) const;

	std::map<std::string, std::shared_ptr<IUIElement>> m_children;
	int m_x;
	int m_y;
	int m_height;
	int m_width;
	int m_windowHeight = 640;
	int m_windowWidth = 640;
	int m_uiWidth = 600;
	int m_uiHeight = 600;
	float m_scale = 1.0f;
	bool m_visible = true;
	mutable bool m_invalidated = true;
	mutable bool m_childrenInvalidated = true;
	IUIElement* m_parent;
	IUIElement* m_focused = nullptr;
	std::shared_ptr<UITheme> m_theme;
	view::ITextWriter& m_textWriter;
	mutable std::unique_ptr<view::ICachedTexture> m_cache;
	mutable std::unique_ptr<view::ICachedTexture> m_childrenCache;
};
}
}