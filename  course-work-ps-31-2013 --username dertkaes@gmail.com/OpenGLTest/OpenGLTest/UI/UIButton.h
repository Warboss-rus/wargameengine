#include "UIElement.h"
#include "UIText.h"

class CUIButton : public CUIElement
{
public:
	CUIButton(int x, int y, int height, int width, char* text, void (onClick)(), IUIElement * parent): 
		CUIElement(x, y, height, width, parent), m_text(0, 0, height, width, text, m_theme.text, m_theme.text.center), m_onClick(onClick), m_isPressed(false) {}
	void Draw() const;
	bool LeftMouseButtonUp(int x, int y);
	bool LeftMouseButtonDown(int x, int y);
private:
	CUIText m_text;
	void ( * m_onClick)();
	bool m_isPressed;
};