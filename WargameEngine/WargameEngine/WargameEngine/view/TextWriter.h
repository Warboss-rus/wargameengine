#include <ft2build.h>
#include FT_FREETYPE_H
#include <string>
#include <map>
#include <set>
#include <vector>

#pragma once

struct sSymbol
{
	char symbol;
	unsigned int size;
	FT_Face face;
	bool operator< (const sSymbol & other) const;
};
struct sGlyph
{
	unsigned int texture;
	int bitmap_left;
	int bitmap_top;
	int width;
	int rows;
	int advancex;
};

class CTextWriter
{
public:
	CTextWriter();
	~CTextWriter();
	void PrintText(int x, int y, std::string const& font, unsigned int size, std::string const& text, int width = 0, int height = 0);
	int GetStringHeight(std::string const& font, unsigned int size, std::string const& text);
	int GetStringWidth(std::string const& font, unsigned int size, std::string const& text);
private:
	FT_Face GetFace(std::string const& name);
	sGlyph GetSymbol(FT_Face font, unsigned int size, char symbol);
	void DrawBitmap(int x, int y, sGlyph  const& symbol);
	sGlyph CreateSymbol(sSymbol  const& s);
	FT_Library m_ft;
	std::map<std::string, FT_Face> m_faces;
	std::map<sSymbol, sGlyph> m_symbols;
};