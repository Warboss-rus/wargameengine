#include "UIRenderer.h"
#include "IRenderer.h"
#include "TextureManager.h"
#include "TextWriter.h"

namespace wargameEngine
{
namespace view
{
using namespace ui;

namespace
{
struct UITexture : public ui::ICachedTexture
{
	UITexture(std::unique_ptr<view::ICachedTexture>&& texture, int width, int height)
		: texture(std::move(texture)), width(width), height(height)
	{}
	std::unique_ptr<view::ICachedTexture> texture;
	int width;
	int height;
};
}

UIRenderer::UIRenderer(IRenderer& renderer, TextWriter& textWriter, TextureManager& textureManager)
	: m_renderer(renderer), m_textWriter(textWriter), m_textureManager(textureManager)
{
}

void UIRenderer::Translate(int x, int y)
{
	m_transX += x;
	m_transY += y;
}

void UIRenderer::DrawTexturedRect(const RectI& rect, const RectF& texCoords, const Path& texture)
{
	if (m_boundTexture != texture)
	{
		if (m_boundTexture.empty())
		{
			static constexpr float BLACK_COLOR[] = { 0.0f, 0.0f, 0.0f, 1.0f };
			m_renderer.SetColor(BLACK_COLOR);
		}
		m_boundTexture = texture;
		m_textureManager.LoadTextureNow(texture);
		m_renderer.SetTexture(*m_textureManager.GetTexturePtr(texture));
	}
	m_renderer.RenderArrays(IRenderer::RenderMode::TriangleStrip,
	{ CVector2i(rect.left + m_transX, rect.top + m_transY),{ rect.right + m_transX, rect.top + m_transY },{ rect.left + m_transX, rect.bottom + m_transY },{ rect.right + m_transX, rect.bottom + m_transY } },
	{ CVector2f(texCoords.left, texCoords.top),{ texCoords.right, texCoords.top },{ texCoords.left, texCoords.bottom },{ texCoords.right, texCoords.bottom } });
}

void UIRenderer::DrawRect(const RectI& rect, const float* color)
{
	if (!m_boundTexture.empty())
	{
		m_renderer.UnbindTexture();
		m_boundTexture.clear();
	}
	m_renderer.SetColor(color);
	m_renderer.RenderArrays(IRenderer::RenderMode::TriangleStrip, { CVector2i(rect.left + m_transX, rect.top + m_transY),{ rect.right + m_transX, rect.top },
	{ rect.left + m_transX, rect.bottom + m_transY },{ rect.right + m_transX, rect.bottom + m_transY } }, {});
}

void UIRenderer::DrawLine(Point a, Point b, const float* color)
{
	if (!m_boundTexture.empty())
	{
		m_renderer.UnbindTexture();
		m_boundTexture.clear();
	}
	m_renderer.SetColor(color);
	m_renderer.RenderArrays(IRenderer::RenderMode::Lines, { CVector2i(a.x + m_transX, a.y + m_transY),{ b.x + m_transX, b.y + m_transY }}, {});
}

void UIRenderer::DrawText(const RectI& rect, std::wstring const& str, UITheme::Text const& theme, float scale)
{
	unsigned int textSize = static_cast<unsigned>(theme.fontSize * scale);
	int x = rect.left;
	int y = rect.top;
	const int width = rect.right - rect.left;
	const int height = rect.bottom - rect.top;
	if (theme.aligment == UITheme::Text::Aligment::center)
		x = (width - m_textWriter.GetStringWidth(str, theme.font, textSize)) / 2;
	if (theme.aligment == UITheme::Text::Aligment::right)
		x = width - m_textWriter.GetStringWidth(str, theme.font, textSize);
	y += (height - textSize) / 2 + textSize;
	m_renderer.SetColor(theme.color);
	m_textWriter.PrintText(m_renderer, x + m_transX, y + m_transY, theme.font, textSize, str);
	m_renderer.UnbindTexture();
	m_boundTexture.clear();
}

IUIRenderer::CachedTexture UIRenderer::CreateTexture(int width, int height)
{
	return std::make_unique<UITexture>(m_renderer.CreateTexture(nullptr, width, height, IRenderer::CachedTextureType::RenderTarget), width, height);
}

void UIRenderer::RenderToTexture(CachedTexture& texture, const std::function<void() >& handler)
{
	auto& uitexture = reinterpret_cast<UITexture&>(*texture);
	m_renderer.RenderToTexture([this, &handler] {
		ScopedTranslation translation(*this, -m_transX, -m_transY);
		handler();
	}, *uitexture.texture, uitexture.width, uitexture.height);
}

void UIRenderer::DrawCachedTexture(const CachedTexture& texture)
{
	auto& uitexture = reinterpret_cast<UITexture&>(*texture);
	m_renderer.SetTexture(*uitexture.texture);
	m_renderer.RenderArrays(IRenderer::RenderMode::TriangleStrip,
	{ CVector2i(m_transX, m_transY),{ uitexture.width + m_transX, m_transY },{ m_transX, uitexture.height + m_transY },{ uitexture.width + m_transX, uitexture.height + m_transY } }, { { 0.0f, 0.0f },{ 1.0f, 0.0f },{ 0.0f, 1.0f },{ 1.0f, 1.0f } });
	m_renderer.UnbindTexture();
	m_boundTexture.clear();
}

int UIRenderer::GetStringWidth(const std::wstring& text, const std::string& font, unsigned fontSize)
{
	return m_textWriter.GetStringWidth(text, font, fontSize);
}

int UIRenderer::GetStringHeight(const std::wstring& text, const std::string& font, unsigned fontSize)
{
	return m_textWriter.GetStringHeight(text, font, fontSize);
}

}
}