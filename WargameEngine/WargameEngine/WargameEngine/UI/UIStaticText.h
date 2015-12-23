#include "UIElement.h"

class CUIStaticText : public CUIElement
{
public:
	CUIStaticText(int x, int y, int height, int width, std::wstring const& text, IUIElement * parent, IRenderer & renderer, ITextWriter & textWriter);
	void Draw() const override;
	std::wstring const GetText() const override;
	void SetText(std::wstring const& text) override;
private:
	std::wstring m_text;
};