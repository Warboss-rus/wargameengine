#include <string>
#include "UITheme.h"
#pragma once

class CUIText
{
public:
	CUIText(int x, int y, int height, int width, char* text, CUITheme::sText theme = CUITheme::defaultTheme.text, CUITheme::sText::eAligment aligment = CUITheme::sText::left): 
		m_x(x), m_y(y + (height + theme.fontHeight) / 2), m_width(width), m_text(text), m_theme(theme) 	{ m_theme.aligment = aligment; }
	void Draw() const;
private:
	int m_x;
	int m_y;
	int m_width;
	std::string m_text;
	CUITheme::sText m_theme;
};