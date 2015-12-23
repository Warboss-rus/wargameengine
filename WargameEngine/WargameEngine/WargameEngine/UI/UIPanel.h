#include "UIElement.h"

class CUIPanel : public CUIElement
{
public:
	CUIPanel(int x, int y, int height, int width, IUIElement * parent, IRenderer & renderer, ITextWriter & textWriter):CUIElement(x, y, height, width, parent, renderer, textWriter) {}
	void Draw() const override;
};