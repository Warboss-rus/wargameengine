#include "UICachedElement.h"

namespace wargameEngine
{
namespace ui
{
class UIButton : public UICachedElement
{
public:
	UIButton(int x, int y, int height, int width, std::wstring const& text, std::function<void()> const& onClick, IUIElement* parent);
	void DoPaint(IUIRenderer& renderer) const override;
	bool LeftMouseButtonUp(int x, int y) override;
	bool LeftMouseButtonDown(int x, int y) override;
	std::wstring const GetText() const override;
	void SetText(std::wstring const& text) override;
	void SetOnClickCallback(std::function<void()> const& onClick) override;
	void SetBackgroundImage(Path const& image) override;

private:
	std::wstring m_text;
	Path m_backgroundImage;
	std::function<void()> m_onClick;
	bool m_isPressed;
};
}
}