#include "UIText.h"
#include "UITheme.h"
#include "../view/GameView.h"

void PrintText(IRenderer & renderer, int x, int y, int width, int height, std::string const& str, CUITheme::sText const& theme)
{
	std::wstring wstr(str.begin(), str.end());
	PrintText(renderer, x, y, width, height, wstr, theme);
}

void PrintText(IRenderer & renderer, int x, int y, int width, int height, std::wstring const& str, CUITheme::sText const& theme)
{
	CTextWriter & text = CGameView::GetInstance().lock()->GetTextWriter();
	if(theme.aligment == theme.center)
		x = (width - text.GetStringWidth(theme.font, theme.fontSize, str)) / 2;
	if(theme.aligment == theme.right)
		x = width - text.GetStringWidth(theme.font, theme.fontSize, str);
	y += (height - theme.fontSize) / 2 + theme.fontSize;
	renderer.SetColor(theme.color);
	text.PrintText(x, y, theme.font, theme.fontSize, str);
	renderer.SetColor(0.0f, 0.0f, 0.0f);
}

int GetStringHeight(CUITheme::sText const& theme, std::string const& str)
{
	return CGameView::GetInstance().lock()->GetTextWriter().GetStringHeight(theme.font, theme.fontSize, str);
}

int GetStringHeight(CUITheme::sText const& theme, std::wstring const& str)
{
	return CGameView::GetInstance().lock()->GetTextWriter().GetStringHeight(theme.font, theme.fontSize, str);
}

int GetStringWidth(CUITheme::sText const& theme, std::string const& str)
{
	return CGameView::GetInstance().lock()->GetTextWriter().GetStringWidth(theme.font, theme.fontSize, str);
}

int GetStringWidth(CUITheme::sText const& theme, std::wstring const& str)
{
	return CGameView::GetInstance().lock()->GetTextWriter().GetStringWidth(theme.font, theme.fontSize, str);
}
