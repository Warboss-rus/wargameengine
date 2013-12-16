#include "UIElement.h"
#include <vector>

class CUIList : public CUIElement
{
public:
	CUIList(int x, int y, int height, int width, IUIElement * parent): CUIElement(x, y, height, width, parent), m_selected(-1), m_onChange(callback()) { }
	void Draw() const;
	bool LeftMouseButtonUp(int x, int y);
	void AddItem(std::string const& str);
	void DeleteItem(size_t index);
	std::string const GetText() const;
	int GetSelectedIndex() const { return m_selected; }
	size_t GetItemsCount() const { return m_items.size(); }
	std::string GetItem(size_t index) const { return m_items[index]; }
	void ClearItems() { m_items.clear(); m_selected = -1; }
	void SetSelected(size_t index);
	void SetText(std::string const& text);
	void SetOnChangeCallback(callback(onChange)) { m_onChange = onChange; }
private:
	std::vector<std::string> m_items;
	int m_selected;
	callback(m_onChange);
};