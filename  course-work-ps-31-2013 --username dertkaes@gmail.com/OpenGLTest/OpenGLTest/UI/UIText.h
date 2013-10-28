#include <string>
#include "UITheme.h"
#pragma once

class CUIText
{
public:
	CUIText(int x, int y, int height, int width, char* text, CUITheme::sText theme = CUITheme::defaultTheme.text, CUITheme::sText::eAligment aligment = CUITheme::sText::left): 
		m_x(x), m_y(y + (height + theme.fontHeight) / 2), m_width(width), m_text(text), m_theme(theme) 	{ m_theme.aligment = aligment; }
	void Draw() const;
	std::string const& GetText() const { return m_text; }
	void SetText (std::string const& text) { m_text = text; }
	std::string & GetText() { return m_text; }
	CUITheme::sText & GetTheme() { return m_theme; }
private:
	int m_x;
	int m_y;
	int m_width;
	std::string m_text;
	CUITheme::sText m_theme;
};

void PrintText(int x, int y, std::string str, void* font);