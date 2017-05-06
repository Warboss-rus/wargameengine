#include "UIPanel.h"
#include "../view/IRenderer.h"
#include "UITheme.h"

void CUIPanel::Draw() const
{
	if (!m_visible)
		return;
	m_renderer.PushMatrix();
	m_renderer.Translate(GetX(), GetY(), 0);
	m_renderer.UnbindTexture();
	m_renderer.SetColor(m_theme->defaultColor);
	m_renderer.RenderArrays(RenderMode::TRIANGLE_STRIP,
		{ CVector2i(0, 0), { 0, GetHeight() }, { GetWidth(), 0 }, { GetWidth(), GetHeight() } }, {});
	m_renderer.SetColor(0, 0, 0);
	CUIElement::Draw();
	m_renderer.PopMatrix();
}