#include "UIElement.h"
#include "UIText.h"

class CUICheckBox : public CUIElement
{
public:
	CUICheckBox(int x, int y, int height, int width, char* text, bool initState, IUIElement * parent):
		CUIElement(x, y, height, width, parent), m_text(m_height * m_theme.checkbox.checkboxSizeCoeff, 0 ,height, width, text, m_theme.text), m_state(initState), m_pressed(false) {}
	void Draw() const;
	bool LeftMouseButtonUp(int x, int y);
	bool LeftMouseButtonDown(int x, int y);
	void SetState(bool state);
	bool GetState() const { return m_state; }
	std::string const& GetText() const { return m_text.GetText(); }
	void SetText(std::string const& text) { m_text.SetText(text); }
private:
	CUIText m_text;
	bool m_state;
	bool m_pressed;
};