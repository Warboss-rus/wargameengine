#include "UICheckBox.h"
#include "../view/IRenderer.h"
#include "UIText.h"

namespace wargameEngine
{
namespace ui
{
UICheckBox::UICheckBox(int x, int y, int height, int width, std::wstring const& text, bool initState, IUIElement* parent, view::ITextWriter& textWriter)
	: UIElement(x, y, height, width, parent, textWriter)
	, m_text(text)
	, m_state(initState)
	, m_pressed(false)
{
}

void UICheckBox::Draw(view::IRenderer& renderer) const
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
			renderer.SetTexture(m_theme->texture, true);
			auto& theme = m_theme->checkbox;
			float* texCoords = m_state ? theme.checkedTexCoord : theme.texCoord;
			int size = static_cast<int>(GetHeight() * theme.checkboxSizeCoeff);
			renderer.RenderArrays(RenderMode::TriangleStrip,
				{ CVector2i(0, 0), { 0, size }, { size, 0 }, { size, size } },
				{ CVector2f(texCoords), { texCoords[0], texCoords[3] }, { texCoords[2], texCoords[1] }, { texCoords[2], texCoords[3] } });
			PrintText(renderer, m_textWriter, static_cast<int>(size) + 1, 0, GetWidth(), GetHeight(), m_text, m_theme->text, m_scale);
		}, *m_cache, GetWidth(), GetHeight());
	}
	renderer.SetTexture(*m_cache);
	renderer.RenderArrays(RenderMode::TriangleStrip,
		{ CVector2i(0, 0), { GetWidth(), 0 }, { 0, GetHeight() }, { GetWidth(), GetHeight() } },
		{ CVector2f(0.0f, 0.0f), { 1.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } });
	UIElement::Draw(renderer);
	renderer.PopMatrix();
}

bool UICheckBox::LeftMouseButtonUp(int x, int y)
{
	if (!m_visible)
		return false;
	Invalidate();
	if (UIElement::LeftMouseButtonUp(x, y))
	{
		m_pressed = false;
		return true;
	}
	if (m_pressed && PointIsOnElement(x, y))
	{
		m_state = !m_state;
		m_pressed = false;
		return true;
	}
	m_pressed = false;
	return false;
}

bool UICheckBox::LeftMouseButtonDown(int x, int y)
{
	if (!m_visible)
		return false;
	Invalidate();
	if (UIElement::LeftMouseButtonDown(x, y))
	{
		return true;
	}
	if (PointIsOnElement(x, y))
	{
		m_pressed = true;
		return true;
	}
	return false;
}

void UICheckBox::SetState(bool state)
{
	m_state = state;
	Invalidate();
}

bool UICheckBox::GetState() const
{
	return m_state;
}

std::wstring const UICheckBox::GetText() const
{
	return m_text;
}

void UICheckBox::SetText(std::wstring const& text)
{
	m_text = text;
	Invalidate();
}
}
}