#pragma once
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include "..\ITextRasterizer.h"
#include "..\UI\IUIRenderer.h"

namespace wargameEngine
{
namespace view
{
class IRenderer;
class ICachedTexture;

class TextWriter : public ui::IUITextHelper
{
public:
	TextWriter(ITextRasterizer& rasterizer);
	~TextWriter();

	void Reset();
	void PrintText(IRenderer& renderer, int x, int y, std::string const& font, unsigned int size, std::wstring const& text, int width = 0, int height = 0);
	int GetStringHeight(std::wstring const& text, std::string const& font, unsigned int size) override;
	int GetStringWidth(std::wstring const& text, std::string const& font, unsigned int size) override;

private:
	struct Glyph : ITextRasterizer::GlyphInfo
	{
		Glyph(GlyphInfo&& info)
			: GlyphInfo(std::move(info))
		{}
		std::unique_ptr<ICachedTexture> texture;
	};
	using FaceKey = std::pair<std::string, unsigned>;
	using Face = std::unordered_map<wchar_t, Glyph>;

	void DrawBitmap(IRenderer& renderer, int x, int y, const Glyph& symbol);
	Glyph& GetSymbol(Face& face, std::string const& font, unsigned int size, wchar_t symbol);
	void CreateTextureIfNeeded(Glyph& glyph, IRenderer& renderer);

	ITextRasterizer& m_rasterizer;
	std::map<FaceKey, Face> m_faces;
};
}
}