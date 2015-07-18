#include "UIElement.h"
#include <vector>

class CUIRadioGroup : public CUIElement
{
public:
	CUIRadioGroup(int x, int y, int height, int width, IUIElement * parent, IRenderer & renderer): CUIElement(x, y, height, width, parent, renderer), m_selected(-1) { }
	void Draw() const override;
	bool LeftMouseButtonUp(int x, int y) override;
	void AddItem(std::string const& str) override;
	void DeleteItem(size_t index) override;
	std::string const GetText() const override;
	int GetSelectedIndex() const override;
	size_t GetItemsCount() const override;
	std::string GetItem(size_t index) const override;
	void ClearItems() override;
	void SetSelected(size_t index) override;
	void SetText(std::string const& text) override;
	void SetOnChangeCallback(std::function<void()> const& onChange) override;
private:
	std::vector<std::string> m_items;
	int m_selected;
	std::function<void()> m_onChange;
};