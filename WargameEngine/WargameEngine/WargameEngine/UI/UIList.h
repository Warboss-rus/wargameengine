#include "UIElement.h"
#include "UIScrollBar.h"
#include <vector>

class CUIList : public CUIElement
{
public:
	CUIList(int x, int y, int height, int width, IUIElement * parent, IRenderer & renderer);
	virtual void Draw() const override;
	virtual bool LeftMouseButtonUp(int x, int y) override;
	virtual bool LeftMouseButtonDown(int x, int y) override;
	virtual void OnMouseMove(int x, int y) override;
	virtual void AddItem(std::wstring const& str) override;
	virtual void DeleteItem(size_t index) override;
	virtual std::wstring const GetText() const override;
	virtual int GetSelectedIndex() const override;
	virtual size_t GetItemsCount() const override;
	virtual std::wstring GetItem(size_t index) const override;
	virtual void ClearItems() override;
	virtual void SetSelected(size_t index) override;
	virtual void SetText(std::wstring const& text) override;
	virtual void SetOnChangeCallback(std::function<void()> const& onChange) override;
	virtual void Resize(int windowHeight, int windowWidth) override;
	virtual void SetTheme(std::shared_ptr<CUITheme> theme) override;
private:
	std::vector<std::wstring> m_items;
	int m_selected;
	std::function<void()> m_onChange;
	CUIScrollBar m_scrollbar;
};