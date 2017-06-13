#include "UIElement.h"

namespace wargameEngine
{
namespace ui
{
class UIButton : public UIElement
{
public:
	UIButton(int x, int y, int height, int width, std::wstring const& text, std::function<void()> const& onClick, IUIElement* parent, view::ITextWriter& textWriter);
	void Draw(view::IRenderer& renderer) const override;
	bool LeftMouseButtonUp(int x, int y) override;
	bool LeftMouseButtonDown(int x, int y) override;
	std::wstring const GetText() const override;
	void SetText(std::wstring const& text) override;
	void SetOnClickCallback(std::function<void()> const& onClick) override;
	void SetBackgroundImage(std::string const& image) override;

private:
	std::wstring m_text;
	std::string m_backgroundImage;
	std::function<void()> m_onClick;
	bool m_isPressed;
};
}
}