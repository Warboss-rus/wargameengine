#include "UICachedElement.h"

namespace wargameEngine
{
namespace ui
{

void UICachedElement::Draw(IUIRenderer& renderer) const
{
	if (!m_visible)
		return;
	ScopedTranslation translation(renderer, GetX(), GetY());
	if (!m_cache)
	{
		m_cache = renderer.CreateTexture(GetWidth(), GetHeight());
	}
	if (m_invalidated)
	{
		renderer.RenderToTexture(m_cache, [this, &renderer]() {
			DoPaint(renderer);
		});
	}
	renderer.DrawCachedTexture(m_cache);
	UIElement::Draw(renderer);
}

}
}

