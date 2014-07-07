#include "UIElement.h"

class CUIStaticText : public CUIElement
{
public:
	CUIStaticText(int x, int y, int height, int width, char* text, IUIElement * parent): 
		CUIElement(x, y, height, width, parent), m_text(text) {}
	void Draw() const;
	std::string const GetText() const { return m_text; }
	void SetText(std::string const& text) { m_text = text; }
private:
	std::string m_text;
};