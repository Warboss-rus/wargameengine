#include "UIElement.h"

class CUIPanel : public CUIElement
{
public:
	CUIPanel(int x, int y, int height, int width) { m_x = x; m_y = y; m_height = height; m_width = width; }
	void Draw() const;
};