#include "UIElement.h"

class CUICheckBox : public CUIElement
{
public:
	CUICheckBox(int x, int y, int height, int width, char* text, bool initState, IUIElement * parent):
		CUIElement(x, y, height, width, parent), m_text(text), m_state(initState), m_pressed(false) {}
	void Draw() const;
	bool LeftMouseButtonUp(int x, int y);
	bool LeftMouseButtonDown(int x, int y);
	void SetState(bool state);
	bool GetState() const { return m_state; }
	std::string const GetText() const { return m_text; }
	void SetText(std::string const& text) { m_text = text; }
private:
	std::string m_text;
	bool m_state;
	bool m_pressed;
};