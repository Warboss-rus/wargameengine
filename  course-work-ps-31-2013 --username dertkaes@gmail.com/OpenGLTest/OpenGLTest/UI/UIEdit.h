#include "UIElement.h"

class CUIEdit : public CUIElement
{
public:
	CUIEdit(int x, int y, int height, int width, char* text, IUIElement * parent): 
		CUIElement(x, y, height, width, parent), m_text(text), m_isPressed(false), m_pos(0), m_beginSelection(0) {}
	void Draw() const;
	bool OnKeyPress(unsigned char key);
	bool OnSpecialKeyPress(int key);
	bool LeftMouseButtonUp(int x, int y);
	bool LeftMouseButtonDown(int x, int y);
	const std::string GetText() const { return m_text; }
private:
	std::string m_text;
	size_t m_pos;
	size_t m_beginSelection;
	bool m_isPressed;
};