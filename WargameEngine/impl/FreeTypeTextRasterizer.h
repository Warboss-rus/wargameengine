#pragma once
#include "../ITextRasterizer.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <map>
#include <string>
#include <vector>

class FreeTypeTextRasterizer : public wargameEngine::ITextRasterizer
{
public:
	FreeTypeTextRasterizer();
	~FreeTypeTextRasterizer();

	GlyphInfo GetGlyphInfo(wchar_t symbol, std::string const& font, unsigned int size) override;

	void Reset();
	void AddFontLocation(std::string const& fontLocation);

private:
	FT_Face GetFace(std::string const& name);

	FT_Library m_ft;
	std::map<std::string, FT_Face> m_faces;
	std::string m_customFontLocation;
};