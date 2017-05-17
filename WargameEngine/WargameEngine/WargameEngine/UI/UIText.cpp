#include "UIText.h"
#include "../view/IRenderer.h"
#include "../view/ITextWriter.h"
#include "UITheme.h"

namespace wargameEngine
{
namespace ui
{
void PrintText(view::IRenderer& renderer, view::ITextWriter& textWriter, int x, int y, int width, int height, std::string const& str, UITheme::Text const& theme, float scale)
{
	std::wstring wstr(str.begin(), str.end());
	PrintText(renderer, textWriter, x, y, width, height, wstr, theme, scale);
}

void PrintText(view::IRenderer& renderer, view::ITextWriter& textWriter, int x, int y, int width, int height, std::wstring const& str, UITheme::Text const& theme, float scale)
{
	unsigned int textSize = static_cast<unsigned>(theme.fontSize * scale);
	if (theme.aligment == UITheme::Text::Aligment::center)
		x = (width - textWriter.GetStringWidth(theme.font, textSize, str)) / 2;
	if (theme.aligment == UITheme::Text::Aligment::right)
		x = width - textWriter.GetStringWidth(theme.font, textSize, str);
	y += (height - textSize) / 2 + textSize;
	renderer.SetColor(theme.color);
	textWriter.PrintText(renderer, x, y, theme.font, textSize, str);
	renderer.SetColor(0, 0, 0);
}
}
}