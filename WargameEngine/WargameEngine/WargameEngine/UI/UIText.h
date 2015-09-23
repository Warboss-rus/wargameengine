#pragma once
#include <string>
#include "UITheme.h"
#include "../view/IRenderer.h"

void PrintText(IRenderer & renderer, int x, int y, int width, int height, std::string const& str, CUITheme::sText const& theme);
void PrintText(IRenderer & renderer, int x, int y, int width, int height, std::wstring const& str, CUITheme::sText const& theme);
int GetStringHeight(CUITheme::sText const& theme, std::string const& str);
int GetStringWidth(CUITheme::sText const& theme, std::string const& str);
int GetStringHeight(CUITheme::sText const& theme, std::wstring const& str);
int GetStringWidth(CUITheme::sText const& theme, std::wstring const& str);