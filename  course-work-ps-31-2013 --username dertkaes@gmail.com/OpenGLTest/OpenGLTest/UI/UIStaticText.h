#include "UIElement.h"
#include "UIText.h"

class CUIStaticText : public CUIElement
{
public:
	CUIStaticText(int x, int y, int height, int width, char* text, IUIElement * parent): 
		CUIElement(x, y, height, width, parent), m_text(0, 0, height, width, text, m_theme.text) {}
	void Draw() const;
private:
	CUIText m_text;
};