#include "UIElement.h"

class CUIStaticText : public CUIElement
{
public:
	CUIStaticText(int x, int y, int height, int width, char* text, IUIElement * parent, IRenderer & renderer): 
		CUIElement(x, y, height, width, parent, m_renderer), m_text(text) {}
	void Draw() const override;
	std::string const GetText() const override;
	void SetText(std::string const& text) override;
private:
	std::string m_text;
};