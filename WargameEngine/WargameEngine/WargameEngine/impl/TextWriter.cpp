#include "TextWriter.h"
#include <algorithm>

CTextWriter::CTextWriter(IRenderer & renderer)
	:m_renderer(renderer)
{
	if(FT_Init_FreeType(&m_ft))
	{
		throw std::exception();
	}
}

CTextWriter::~CTextWriter()
{
	FT_Done_FreeType(m_ft);
}

FT_Face CTextWriter::GetFace(std::string const& name)
{
	if(m_faces.find(name) == m_faces.end())
	{
		if(FT_New_Face(m_ft, name.c_str(), 0, &m_faces[name]) 
			&& FT_New_Face(m_ft, (m_customFontLocation + name).c_str(), 0, &m_faces[name])
#ifdef _WIN32
			 && FT_New_Face(m_ft, (std::string(getenv("windir")) + "\\fonts\\" + name).c_str(), 0, &m_faces[name]))
#elif __unix
			&& FT_New_Face(m_ft, ("/usr/share/fonts/" + name).c_str(), 0, &m_faces[name])
			&& FT_New_Face(m_ft, ("/usr/local/share/fonts/" + name).c_str(), 0, &m_faces[name])
			&& FT_New_Face(m_ft, ("~/.fonts/" + name).c_str(), 0, &m_faces[name]))
#elif __APPLE__
			&& FT_New_Face(m_ft, ("~/Library/Fonts/" + name).c_str(), 0, &m_faces[name])
			&& FT_New_Face(m_ft, ("/System/Library/Fonts/" + name).c_str(), 0, &m_faces[name])
#else
			)
#endif
		{
			throw std::exception();
		}
	}
	return m_faces[name];
}

sGlyph CTextWriter::CreateSymbol(sSymbol const& s)
{
	sGlyph symbol;
	FT_Face face = s.face;
	FT_Set_Pixel_Sizes(face, 0, s.size);
	FT_Load_Char(face, s.unicodeSymbol, FT_LOAD_RENDER);

	symbol.texture = m_renderer.CreateTexture(face->glyph->bitmap.buffer, face->glyph->bitmap.width, face->glyph->bitmap.rows, CachedTextureType::ALPHA);
	symbol.bitmap_left = face->glyph->bitmap_left;
	symbol.bitmap_top = face->glyph->bitmap_top;
	symbol.width = face->glyph->bitmap.width;
	symbol.rows = face->glyph->bitmap.rows;
	symbol.advancex = face->glyph->advance.x >> 6;
	return symbol;
}

const sGlyph& CTextWriter::GetSymbol(FT_Face font, unsigned int size, char symbol)
{
	auto it = std::find_if(m_symbols.begin(), m_symbols.end(), [&](auto const& pair) {
		auto& s = pair.first;
		return s.face == font && s.size == size && s.symbol == symbol;
	});
	if (it == m_symbols.end())
	{
		sSymbol s;
		s.face = font;
		s.size = size;
		s.symbol = symbol;
		mbstowcs(&s.unicodeSymbol, &symbol, 1);
		m_symbols.emplace(s, CreateSymbol(s));
		m_symbols[s] = CreateSymbol(s);
		return m_symbols.at(s);
	}
	return it->second;
}

const sGlyph& CTextWriter::GetSymbol(FT_Face font, unsigned int size, wchar_t symbol)
{
	auto it = std::find_if(m_symbols.begin(), m_symbols.end(), [&](auto const& pair) {
		auto& s = pair.first;
		return s.face == font && s.size == size && s.unicodeSymbol == symbol;
	});
	if (it == m_symbols.end())
	{
		sSymbol s;
		s.face = font;
		s.size = size;
		s.unicodeSymbol = symbol;
		wcstombs(&s.symbol, &symbol, 1);
		m_symbols[s] = CreateSymbol(s);
		return m_symbols[s];
	}
	return it->second;
}

