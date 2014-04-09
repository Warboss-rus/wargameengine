#include "TextWriter.h"
#include <GL\glut.h>
#include <exception>

#ifndef GL_CLAMP_TO_EDGE_EXT 
#define GL_CLAMP_TO_EDGE_EXT 0x812F 
#endif 

CTextWriter::CTextWriter()
{
	if(FT_Init_FreeType(&m_ft))
	{
		throw std::exception("Cannot initialize FreeType library");
	}
}

FT_Face CTextWriter::GetFace(std::string const& name)
{
	if(m_faces.find(name) == m_faces.end())
	{
		if(FT_New_Face(m_ft, name.c_str(), 0, &m_faces[name]) && 
#ifdef _WIN32
			FT_New_Face(m_ft, (std::string(getenv("windir")) + "\\fonts\\" + name).c_str(), 0, &m_faces[name]))
#else
			true)
#endif
		{
			throw std::exception("Cannot get face");
		}
	}
	return m_faces[name];
}

sGlyph CTextWriter::CreateSymbol(sSymbol s)
{
	sGlyph symbol;
	FT_Face face = s.face;
	FT_Set_Pixel_Sizes(face, 0, s.size);
	wchar_t unicodeSymbol;
	mbstowcs(&unicodeSymbol, &s.symbol, 1);
	FT_Load_Char(face, unicodeSymbol, FT_LOAD_RENDER);
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_ALPHA, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE_EXT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE_EXT);
	symbol.texture = textureID;
	symbol.bitmap_left = face->glyph->bitmap_left;
	symbol.bitmap_top = face->glyph->bitmap_top;
	symbol.width = face->glyph->bitmap.width;
	symbol.rows = face->glyph->bitmap.rows;
	symbol.advancex = face->glyph->advance.x >> 6;
	return symbol;
}

sGlyph CTextWriter::GetSymbol(std::string const& font, unsigned int size, char symbol)
{
	sSymbol s;
	s.face = GetFace(font);
	s.size = size;
	s.symbol = symbol;
	if(m_symbols.find(s) == m_symbols.end())
	{
		m_symbols[s] = CreateSymbol(s);
	}
	return m_symbols[s];
}

void CTextWriter::DrawBitmap(int & x, int & y, sGlyph symbol)
{
	int x2 = x + symbol.bitmap_left;
    int y2 = y - symbol.bitmap_top;
    int w = symbol.width;
    int h = symbol.rows;
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2i(x2, y2);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2i(x2 + w, y2);
	glTexCoord2f(0.0f, 1.0f);
	glVertex2i(x2, y2+h);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2i(x2+w, y2+h);
	glEnd();
}

void CTextWriter::PrintText(int x, int y, std::string const& font, unsigned int size, std::string const& text, int width, int height)
{
	int newx = x;
	for (size_t i = 0; i < text.size(); ++i)
	{
		if(text[i] == '\n')
		{
			y += size;
			newx = x;
		}
		else
		{
			sGlyph glyph = GetSymbol(font, size, text[i]);
			if(width > 0 && newx + glyph.advancex > x + width)
			{
				newx += glyph.advancex;
				continue;
			}
			glBindTexture(GL_TEXTURE_2D, glyph.texture);
			DrawBitmap(newx, y, glyph);
			newx += glyph.advancex;
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
}

bool sSymbol::operator< (const sSymbol &other) const
{
	if(symbol < other.symbol) return true;
	if(symbol > other.symbol) return false;
	if(size < other.size) return true;
	if(size > other.size) return false;
	if(face < other.face) return true;
	if(face > other.face) return false;
	return false;
}

int CTextWriter::GetStringHeight(std::string const& font, unsigned int size, std::string const& text)
{
	int height = 0;
	for(size_t i = 0; i < text.size(); ++i)
	{
		int temp = GetSymbol(font, size, text[i]).rows;
		if(temp > height)
		{
			height = temp;
		}
	}
	return height;
}

int CTextWriter::GetStringWidth(std::string const& font, unsigned int size, std::string const& text)
{
	int width = 0;
	for(size_t i = 0; i < text.size(); ++i)
	{
		width += GetSymbol(font, size, text[i]).advancex;
	}
	return width;
}