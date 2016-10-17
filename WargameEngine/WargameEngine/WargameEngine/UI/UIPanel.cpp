#include "UIPanel.h"
#include "UITheme.h"
#include "../view/IRenderer.h"

void CUIPanel::Draw() const
{
	if(!m_visible)
		return;
	m_renderer.PushMatrix();
	m_renderer.Translate(GetX(), GetY(), 0);
	m_renderer.SetColor(m_theme->defaultColor);
	m_renderer.RenderArrays(RenderMode::TRIANGLE_STRIP,
	{ CVector2i(0, 0), { 0, GetHeight() },{ GetWidth(), 0 }, { GetWidth(), GetHeight() } }, {});
	m_renderer.SetColor(0.0f,0.0f,0.0f);
	CUIElement::Draw();
	m_renderer.PopMatrix();
}