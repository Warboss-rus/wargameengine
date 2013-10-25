#include "UIElement.h"

class CUIPanel : public CUIElement
{
public:
	CUIPanel(int x, int y, int height, int width, IUIElement * parent):CUIElement(x, y, height, width, parent) {}
	void Draw() const;
};