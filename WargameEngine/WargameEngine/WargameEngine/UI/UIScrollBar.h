#include "UITheme.h"
#pragma once

class CUIScrollBar
{
public:
	CUIScrollBar(CUITheme const& theme) { m_theme = theme; }
	void Update(int size, int contentSize, int width, int step);
	void Draw() const;
	bool LeftMouseButtonDown(int x, int y);
	bool LeftMouseButtonUp(int x, int y);
	bool IsOnElement(int x, int y) const;
	int GetPosition() const;
private:
	float m_position;
	int m_size;
	int m_contentSize;
	int m_width;
	int m_mousePos;
	int m_step;
	bool m_pressed;
	bool m_buttonPressed;
	CUITheme m_theme;
};