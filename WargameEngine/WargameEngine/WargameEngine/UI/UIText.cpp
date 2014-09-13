#include "UIText.h"
#include "../gl.h"
#include "UITheme.h"
#include "../view/TextWriter.h"

CTextWriter text;

void PrintText(int x, int y, int width, int height, std::string const& str, CUITheme::sText const& theme)
{
	if(theme.aligment == theme.center)
		x = (width - text.GetStringWidth(theme.font, theme.fontSize, str)) / 2;
	if(theme.aligment == theme.right)
		x = width - text.GetStringWidth(theme.font, theme.fontSize, str);
	y += (height - theme.fontSize) / 2 + theme.fontSize;
	glColor3f(theme.color[0], theme.color[1], theme.color[2]);
	text.PrintText(x, y, "times.ttf", theme.fontSize, str);
	glColor3f(0.0f, 0.0f, 0.0f);
}

int GetStringHeight(CUITheme::sText const& theme, std::string const& str)
{
	return text.GetStringHeight(theme.font, theme.fontSize, str);
}

int GetStringWidth(CUITheme::sText const& theme, std::string const& str)
{
	return text.GetStringWidth(theme.font, theme.fontSize, str);
}