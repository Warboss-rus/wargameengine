#include "UIElement.h"

class CUIEdit : public CUIElement
{
public:
	CUIEdit(int x, int y, int height, int width, char* text, IUIElement * parent, IRenderer & renderer): 
		CUIElement(x, y, height, width, parent, renderer), m_isPressed(false), m_pos(0), m_beginSelection(0), m_text(text){}
	void Draw() const override;
	bool OnKeyPress(unsigned char key) override;
	bool OnSpecialKeyPress(int key) override;
	bool LeftMouseButtonUp(int x, int y) override;
	bool LeftMouseButtonDown(int x, int y) override;
	std::string const GetText() const override;
	void SetText(std::string const& text) override;
private:
	std::string m_text;
	size_t m_pos;
	size_t m_beginSelection;
	bool m_isPressed;
};