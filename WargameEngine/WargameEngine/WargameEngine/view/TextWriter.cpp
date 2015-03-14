#include "TextWriter.h"
#include <GL/glew.h>
#include "gl.h"
#ifndef GL_CLAMP_TO_EDGE_EXT 
#define GL_CLAMP_TO_EDGE_EXT 0x812F 
#endif

CTextWriter::CTextWriter()
{
	if(FT_Init_FreeType(&m_ft))
	{
		throw std::exception();
	}
}

CTextWriter::~CTextWriter()
{
	/*for (auto i = m_symbols.begin(); i != m_symbols.end(); ++i)//causes a crash
	{
		glDeleteTextures(1, &i->second.texture);
	}
	for (auto i = m_lines.begin(); i != m_lines.end(); ++i)
	{
		glDeleteTextures(1, &i->second.texture);
	}*/
	FT_Done_FreeType(m_ft);
}

FT_Face CTextWriter::GetFace(std::string const& name)
{
	if(m_faces.find(name) == m_faces.end())
	{
		if(FT_New_Face(m_ft, name.c_str(), 0, &m_faces[name]) 
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

sGlyph CTextWriter::CreateSymbol(sSymbol  const& s)
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
	symbol.data = face->glyph->bitmap.buffer;
	return symbol;
}

sGlyph CTextWriter::GetSymbol(FT_Face font, unsigned int size, char symbol)
{
	sSymbol s;
	s.face = font;
	s.size = size;
	s.symbol = symbol;
	if(m_symbols.find(s) == m_symbols.end())
	{
		m_symbols[s] = CreateSymbol(s);
	}
	return m_symbols[s];
}

void CTextWriter::DrawBitmap(int x, int y, sGlyph const& symbol)
{
	int x2 = x + symbol.bitmap_left;
    int y2 = y - symbol.bitmap_top;
    int w = symbol.width;
    int h = symbol.rows;
	int vert[] = {
		x2, y2,
		x2 + w, y2,
		x2, y2 + h,
		x2 + w, y2 + h};
	float tex[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f };
	
	glVertexPointer(2, GL_INT, 0, vert);
	glTexCoordPointer(2, GL_FLOAT, 0, tex);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void CTextWriter::PrintText(int x, int y, std::string const& font, unsigned int size, std::string const& text, int width, int height)
{
	/*sTextLine line;
	line.face = GetFace(font);
	line.size = size;
	line.text = text;
	if (m_lines.find(line) == m_lines.end())
	{
		int w = 0;
		int h = 0;
		for (unsigned int i = 0; i < text.size(); ++i)
		{
			sGlyph glyph = GetSymbol(line.face, size, text[i]);
			w += glyph.width + 1;
			if (glyph.rows + glyph.bitmap_top > h) h = glyph.rows + glyph.bitmap_top;
		}
		//h++;
		GLuint tex;
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE_EXT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE_EXT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, w, h, 0, GL_ALPHA, GL_UNSIGNED_BYTE, NULL);
		int newx = 0;
		for (unsigned int i = 0; i < text.size(); ++i)
		{
			sGlyph glyph = GetSymbol(line.face, size, text[i]);
			wchar_t unicodeSymbol;
			mbstowcs(&unicodeSymbol, &text[i], 1);
			FT_Load_Char(line.face, unicodeSymbol, FT_LOAD_RENDER);
			glTexSubImage2D(GL_TEXTURE_2D, 0, newx + glyph.bitmap_left, h - glyph.bitmap_top, glyph.width, glyph.rows, GL_ALPHA, GL_UNSIGNED_BYTE, line.face->glyph->bitmap.buffer);
			newx += glyph.width + 1;
		}
		sRenderedLine rend;
		rend.texture = tex;
		rend.height = h;
		rend.width = w;
		m_lines[line] = rend;
	}
	sRenderedLine const& rendered = m_lines[line];
	glBindTexture(GL_TEXTURE_2D, rendered.texture);
	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2i(x, y);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2i(x + rendered.width, y);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2i(x, y - rendered.height);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2i(x + rendered.width, y - rendered.height);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);*/
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
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
			sGlyph glyph = GetSymbol(face, size, text[i]);
			if(width > 0 && newx + glyph.advancex > x + width)
			{
				newx += glyph.advancex;
				continue;
			}
			glBindTexture(GL_TEXTURE_2D, glyph.texture);
			DrawBitmap(newx, y, glyph);
			newx += glyph.advancex;
		}
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

bool sSymbol::operator< (const sSymbol &other) const
{
	return (symbol < other.symbol || size < other.size || face < other.face);
}

int CTextWriter::GetStringHeight(std::string const& font, unsigned int size, std::string const& text)
{
	int height = 0;
	FT_Face face = GetFace(font);
	for(size_t i = 0; i < text.size(); ++i)
	{
		int temp = GetSymbol(face, size, text[i]).rows;
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
	FT_Face face = GetFace(font);
	for(size_t i = 0; i < text.size(); ++i)
	{
		width += GetSymbol(face, size, text[i]).advancex;
	}
	return width;
}

bool sTextLine::operator< (const sTextLine &other) const
{
	return (text < other.text || size < other.size || face < other.face);
}