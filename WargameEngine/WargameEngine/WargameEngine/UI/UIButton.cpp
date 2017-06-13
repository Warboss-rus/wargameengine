#include "UIButton.h"
#include "../view/IRenderer.h"
#include "UIText.h"

namespace wargameEngine
{
namespace ui
{

UIButton::UIButton(int x, int y, int height, int width, std::wstring const& text, std::function<void()> const& onClick, IUIElement* parent, view::ITextWriter& textWriter)
	: UIElement(x, y, height, width, parent, textWriter)
	, m_text(text)
	, m_onClick(onClick)
	, m_isPressed(false)
{
}

void UIButton::Draw(view::IRenderer& renderer) const
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
			if (m_backgroundImage.empty())
			{
				renderer.SetTexture(m_theme->texture, true);
				float* texCoord = m_isPressed ? m_theme->button.pressedTexCoord : m_theme->button.texCoord;
				renderer.RenderArrays(RenderMode::TriangleStrip,
					{ CVector2i(0, 0), { GetWidth(), 0 }, { 0, GetHeight() }, { GetWidth(), GetHeight() } },
					{ CVector2f(texCoord), { texCoord[2], texCoord[1] }, { texCoord[0], texCoord[3] }, { texCoord[2], texCoord[3] } });
			}
			else
			{
				renderer.SetTexture(m_theme->texture, true);
				renderer.RenderArrays(RenderMode::TriangleStrip,
					{ CVector2i(0, 0), { GetWidth(), 0 }, { 0, GetHeight() }, { GetWidth(), GetHeight() } },
					{ CVector2f(0.0f, 0.0f), { 1.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } });
			}
			PrintText(renderer, m_textWriter, 0, 0, GetWidth(), GetHeight(), m_text, m_theme->button.text, m_scale);
		}, *m_cache, GetWidth(), GetHeight());
	}
	renderer.SetTexture(*m_cache);
	renderer.RenderArrays(RenderMode::TriangleStrip,
		{ CVector2i(0, 0), { GetWidth(), 0 }, { 0, GetHeight() }, { GetWidth(), GetHeight() } },
		{ CVector2f(0.0f, 0.0f), { 1.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } });
	UIElement::Draw(renderer);
	renderer.PopMatrix();
}

bool UIButton::LeftMouseButtonUp(int x, int y)
{
	if (!m_visible)
		return false;
	Invalidate();
	if (UIElement::LeftMouseButtonUp(x, y))
	{
		m_isPressed = false;
		return true;
	}
	if (m_isPressed && PointIsOnElement(x, y))
	{
		m_onClick();
		SetFocus();
		m_isPressed = false;
		return true;
	}
	m_isPressed = false;
	return false;
}

bool UIButton::LeftMouseButtonDown(int x, int y)
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
		m_isPressed = true;
		return true;
	}
	return false;
}

std::wstring const UIButton::GetText() const
{
	return m_text;
}

void UIButton::SetText(std::wstring const& text)
{
	m_text = text;
	Invalidate();
}

void UIButton::SetOnClickCallback(std::function<void()> const& onClick)
{
	m_onClick = onClick;
}

void UIButton::SetBackgroundImage(std::string const& image)
{
	m_backgroundImage = image;
	Invalidate();
}
}
}