#include "UIElement.h"

class CUIButton : public CUIElement
{
public:
	CUIButton(int x, int y, int height, int width, char* text, std::function<void()> const& onClick, IUIElement * parent):
		CUIElement(x, y, height, width, parent), m_text(text), m_onClick(onClick), m_isPressed(false) {}
	void Draw() const override;
	bool LeftMouseButtonUp(int x, int y) override;
	bool LeftMouseButtonDown(int x, int y) override;
	std::string const GetText() const override;
	void SetText(std::string const& text) override;
	void SetOnClickCallback(std::function<void()> const& onClick) override;
	void SetBackgroundImage(std::string const& image) override;
private:
	std::string m_text;
	std::string m_backgroundImage;
	std::function<void()> m_onClick;
	bool m_isPressed;
};