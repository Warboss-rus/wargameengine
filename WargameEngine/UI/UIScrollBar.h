#pragma once
#include "UITheme.h"
#include <memory>

namespace wargameEngine
{
namespace ui
{
class IUIRenderer;

class UIScrollBar
{
public:
	UIScrollBar(std::shared_ptr<UITheme> theme);
	void Update(int size, int contentSize, int width, int step);
	void Draw(IUIRenderer& renderer) const;
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
	std::shared_ptr<UITheme> m_theme;
};
}
}