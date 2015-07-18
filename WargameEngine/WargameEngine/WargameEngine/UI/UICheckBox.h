#include "UIElement.h"

class CUICheckBox : public CUIElement
{
public:
	CUICheckBox(int x, int y, int height, int width, char* text, bool initState, IUIElement * parent, IRenderer & renderer);
	void Draw() const override;
	bool LeftMouseButtonUp(int x, int y) override;
	bool LeftMouseButtonDown(int x, int y) override;
	void SetState(bool state) override;
	bool GetState() const override;
	std::string const GetText() const override;
	void SetText(std::string const& text) override;
private:
	std::string m_text;
	bool m_state;
	bool m_pressed;
};