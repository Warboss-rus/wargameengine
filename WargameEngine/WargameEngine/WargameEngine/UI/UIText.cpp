#include "UIText.h"
#include "UITheme.h"
#include "..\view\ITextWriter.h"
#include "..\view\IRenderer.h"

void PrintText(IRenderer & renderer, ITextWriter & textWriter, int x, int y, int width, int height, std::string const& str, CUITheme::sText const& theme, float scale)
{
	std::wstring wstr(str.begin(), str.end());
	PrintText(renderer, textWriter, x, y, width, height, wstr, theme, scale);
}

void PrintText(IRenderer & renderer, ITextWriter & textWriter, int x, int y, int width, int height, std::wstring const& str, CUITheme::sText const& theme, float scale)
{
	unsigned int textSize = static_cast<unsigned>(theme.fontSize * scale);
	if(theme.aligment == theme.center)
		x = (width - textWriter.GetStringWidth(theme.font, textSize, str)) / 2;
	if(theme.aligment == theme.right)
		x = width - textWriter.GetStringWidth(theme.font, textSize, str);
	y += (height - textSize) / 2 + textSize;
	renderer.SetColor(theme.color);
	textWriter.PrintText(x, y, theme.font, textSize, str);
	renderer.SetColor(0.0f, 0.0f, 0.0f);
}
