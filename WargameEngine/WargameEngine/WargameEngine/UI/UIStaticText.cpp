#include "UIStaticText.h"

namespace wargameEngine
{
namespace ui
{
UIStaticText::UIStaticText(int x, int y, int height, int width, std::wstring const& text, IUIElement* parent)
	: UICachedElement(x, y, height, width, parent)
	, m_text(text)
{
}

void UIStaticText::DoPaint(IUIRenderer& renderer) const
{
	renderer.DrawText({ 0, 0, GetWidth(), GetHeight() }, m_text, m_theme->text, m_scale);
}

std::wstring const UIStaticText::GetText() const
{
	return m_text;
}

void UIStaticText::SetText(std::wstring const& text)
{
	m_text = text;
	Invalidate();
}
}
}