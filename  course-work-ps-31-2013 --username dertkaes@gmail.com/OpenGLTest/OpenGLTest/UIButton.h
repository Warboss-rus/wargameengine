#include "UIElement.h"

class CUIButton : public CUIElement
{
public:
	CUIButton(int x, int y, int height, int width, char* text, void (onClick)()): 
		m_text(text), m_onClick(onClick) { m_x = x; m_y = y; m_height = height; m_width = width; }
	void Draw() const;
	bool LeftMouseButtonUp(int x, int y);
private:
	std::string m_text;
	void ( * m_onClick)();
};