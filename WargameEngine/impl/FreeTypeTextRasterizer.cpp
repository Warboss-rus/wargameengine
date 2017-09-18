#include "FreeTypeTextRasterizer.h"

using namespace wargameEngine;

FreeTypeTextRasterizer::FreeTypeTextRasterizer()
{
	if (FT_Init_FreeType(&m_ft))
	{
		throw std::exception();
	}
}

FreeTypeTextRasterizer::~FreeTypeTextRasterizer()
{
	FT_Done_FreeType(m_ft);
}

FreeTypeTextRasterizer::GlyphInfo FreeTypeTextRasterizer::GetGlyphInfo(wchar_t symbol, std::string const& font, unsigned int size)
{
	auto face = GetFace(font);
	GlyphInfo info;
	FT_Set_Pixel_Sizes(face, 0, size);
	FT_Load_Char(face, symbol, FT_LOAD_RENDER);

	info.bitmap_left = face->glyph->bitmap_left;
	info.bitmap_top = face->glyph->bitmap_top;
	info.width = face->glyph->bitmap.width;
	info.rows = face->glyph->bitmap.rows;
	info.advancex = face->glyph->advance.x >> 6;
	info.bitmapData.resize(face->glyph->bitmap.width * face->glyph->bitmap.rows);
	memcpy(info.bitmapData.data(), face->glyph->bitmap.buffer, info.bitmapData.size());
	return info;
}

void FreeTypeTextRasterizer::Reset()
{
	m_faces.clear();
}

FT_Face FreeTypeTextRasterizer::GetFace(std::string const& name)
{
	if (m_faces.find(name) == m_faces.end())
	{
		if (FT_New_Face(m_ft, name.c_str(), 0, &m_faces[name])
			&& FT_New_Face(m_ft, (m_customFontLocation + name).c_str(), 0, &m_faces[name])
#ifdef _WIN32
			&& FT_New_Face(m_ft, (std::string(getenv("windir")) + "\\fonts\\" + name).c_str(), 0, &m_faces[name]))
#elif __unix
			&& FT_New_Face(m_ft, ("/usr/share/fonts/" + name).c_str(), 0, &m_faces[name])
			&& FT_New_Face(m_ft, ("/usr/local/share/fonts/" + name).c_str(), 0, &m_faces[name])
			&& FT_New_Face(m_ft, ("~/.fonts/" + name).c_str(), 0, &m_faces[name])
			&& FT_New_Face(m_ft, ("/system/fonts/" + name).c_str(), 0, &m_faces[name]))
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

void FreeTypeTextRasterizer::AddFontLocation(std::string const& fontLocation)
{
	m_customFontLocation = fontLocation;
}
