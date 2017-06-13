#pragma once
#include "UITheme.h"
#include <string>

namespace wargameEngine
{
namespace view
{
class IRenderer;
class ITextWriter;
}

namespace ui
{
void PrintText(view::IRenderer& renderer, view::ITextWriter& textWriter, int x, int y, int width, int height, std::string const& str, ui::UITheme::Text const& theme, float scale);
void PrintText(view::IRenderer& renderer, view::ITextWriter& textWriter, int x, int y, int width, int height, std::wstring const& str, ui::UITheme::Text const& theme, float scale);
}
}