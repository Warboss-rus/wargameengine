#include "TextWriter.h"
#include "IRenderer.h"

namespace wargameEngine
{
namespace view
{

TextWriter::TextWriter(ITextRasterizer& rasterizer)
	: m_rasterizer(rasterizer)
{
}

TextWriter::~TextWriter()
{
}

void TextWriter::Reset()
{
	m_faces.clear();
}

void TextWriter::DrawBitmap(IRenderer& renderer, int x, int y, const Glyph& symbol)
{
	int x2 = x + symbol.bitmap_left;
	int y2 = y - symbol.bitmap_top;
	int w = symbol.width;
	int h = symbol.rows;
	static const std::array<CVector2f, 4> tex = { CVector2f(0.0f, 0.0f),{ 1.0f, 0.0f },{ 0.0f, 1.0f },{ 1.0f, 1.0f } };
	renderer.RenderArrays(IRenderer::RenderMode::TriangleStrip, { CVector2i(x2, y2),{ x2 + w, y2 },{ x2, y2 + h },{ x2 + w, y2 + h } }, tex);
}

TextWriter::Glyph& TextWriter::GetSymbol(Face& face, std::string const& font, unsigned int size, wchar_t symbol)
{
	auto it = face.find(symbol);
	if (it == face.end())
	{
		auto info = m_rasterizer.GetGlyphInfo(symbol, font, size);
		it = face.emplace(std::make_pair(symbol, Glyph(std::move(info)))).first;
	}
	return it->second;
}

void TextWriter::CreateTextureIfNeeded(Glyph& glyph, IRenderer& renderer)
{
	if (!glyph.texture)
	{
		glyph.texture = renderer.CreateTexture(glyph.bitmapData.data(), glyph.width, glyph.rows, IRenderer::CachedTextureType::Alpha);
	}
}

void TextWriter::PrintText(IRenderer& renderer, int x, int y, std::string const& font, unsigned int size, std::wstring const& text, int width, int /*height*/)
{
	int newx = x;
	auto& face = m_faces[{font, size}];
	for (size_t i = 0; i < text.size(); ++i)
	{
		if (text[i] == L'\n')
		{
			y += size;
			newx = x;
		}
		else
		{
			Glyph& glyph = GetSymbol(face, font, size, text[i]);
			if (width > 0 && newx + glyph.advancex > x + width)
			{
				newx += glyph.advancex;
				continue;
			}
			CreateTextureIfNeeded(glyph, renderer);
			renderer.SetTexture(*glyph.texture);
			DrawBitmap(renderer, newx, y, glyph);
			newx += glyph.advancex;
		}
	}
}

int TextWriter::GetStringHeight(std::string const& font, unsigned int size, std::wstring const& text)
{
	int height = 0;
	auto& face = m_faces[{font, size}];
	for (size_t i = 0; i < text.size(); ++i)
	{
		int temp = GetSymbol(face, font, size, text[i]).rows;
		if (temp > height)
		{
			height = temp;
		}
	}
	return height;
}

int TextWriter::GetStringWidth(std::string const& font, unsigned int size, std::wstring const& text)
{
	int width = 0;
	auto& face = m_faces[{font, size}];
	for (size_t i = 0; i < text.size(); ++i)
	{
		width += GetSymbol(face, font, size, text[i]).advancex;
	}
	return width;
}
}
}