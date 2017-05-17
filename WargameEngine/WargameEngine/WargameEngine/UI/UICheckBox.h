#include "UIElement.h"

namespace wargameEngine
{
namespace ui
{
class UICheckBox : public UIElement
{
public:
	UICheckBox(int x, int y, int height, int width, std::wstring const& text, bool initState, IUIElement* parent, view::ITextWriter& textWriter);
	void Draw(view::IRenderer& renderer) const override;
	bool LeftMouseButtonUp(int x, int y) override;
	bool LeftMouseButtonDown(int x, int y) override;
	void SetState(bool state) override;
	bool GetState() const override;
	std::wstring const GetText() const override;
	void SetText(std::wstring const& text) override;

private:
	std::wstring m_text;
	bool m_state;
	bool m_pressed;
};
}
}