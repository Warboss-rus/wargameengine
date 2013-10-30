#include "UIElement.h"
#include <vector>
#include <string>

class CUIListBox : public CUIElement
{
public:
	CUIListBox(int x, int y, int height, int width, IUIElement * parent): CUIElement(x, y, height, width, parent), 
		m_selected(-1), m_expanded(false), m_pressed(false) { }
	void Draw() const;
	bool LeftMouseButtonDown(int x, int y);
	bool LeftMouseButtonUp(int x, int y);
	void AddItem(std::string const& str);
	void DeleteItem(size_t index);
	std::string const& GetText() const;
	int GetSelectedIndex() const { return m_selected; }
	size_t GetItemsCount() const { return m_items.size(); }
	std::string GetItem(size_t index) const { return m_items[index]; }
	void SetSelected(size_t index);
	int GetHeight() const;
private:
	std::vector<std::string> m_items;
	int m_selected;
	bool m_expanded;
	bool m_pressed;
};