#include "UICachedElement.h"

namespace wargameEngine
{
namespace ui
{
class UIStaticText : public UICachedElement
{
public:
	UIStaticText(int x, int y, int height, int width, std::wstring const& text, IUIElement* parent);
	void DoPaint(IUIRenderer& renderer) const override;
	std::wstring const GetText() const override;
	void SetText(std::wstring const& text) override;

private:
	std::wstring m_text;
};
}
}