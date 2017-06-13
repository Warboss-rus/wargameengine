#pragma once
#include "../view/ITextWriter.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include "../view/IRenderer.h"
#include <map>
#include <set>
#include <string>
#include <vector>

struct sSymbol
{
	char symbol;
	wchar_t unicodeSymbol;
	unsigned int size;
	FT_Face face;
	bool operator<(const sSymbol& other) const;
};
struct sGlyph
{
	std::unique_ptr<wargameEngine::view::ICachedTexture> texture;
	int bitmap_left;
	int bitmap_top;
	int width;
	int rows;
	int advancex;
	std::vector<unsigned char> bitmapData;
};

class CTextWriter : public wargameEngine::view::ITextWriter
{
public:
	CTextWriter();
	~CTextWriter();

	 void Reset() override;
	 void PrintText(wargameEngine::view::IRenderer& renderer, int x, int y, std::string const& font, unsigned int size, std::string const& text, int width = 0, int height = 0) override;
	 void PrintText(wargameEngine::view::IRenderer& renderer, int x, int y, std::string const& font, unsigned int size, std::wstring const& text, int width = 0, int height = 0) override;
	 int GetStringHeight(std::string const& font, unsigned int size, std::string const& text) override;
	 int GetStringWidth(std::string const& font, unsigned int size, std::string const& text) override;
	 int GetStringHeight(std::string const& font, unsigned int size, std::wstring const& text) override;
	 int GetStringWidth(std::string const& font, unsigned int size, std::wstring const& text) override;

	void AddFontLocation(std::string const& fontLocation);

private:
	FT_Face GetFace(std::string const& name);
	sGlyph& GetSymbol(FT_Face font, unsigned int size, char symbol);
	sGlyph& GetSymbol(FT_Face font, unsigned int size, wchar_t symbol);
	void DrawBitmap(wargameEngine::view::IRenderer& renderer, int x, int y, sGlyph const& symbol);
	sGlyph CreateSymbol(const sSymbol& s);
	void CreateTexture(sGlyph& glyph, wargameEngine::view::IRenderer& renderer);

	FT_Library m_ft;
	std::map<std::string, FT_Face> m_faces;
	std::map<sSymbol, sGlyph> m_symbols;
	std::string m_customFontLocation;
};