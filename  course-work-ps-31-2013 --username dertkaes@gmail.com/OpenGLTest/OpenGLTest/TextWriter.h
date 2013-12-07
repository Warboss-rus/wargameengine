#include <ft2build.h>"
#include FT_FREETYPE_H"
#include <string>
#include <map>
#include <set>

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
	float advancex;
};

class CTextWriter
{
public:
	CTextWriter();
	void PrintText(float x, float y, std::string const& font, unsigned int size, std::string const& text);
	int GetStringHeight(std::string const& font, unsigned int size, std::string const& text);
	int GetStringWidth(std::string const& font, unsigned int size, std::string const& text);
private:
	FT_Face GetFace(std::string const& name);
	sGlyph GetSymbol(std::string const& font, unsigned int size, char symbol);
	void DrawBitmap(float & x, float & y, sGlyph symbol);
	sGlyph CreateSymbol(sSymbol s);
	FT_Library m_ft;
	std::map<std::string, FT_Face> m_faces;
	std::map<sSymbol, sGlyph> m_symbols;
};