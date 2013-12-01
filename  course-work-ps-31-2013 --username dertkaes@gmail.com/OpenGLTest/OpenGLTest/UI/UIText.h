#include <string>
#include "UITheme.h"
#pragma once

void PrintText(int x, int y, int width, int height, std::string const& str, CUITheme::sText const& theme);
void PrintText(int x, int y, std::string const& str, void* font);