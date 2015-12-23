#include "UIText.h"
#include "UITheme.h"
#include "..\view\ITextWriter.h"
#include "..\view\IRenderer.h"

void PrintText(IRenderer & renderer, ITextWriter & textWriter, int x, int y, int width, int height, std::string const& str, CUITheme::sText const& theme)
{
	std::wstring wstr(str.begin(), str.end());
	PrintText(renderer, textWriter, x, y, width, height, wstr, theme);
}

void PrintText(IRenderer & renderer, ITextWriter & textWriter, int x, int y, int width, int height, std::wstring const& str, CUITheme::sText const& theme)
{
	if(theme.aligment == theme.center)
		x = (width - textWriter.GetStringWidth(theme.font, theme.fontSize, str)) / 2;
	if(theme.aligment == theme.right)
		x = width - textWriter.GetStringWidth(theme.font, theme.fontSize, str);
	y += (height - theme.fontSize) / 2 + theme.fontSize;
	renderer.SetColor(theme.color);
	textWriter.PrintText(x, y, theme.font, theme.fontSize, str);
	renderer.SetColor(0.0f, 0.0f, 0.0f);
}
