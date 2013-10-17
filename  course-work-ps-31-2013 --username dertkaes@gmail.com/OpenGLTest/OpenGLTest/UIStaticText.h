#include "UIElement.h"
#include "UIUtils.h"

class CUIStaticText : public CUIElement
{
public:
	CUIStaticText(int x, int y, int height, int width, char* text): 
		m_text(text) { m_x = x; m_y = y; m_height = height; m_width = width; }
	void Draw() const;
private:
	std::string m_text;
};