#include "UICachedElement.h"
#include "UIScrollBar.h"
#include <vector>

namespace wargameEngine
{
namespace ui
{
class UIComboBox : public UICachedElement
{
public:
	UIComboBox(int x, int y, int height, int width, IUIElement* parent);
	void DoPaint(IUIRenderer& renderer) const override;
	bool LeftMouseButtonDown(int x, int y) override;
	bool LeftMouseButtonUp(int x, int y) override;
	void OnMouseMove(int x, int y) override;
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
	void Resize(int windowHeight, int windowWidth) override;
	void SetTheme(std::shared_ptr<UITheme> const& theme) override;
	void SetScale(float scale) override;
	int GetHeight() const override;

private:
	bool PointIsOnElement(int x, int y) const final;
	int GetHeaderHeight() const;

	std::vector<std::wstring> m_items;
	int m_selected;
	bool m_expanded;
	bool m_pressed;
	std::function<void()> m_onChange;
	UIScrollBar m_scrollbar;
};
}
}