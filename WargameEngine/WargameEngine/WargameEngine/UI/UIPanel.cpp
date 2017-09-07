#include "UIPanel.h"

namespace wargameEngine
{
namespace ui
{
UIPanel::UIPanel(int x, int y, int height, int width, IUIElement* parent)
	: UIElement(x, y, height, width, parent)
{
}

void UIPanel::Draw(IUIRenderer& renderer) const
{
	if (!m_visible)
		return;
	renderer.Translate(GetX(), GetY());
	renderer.DrawRect({ 0, 0, GetWidth(), GetHeight() }, m_theme->defaultColor);
	UIElement::Draw(renderer);
	renderer.Restore();
}
}
}