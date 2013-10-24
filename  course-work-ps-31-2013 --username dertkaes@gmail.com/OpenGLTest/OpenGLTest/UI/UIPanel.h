#include "UIElement.h"

class CUIPanel : public CUIElement
{
public:
	CUIPanel(int x, int y, int height, int width):CUIElement(x, y, height, width) {}
	void Draw() const;
};