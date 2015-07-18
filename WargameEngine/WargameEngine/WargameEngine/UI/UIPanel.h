#include "UIElement.h"

class CUIPanel : public CUIElement
{
public:
	CUIPanel(int x, int y, int height, int width, IUIElement * parent, IRenderer & renderer):CUIElement(x, y, height, width, parent, renderer) {}
	void Draw() const override;
};