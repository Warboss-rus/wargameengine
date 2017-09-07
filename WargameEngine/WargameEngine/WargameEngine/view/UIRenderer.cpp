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
	m_renderer.PushMatrix();
	m_renderer.Translate(x, y);
}

void UIRenderer::Restore()
{
	m_renderer.PopMatrix();
}

void UIRenderer::DrawTexturedRect(const RectI& rect, const RectF& texCoords, const Path& texture)
{
	if (m_boundTexture != texture)
	{
		if (m_boundTexture.empty())
		{
			m_renderer.SetColor(0, 0, 0);
		}
		m_boundTexture = texture;
		m_textureManager.LoadTextureNow(texture);
		m_renderer.SetTexture(*m_textureManager.GetTexturePtr(texture));
	}
	m_renderer.RenderArrays(IRenderer::RenderMode::TriangleStrip,
	{ CVector2i(rect.left, rect.top),{ rect.right, rect.top },{ rect.left, rect.bottom },{ rect.right, rect.bottom } },
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
	m_renderer.RenderArrays(IRenderer::RenderMode::TriangleStrip, { CVector2i(rect.left, rect.top),{ rect.right, rect.top },{ rect.left, rect.bottom },{ rect.right, rect.bottom } }, {});
}

void UIRenderer::DrawLine(Point a, Point b, const float* color)
{
	if (!m_boundTexture.empty())
	{
		m_renderer.UnbindTexture();
		m_boundTexture.clear();
	}
	m_renderer.SetColor(color);
	m_renderer.RenderArrays(IRenderer::RenderMode::Lines, { CVector2i(a.x, a.y),{ b.x, b.y }}, {});
}

void UIRenderer::DrawText(const RectI& rect, std::wstring const& str, UITheme::Text const& theme, float scale)
{
	unsigned int textSize = static_cast<unsigned>(theme.fontSize * scale);
	int x = rect.left;
	int y = rect.top;
	const int width = rect.right - rect.left;
	const int height = rect.bottom - rect.top;
	if (theme.aligment == UITheme::Text::Aligment::center)
		x = (width - m_textWriter.GetStringWidth(theme.font, textSize, str)) / 2;
	if (theme.aligment == UITheme::Text::Aligment::right)
		x = width - m_textWriter.GetStringWidth(theme.font, textSize, str);
	y += (height - textSize) / 2 + textSize;
	m_renderer.SetColor(theme.color);
	m_textWriter.PrintText(m_renderer, x, y, theme.font, textSize, str);
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
	m_renderer.RenderToTexture(handler, *uitexture.texture, uitexture.width, uitexture.height);
}

void UIRenderer::DrawCachedTexture(const CachedTexture& texture)
{
	auto& uitexture = reinterpret_cast<UITexture&>(*texture);
	m_renderer.SetTexture(*uitexture.texture);
	m_renderer.RenderArrays(IRenderer::RenderMode::TriangleStrip,
	{ CVector2i(0, 0),{ uitexture.width, 0 },{ 0, uitexture.height },{ uitexture.width, uitexture.height } }, { { 0.0f, 0.0f },{ 1.0f, 0.0f },{ 0.0f, 1.0f },{ 1.0f, 1.0f } });
	m_renderer.UnbindTexture();
	m_boundTexture.clear();
}

int UIRenderer::GetStringWidth(const std::wstring& text, const std::string& font, unsigned fontSize) const
{
	return m_textWriter.GetStringWidth(font, fontSize, text);
}

int UIRenderer::GetStringHeight(const std::wstring& text, const std::string& font, unsigned fontSize) const
{
	return m_textWriter.GetStringHeight(font, fontSize, text);
}

}
}