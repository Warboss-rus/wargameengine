#include "UIPanel.h"
#include "../view/IRenderer.h"
#include "UITheme.h"

namespace wargameEngine
{
namespace ui
{

UIPanel::UIPanel(int x, int y, int height, int width, IUIElement* parent, view::ITextWriter& textWriter)
	: UIElement(x, y, height, width, parent, textWriter)
{
}

void UIPanel::Draw(view::IRenderer& renderer) const
{
	if (!m_visible)
		return;
	renderer.PushMatrix();
	renderer.Translate(GetX(), GetY(), 0);
	renderer.UnbindTexture();
	renderer.SetColor(m_theme->defaultColor);
	renderer.RenderArrays(RenderMode::TriangleStrip,
		{ CVector2i(0, 0), { 0, GetHeight() }, { GetWidth(), 0 }, { GetWidth(), GetHeight() } }, {});
	renderer.SetColor(0, 0, 0);
	UIElement::Draw(renderer);
	renderer.PopMatrix();
}
}
}