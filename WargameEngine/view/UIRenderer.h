#pragma once
#include "..\UI\IUIRenderer.h"

namespace wargameEngine
{
namespace view
{
class IRenderer;
class TextWriter;
class TextureManager;

class UIRenderer : public ui::IUIRenderer
{
public:
	UIRenderer(IRenderer& renderer, TextWriter& textWriter, TextureManager& textureManager);

	void Translate(int x, int y) override;
	void DrawTexturedRect(const ui::RectI& rect, const ui::RectF& texCoords, const Path& texture) override;
	void DrawRect(const ui::RectI& rect, const float* color) override;
	void DrawLine(ui::Point a, ui::Point b, const float* color) override;
	void DrawText(const ui::RectI& rect, std::wstring const& str, ui::UITheme::Text const& theme, float scale) override;
	CachedTexture CreateTexture(int width, int height) override;
	void RenderToTexture(CachedTexture& texture, const std::function<void() >& handler) override;
	void DrawCachedTexture(const CachedTexture& texture) override;
	int GetStringWidth(const std::wstring& text, const std::string& font, unsigned fontSize) override;
	int GetStringHeight(const std::wstring& text, const std::string& font, unsigned fontSize) override;
private:
	IRenderer& m_renderer;
	TextWriter& m_textWriter;
	TextureManager& m_textureManager;
	Path m_boundTexture;
	int m_transX = 0;
	int m_transY = 0;
};
}
}