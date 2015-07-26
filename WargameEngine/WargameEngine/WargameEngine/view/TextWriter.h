#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include <string>
#include <map>
#include <set>
#include <vector>
#include "IRenderer.h"

struct sSymbol
{
	char symbol;
	wchar_t unicodeSymbol;
	unsigned int size;
	FT_Face face;
	bool operator< (const sSymbol & other) const;
};
struct sGlyph
{
	std::unique_ptr<ICachedTexture> texture;
	int bitmap_left;
	int bitmap_top;
	int width;
	int rows;
	int advancex;
};

class CTextWriter
{
public:
	CTextWriter(IRenderer & renderer);
	~CTextWriter();
	void PrintText(int x, int y, std::string const& font, unsigned int size, std::string const& text, int width = 0, int height = 0);
	void PrintText(int x, int y, std::string const& font, unsigned int size, std::wstring const& text, int width = 0, int height = 0);
	int GetStringHeight(std::string const& font, unsigned int size, std::string const& text);
	int GetStringWidth(std::string const& font, unsigned int size, std::string const& text);
	int GetStringHeight(std::string const& font, unsigned int size, std::wstring const& text);
	int GetStringWidth(std::string const& font, unsigned int size, std::wstring const& text);
private:
	FT_Face GetFace(std::string const& name);
	const sGlyph& GetSymbol(FT_Face font, unsigned int size, char symbol);
	const sGlyph& GetSymbol(FT_Face font, unsigned int size, wchar_t symbol);
	void DrawBitmap(int x, int y, sGlyph  const& symbol);
	sGlyph CreateSymbol(sSymbol  const& s);
	FT_Library m_ft;
	IRenderer & m_renderer;
	std::map<std::string, FT_Face> m_faces;
	std::map<sSymbol, sGlyph> m_symbols;
};