#pragma once
#include <memory>
#include "UITheme.h"

class IRenderer;

class CUIScrollBar
{
public:
	CUIScrollBar(std::shared_ptr<CUITheme> theme, IRenderer & renderer);
	void Update(int size, int contentSize, int width, int step);
	void Draw() const;
	bool LeftMouseButtonDown(int x, int y);
	bool LeftMouseButtonUp(int x, int y);
	bool OnMouseMove(int x, int y);
	bool IsOnElement(int x, int y) const;
	int GetPosition() const;
	void SetScale(float scale);
private:
	float m_position;
	int m_size;
	int m_contentSize;
	int m_width;
	int m_mousePos;
	int m_step;
	float m_scale = 1.0f;
	bool m_pressed;
	bool m_upButtonPressed;
	bool m_downButtonPressed;
	std::shared_ptr<CUITheme> m_theme;
	IRenderer * m_renderer;
};