#pragma once
#include <string>
#include "UITheme.h"
class IRenderer;
class ITextWriter;

void PrintText(IRenderer & renderer, ITextWriter & textWriter, int x, int y, int width, int height, std::string const& str, CUITheme::Text const& theme, float scale);
void PrintText(IRenderer & renderer, ITextWriter & textWriter, int x, int y, int width, int height, std::wstring const& str, CUITheme::Text const& theme, float scale);