void CTextWriter::DrawBitmap(int x, int y, sGlyph const& symbol)
{
	int x2 = x + symbol.bitmap_left;
    int y2 = y - symbol.bitmap_top;
    int w = symbol.width;
    int h = symbol.rows;
	static const std::array<CVector2f, 4> tex = {CVector2f{0.0f, 0.0f},{1.0f, 0.0f},{0.0f, 1.0f}, {1.0f, 1.0f } };
	m_renderer.RenderArrays(RenderMode::TRIANGLE_STRIP,	{ CVector2i(x2, y2), { x2 + w, y2 }, { x2, y2 + h }, { x2 + w, y2 + h } }, tex);
}

void CTextWriter::PrintText(int x, int y, std::string const& font, unsigned int size, std::string const& text, int width, int /*height*/)
{
	int newx = x;
	FT_Face face = GetFace(font);
	
	for (size_t i = 0; i < text.size(); ++i)
	{
		if(text[i] == '\n')
		{
			y += size;
			newx = x;
		}
		else
		{
			const sGlyph& glyph = GetSymbol(face, size, text[i]);
			if(width > 0 && newx + glyph.advancex > x + width)
			{
				newx += glyph.advancex;
				continue;
			}
			m_renderer.SetTexture(*glyph.texture);
			DrawBitmap(newx, y, glyph);
			newx += glyph.advancex;
		}
	}
}

void CTextWriter::PrintText(int x, int y, std::string const& font, unsigned int size, std::wstring const& text, int width, int /*height*/)
{
	int newx = x;
	FT_Face face = GetFace(font);

	for (size_t i = 0; i < text.size(); ++i)
	{
		if (text[i] == '\n')
		{
			y += size;
			newx = x;
		}
		else
		{
			const sGlyph& glyph = GetSymbol(face, size, text[i]);
			if (width > 0 && newx + glyph.advancex > x + width)
			{
				newx += glyph.advancex;
				continue;
			}
			m_renderer.SetTexture(*glyph.texture);
			DrawBitmap(newx, y, glyph);
			newx += glyph.advancex;
		}
	}
}

bool sSymbol::operator< (const sSymbol &other) const
{
	if (symbol < other.symbol)
	{
		return true;
	}
	if (symbol > other.symbol)
	{
		return false;
	}
	if (size < other.size)
	{
		return true;
	}
	if (size > other.size)
	{
		return false;
	}
	if (face < other.face)
	{
		return true;
	}
	if (face > other.face)
	{
		return false;
	}
	return false;
}
int CTextWriter::GetStringHeight(std::string const& font, unsigned int size, std::string const& text)
{
	int height = 0;
	FT_Face face = GetFace(font);
	for (size_t i = 0; i < text.size(); ++i)
	{
		int temp = GetSymbol(face, size, text[i]).rows;
		if (temp > height)
		{
			height = temp;
		}
	}
	return height;
}

int CTextWriter::GetStringHeight(std::string const& font, unsigned int size, std::wstring const& text)
{
	int height = 0;
	FT_Face face = GetFace(font);
	for (size_t i = 0; i < text.size(); ++i)
	{
		int temp = GetSymbol(face, size, text[i]).rows;
		if (temp > height)
		{
			height = temp;
		}
	}
	return height;
}

int CTextWriter::GetStringWidth(std::string const& font, unsigned int size, std::string const& text)
{
	int width = 0;
	FT_Face face = GetFace(font);
	for (size_t i = 0; i < text.size(); ++i)
	{
		width += GetSymbol(face, size, text[i]).advancex;
	}
	return width;

}

int CTextWriter::GetStringWidth(std::string const& font, unsigned int size, std::wstring const& text)
{
	int width = 0;
	FT_Face face = GetFace(font);
	for (size_t i = 0; i < text.size(); ++i)
	{
		width += GetSymbol(face, size, text[i]).advancex;
	}
	return width;
}

void CTextWriter::AddFontLocation(std::string const& fontLocation)
{
	m_customFontLocation = fontLocation;
}
