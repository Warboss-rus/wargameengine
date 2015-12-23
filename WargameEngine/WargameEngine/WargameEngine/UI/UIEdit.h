#include "UIElement.h"

class CUIEdit : public CUIElement
{
public:
	CUIEdit(int x, int y, int height, int width, std::wstring const& text, IUIElement * parent, IRenderer & renderer, ITextWriter & textWriter);
	void Draw() const override;
	bool OnCharacterInput(unsigned int key) override;
	bool OnKeyPress(int key, int modifiers) override;
	bool LeftMouseButtonUp(int x, int y) override;
	bool LeftMouseButtonDown(int x, int y) override;
	std::wstring const GetText() const override;
	void SetText(std::wstring const& text) override;
private:
	void SetCursorPos(int x);

	std::wstring m_text;
	size_t m_pos;
	size_t m_beginSelection;
	bool m_isPressed;
};