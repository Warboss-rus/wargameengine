#include "UIStaticText.h"
#include "../view/IRenderer.h"
#include "UIText.h"

namespace wargameEngine
{
namespace ui
{
UIStaticText::UIStaticText(int x, int y, int height, int width, std::wstring const& text, IUIElement* parent, view::ITextWriter& textWriter)
	: UIElement(x, y, height, width, parent, textWriter)
	, m_text(text)
{
}

void UIStaticText::Draw(view::IRenderer& renderer) const
{
	if (!m_visible)
		return;
	renderer.PushMatrix();
	renderer.Translate(GetX(), GetY(), 0);
	if (!m_cache)
	{
		m_cache = renderer.CreateTexture(nullptr, GetWidth(), GetHeight(), CachedTextureType::RenderTarget);
	}
	if (m_invalidated)
	{
		renderer.RenderToTexture([this, &renderer]() {
			PrintText(renderer, m_textWriter, 0, 0, GetWidth(), GetHeight(), m_text, m_theme->text, m_scale);
		}, *m_cache, GetWidth(), GetHeight());
	}
	renderer.SetTexture(*m_cache);
	renderer.RenderArrays(RenderMode::TriangleStrip,
		{ CVector2i(0, 0), { GetWidth(), 0 }, { 0, GetHeight() }, { GetWidth(), GetHeight() } },
		{ CVector2f(0.0f, 0.0f), { 1.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } });
	UIElement::Draw(renderer);
	renderer.PopMatrix();
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