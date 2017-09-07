#include "UIElement.h"
#include "UIButton.h"
#include "UICheckBox.h"
#include "UIComboBox.h"
#include "UIEdit.h"
#include "UIList.h"
#include "UIPanel.h"
#include "UIRadioGroup.h"
#include "UIStaticText.h"
#include "UIWindow.h"
#include <algorithm>

namespace wargameEngine
{
namespace ui
{

void UIElement::Draw(IUIRenderer& renderer) const
{
	m_invalidated = false;
	if (!m_visible || m_children.empty())
		return;
	if (!m_childrenCache)
	{
		//m_childrenCache = renderer.CreateTexture(nullptr, GetWidth(), GetHeight(), CachedTextureType::RENDER_TARGET);
		m_childrenInvalidated = true;
	}
	if (m_childrenInvalidated)
	{
		//m_renderer.RenderToTexture([this] {
		for (auto i = m_children.begin(); i != m_children.end(); ++i)
		{
			if (i->second && i->second.get() != m_focused)
			{
				i->second->Draw(renderer);
			}
		}
		if (m_focused)
		{
			m_focused->Draw(renderer);
		}
		//}, *m_childrenCache, GetWidth(), GetHeight());
		//m_childrenInvalidated = false;
	}
	/*renderer.SetTexture(*m_childrenCache);
	renderer.RenderArrays(RenderMode::TRIANGLE_STRIP,
	{ CVector2i(0, 0),{ GetWidth(), 0 },{ 0,GetHeight() },{ GetWidth(), GetHeight() } },
	{ CVector2f(0.0f, 0.0f),{ 1.0f, 0.0f },{ 0.0f, 1.0f },{ 1.0f, 1.0f } });*/
}

void UIElement::SetVisible(bool visible)
{
	m_visible = visible;
}

bool UIElement::GetVisible() const
{
	return m_visible;
}

UIElement::UIElement(int x, int y, int height, int width, IUIElement* parent)
	: m_x(x)
	, m_y(y)
	, m_height(height)
	, m_width(width)
	, m_parent(parent)
{
}

void UIElement::AddChild(std::string const& name, std::shared_ptr<IUIElement> const& element)
{
	m_children[name] = element;
	element->SetTheme(m_theme);
	element->Resize(m_windowHeight, m_windowWidth);
	element->SetTargetSize(m_uiWidth, m_uiHeight);
	element->SetScale(m_scale);
	InvalidateChildren();
}

IUIElement* UIElement::GetChildByName(std::string const& name)
{
	return m_children[name].get();
}

void UIElement::DeleteChild(std::string const& name)
{
	if (m_children.find(name)->second.get() == m_focused)
	{
		m_focused = NULL;
	}
	m_children.erase(m_children.find(name));
	InvalidateChildren();
}

void UIElement::DeleteChild(IUIElement* element)
{
	auto it = std::find_if(m_children.begin(), m_children.end(), [element](auto& child) { return child.second.get() == element; });
	if (it != m_children.end())
	{
		m_children.erase(it);
	}
	InvalidateChildren();
}

bool UIElement::PointIsOnElement(int x, int y) const
{
	if (x > GetX() && x < GetX() + GetWidth() && y > GetY() && y < GetY() + GetHeight())
		return true;
	return false;
}

void UIElement::Invalidate(bool resetTexture) const
{
	m_invalidated = true;
	if (resetTexture)
		m_cache.reset();
	if (m_parent)
		m_parent->InvalidateChildren();
}

void UIElement::InvalidateChildren(bool resetTexture) const
{
	m_childrenInvalidated = true;
	if (resetTexture)
		m_childrenCache.reset();
	if (m_parent)
		m_parent->InvalidateChildren();
}

void UIElement::SetTargetSize(int width, int height)
{
	m_uiWidth = width;
	m_uiHeight = height;
	for (auto& child : m_children)
	{
		child.second->SetTargetSize(width, height);
	}
}

void UIElement::SetScale(float scale)
{
	m_scale = scale;
	Invalidate();
	for (auto& child : m_children)
	{
		child.second->SetScale(scale);
	}
}

void UIElement::SetState(bool)
{
	throw std::runtime_error("This UI element doesn't have state");
}

bool UIElement::GetState() const
{
	throw std::runtime_error("This UI element doesn't have state");
}

bool UIElement::LeftMouseButtonDown(int x, int y)
{
	if (!m_visible)
		return false;
	if (m_focused && m_focused->LeftMouseButtonDown(x - GetX(), y - GetY()))
		return true;
	for (auto i = m_children.begin(); i != m_children.end(); ++i)
	{
		if (i->second->LeftMouseButtonDown(x - GetX(), y - GetY()))
		{
			return true;
		}
	}
	return false;
}

bool UIElement::LeftMouseButtonUp(int x, int y)
{
	if (!m_visible)
		return false;
	if (m_focused && m_focused->LeftMouseButtonUp(x - GetX(), y - GetY()))
		return true;
	for (auto i = m_children.begin(); i != m_children.end(); ++i)
	{
		if (i->second->LeftMouseButtonUp(x - GetX(), y - GetY()))
		{
			return true;
		}
	}
	SetFocus();
	return false;
}

bool UIElement::OnCharacterInput(wchar_t key)
{
	if (!m_visible)
		return false;
	for (auto i = m_children.begin(); i != m_children.end(); ++i)
	{
		if (i->second->OnCharacterInput(key))
		{
			return true;
		}
	}
	return false;
}

bool UIElement::OnKeyPress(view::VirtualKey key, int modifiers)
{
	if (!m_visible)
		return false;
	for (auto i = m_children.begin(); i != m_children.end(); ++i)
	{
		if (i->second->OnKeyPress(key, modifiers))
		{
			return true;
		}
	}
	return false;
}

void UIElement::OnMouseMove(int x, int y)
{
	if (m_visible && m_focused)
		m_focused->OnMouseMove(x, y);
}

IUIElement* UIElement::AddNewButton(std::string const& name, int x, int y, int height, int width, std::wstring const& text, std::function<void()> const& onClick)
{
	std::shared_ptr<IUIElement> item = std::make_shared<UIButton>(x, y, height, width, text, onClick, this);
	AddChild(name, item);
	return item.get();
}

IUIElement* UIElement::AddNewStaticText(std::string const& name, int x, int y, int height, int width, std::wstring const& text)
{
	std::shared_ptr<IUIElement> item = std::make_shared<UIStaticText>(x, y, height, width, text, this);
	AddChild(name, item);
	return item.get();
}

IUIElement* UIElement::AddNewPanel(std::string const& name, int x, int y, int height, int width)
{
	std::shared_ptr<IUIElement> item = std::make_shared<UIPanel>(x, y, height, width, this);
	AddChild(name, item);
	return item.get();
}

IUIElement* UIElement::AddNewCheckBox(std::string const& name, int x, int y, int height, int width, std::wstring const& text, bool initState)
{
	std::shared_ptr<IUIElement> item = std::make_shared<UICheckBox>(x, y, height, width, text, initState, this);
	AddChild(name, item);
	return item.get();
}

IUIElement* UIElement::AddNewComboBox(std::string const& name, int x, int y, int height, int width, std::vector<std::wstring>* items /*= nullptr*/)
{
	std::shared_ptr<IUIElement> item = std::make_shared<UIComboBox>(x, y, height, width, this);
	if (items)
	{
		for (auto i = items->begin(); i != items->end(); ++i)
		{
			item->AddItem(*i);
		}
	}
	AddChild(name, item);
	return item.get();
}

IUIElement* UIElement::AddNewEdit(std::string const& name, int x, int y, int height, int width, std::wstring const& text)
{
	std::shared_ptr<IUIElement> item = std::make_shared<UIEdit>(x, y, height, width, text, this);
	AddChild(name, item);
	return item.get();
}

IUIElement* UIElement::AddNewList(std::string const& name, int x, int y, int height, int width)
{
	std::shared_ptr<IUIElement> item = std::make_shared<UIList>(x, y, height, width, this);
	AddChild(name, item);
	return item.get();
}

IUIElement* UIElement::AddNewRadioGroup(std::string const& name, int x, int y, int height, int width)
{
	std::shared_ptr<IUIElement> item = std::make_shared<UIRadioGroup>(x, y, height, width, this);
	AddChild(name, item);
	return item.get();
}

IUIElement* UIElement::AddNewWindow(std::string const& name, int height, int width, std::wstring const& headerText)
{
	std::shared_ptr<IUIElement> item = std::make_shared<CUIWindow>(height, width, headerText, this);
	AddChild(name, item);
	return item.get();
}

void UIElement::SetFocus(IUIElement* focus)
{
	m_focused = focus;
	if (m_parent)
		m_parent->SetFocus(this);
}

bool UIElement::IsFocused(const IUIElement* child) const
{
	if (m_focused != child)
		return false;
	if (m_parent != NULL)
		return m_parent->IsFocused(this);
	return true;
}

void UIElement::SetTheme(std::shared_ptr<UITheme> const& theme)
{
	m_theme = theme;
	Invalidate();
}

std::shared_ptr<UITheme> UIElement::GetTheme() const
{
	return m_theme;
}

std::wstring const UIElement::GetText() const
{
	return L"";
}

void UIElement::SetText(std::wstring const&)
{
	throw std::runtime_error("This UI element has no text");
}

void UIElement::AddItem(std::wstring const&)
{
	throw std::runtime_error("This UI element has no items");
}

void UIElement::DeleteItem(size_t)
{
	throw std::runtime_error("This UI element has no items");
}

size_t UIElement::GetSelectedIndex() const
{
	throw std::runtime_error("This UI element has no items");
}

size_t UIElement::GetItemsCount() const
{
	throw std::runtime_error("This UI element has no items");
}

std::wstring UIElement::GetItem(size_t) const
{
	throw std::runtime_error("This UI element has no items");
}

void UIElement::ClearItems()
{
	throw std::runtime_error("This UI element has no items");
}

void UIElement::SetSelected(size_t)
{
	throw std::runtime_error("This UI element has no items");
}

void UIElement::ClearChildren()
{
	m_children.clear();
	m_focused = NULL;
	InvalidateChildren();
}

int UIElement::GetX() const
{
	return m_x * m_windowWidth / m_uiWidth;
}

int UIElement::GetY() const
{
	return m_y * m_windowHeight / m_uiHeight;
}

int UIElement::GetHeight() const
{
	return m_height * m_windowHeight / m_uiHeight;
}

int UIElement::GetWidth() const
{
	return m_width * m_windowWidth / m_uiWidth;
}

void UIElement::Resize(int windowHeight, int windowWidth)
{
	m_windowHeight = windowHeight;
	m_windowWidth = windowWidth;
	for (auto i = m_children.begin(); i != m_children.end(); ++i)
	{
		i->second->Resize(windowHeight, windowWidth);
	}
	Invalidate(true);
}

void UIElement::SetOnChangeCallback(std::function<void()> const&)
{
	throw std::runtime_error("This UI element has no OnChange event");
}

void UIElement::SetOnClickCallback(std::function<void()> const&)
{
	throw std::runtime_error("This UI element has no OnChange event");
}

void UIElement::SetBackgroundImage(Path const&)
{
	throw std::runtime_error("This UI element has no background image");
}
}
}