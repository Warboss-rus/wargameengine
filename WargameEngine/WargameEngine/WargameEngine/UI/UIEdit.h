#include "UICachedElement.h"

namespace wargameEngine
{
namespace ui
{
class UIEdit : public UICachedElement
{
public:
	UIEdit(int x, int y, int height, int width, std::wstring const& text, IUIElement* parent);
	void DoPaint(IUIRenderer& renderer) const override;
	bool OnCharacterInput(wchar_t key) override;
	bool OnKeyPress(view::VirtualKey key, int modifiers) override;
	bool LeftMouseButtonUp(int x, int y) override;
	bool LeftMouseButtonDown(int x, int y) override;
	std::wstring const GetText() const override;
	void SetText(std::wstring const& text) override;

private:
	void SetCursorPos(int x);

	std::wstring m_text;
	size_t m_pos;
	size_t m_beginSelection;
};
}
}