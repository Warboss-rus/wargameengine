#include "UIElement.h"
#include <vector>
#include <string>

class CUIListBox : public CUIElement
{
public:
	CUIListBox(int x, int y, int height, int width): m_selected(-1), m_expanded(false) { m_x = x; m_y = y; m_height = height; m_width = width; }
	void Draw() const;
	bool LeftMouseButtonUp(int x, int y);
	void AddItem(std::string const& str);
	std::string GetSelectedItem() const;
	void SetSelected(int index);
	int GetHeight() const;
private:
	std::vector<std::string> m_items;
	int m_selected;
	bool m_expanded;
};