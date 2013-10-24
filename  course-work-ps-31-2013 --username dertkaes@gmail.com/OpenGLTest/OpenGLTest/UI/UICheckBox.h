#include "UIElement.h"

class CUICheckBox : public CUIElement
{
public:
	CUICheckBox(int x, int y, int height, int width, char* text, bool initState):
		CUIElement(x, y, height, width), m_text(text), m_state(initState), m_pressed(false) {}
	void Draw() const;
	bool LeftMouseButtonUp(int x, int y);
	bool LeftMouseButtonDown(int x, int y);
	void SetState(bool state);
	bool GetState() const { return m_state; }
private:
	std::string m_text;
	bool m_state;
	bool m_pressed;
};