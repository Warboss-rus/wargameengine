#pragma once
#include <memory>
#include "UITheme.h"
#include "..\view\IRenderer.h"

class CUIScrollBar
{
public:
	CUIScrollBar(std::shared_ptr<CUITheme> theme, IRenderer & renderer):m_theme(theme), m_renderer(&renderer) {}
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
	std::shared_ptr<CUITheme> m_theme;
	IRenderer * m_renderer;
};