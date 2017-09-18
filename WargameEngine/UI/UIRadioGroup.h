#include "UICachedElement.h"
#include <vector>

namespace wargameEngine
{
namespace ui
{
class UIRadioGroup : public UICachedElement
{
public:
	UIRadioGroup(int x, int y, int height, int width, IUIElement* parent);
	void DoPaint(IUIRenderer& renderer) const override;
	bool LeftMouseButtonUp(int x, int y) override;
	void AddItem(std::wstring const& str) override;
	void DeleteItem(size_t index) override;
	std::wstring const GetText() const override;
	size_t GetSelectedIndex() const override;
	size_t GetItemsCount() const override;
	std::wstring GetItem(size_t index) const override;
	void ClearItems() override;
	void SetSelected(size_t index) override;
	void SetText(std::wstring const& text) override;
	void SetOnChangeCallback(std::function<void()> const& onChange) override;

private:
	std::vector<std::wstring> m_items;
	size_t m_selected;
	std::function<void()> m_onChange;
};
}
}