#pragma once
#include <vector>

namespace wargameEngine
{
class ITextRasterizer
{
public:
	struct GlyphInfo
	{
		int bitmap_left;
		int bitmap_top;
		int width;
		int rows;
		int advancex;
		std::vector<std::byte> bitmapData;
	};

	virtual ~ITextRasterizer() = default;

	virtual GlyphInfo GetGlyphInfo(wchar_t symbol, std::string const& font, unsigned int size) = 0;
};
}