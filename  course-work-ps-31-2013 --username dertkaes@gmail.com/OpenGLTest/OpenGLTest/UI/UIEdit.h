#include "UIElement.h"
#include "UIText.h"

class CUIEdit : public CUIElement
{
public:
	CUIEdit(int x, int y, int height, int width, char* text, IUIElement * parent): 
		CUIElement(x, y, height, width, parent), m_isPressed(false), m_pos(0), m_beginSelection(0), 
		m_text(m_theme.edit.borderSize, m_theme.edit.borderSize, height - 2 *m_theme.edit.borderSize, width - 2 * m_theme.edit.borderSize, text, m_theme.text){}
	void Draw() const;
	bool OnKeyPress(unsigned char key);
	bool OnSpecialKeyPress(int key);
	bool LeftMouseButtonUp(int x, int y);
	bool LeftMouseButtonDown(int x, int y);
	std::string const& GetText() const { return m_text.GetText(); }
private:
	CUIText m_text;
	size_t m_pos;
	size_t m_beginSelection;
	bool m_isPressed;
};