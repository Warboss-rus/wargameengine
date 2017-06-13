#include "UIElement.h"

namespace wargameEngine
{
namespace ui
{
class UIStaticText : public UIElement
{
public:
	UIStaticText(int x, int y, int height, int width, std::wstring const& text, IUIElement* parent, view::ITextWriter& textWriter);
	void Draw(view::IRenderer& renderer) const override;
	std::wstring const GetText() const override;
	void SetText(std::wstring const& text) override;

private:
	std::wstring m_text;
};
}
}