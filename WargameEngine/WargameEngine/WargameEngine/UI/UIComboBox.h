#include "UIElement.h"
#include "UIScrollBar.h"
#include <vector>

class CUIComboBox : public CUIElement
{
public:
	CUIComboBox(int x, int y, int height, int width, IUIElement * parent): CUIElement(x, y, height, width, parent), 
		m_selected(-1), m_expanded(false), m_pressed(false), m_scrollbar(m_theme) { }
	void Draw() const override;
	bool LeftMouseButtonDown(int x, int y) override;
	bool LeftMouseButtonUp(int x, int y) override;
	void AddItem(std::string const& str) override;
	void DeleteItem(size_t index) override;
	std::string const GetText() const override;
	int GetSelectedIndex() const override;
	size_t GetItemsCount() const override;
	std::string GetItem(size_t index) const override;
	void ClearItems() override;
	void SetSelected(size_t index) override;
	bool PointIsOnElement(int x, int y) const override;
	void SetText(std::string const& text) override;
	void SetOnChangeCallback(std::function<void()> const& onChange) override;
	void Resize(int windowHeight, int windowWidth) override;
	void SetTheme(std::shared_ptr<CUITheme> theme) override;
private:
	std::vector<std::string> m_items;
	int m_selected;
	bool m_expanded;
	bool m_pressed;
	std::function<void()> m_onChange;
	CUIScrollBar m_scrollbar;
};