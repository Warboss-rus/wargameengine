#include "UIElement.h"

class CUIButton : public CUIElement
{
public:
	CUIButton(int x, int y, int height, int width, char* text, void (onClick)()): 
		CUIElement(x, y, height, width), m_text(text), m_onClick(onClick), m_isPressed(false) {}
	void Draw() const;
	bool LeftMouseButtonUp(int x, int y);
	bool LeftMouseButtonDown(int x, int y);
private:
	std::string m_text;
	void ( * m_onClick)();
	bool m_isPressed;
};