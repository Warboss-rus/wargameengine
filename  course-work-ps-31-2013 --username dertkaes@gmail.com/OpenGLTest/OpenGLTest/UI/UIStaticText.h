#include "UIElement.h"
#include "UIUtils.h"

class CUIStaticText : public CUIElement
{
public:
	CUIStaticText(int x, int y, int height, int width, char* text): 
		CUIElement(x, y, height, width), m_text(text) {}
	void Draw() const;
private:
	std::string m_text;
};