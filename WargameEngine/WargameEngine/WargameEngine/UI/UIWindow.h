#pragma once
#include "UIElement.h"

namespace wargameEngine
{
namespace ui
{
class CUIWindow : public UIElement
{
public:
	CUIWindow(int width, int height, std::wstring const& headerText, IUIElement* parent, view::ITextWriter& textWriter);

	void Draw(view::IRenderer& renderer) const override;
	bool LeftMouseButtonDown(int x, int y) override;
	bool LeftMouseButtonUp(int x, int y) override;
	void OnMouseMove(int x, int y) override;
	int GetHeight() const override;
	int GetX() const override;
	int GetY() const override;

private:
	std::wstring m_headerText;
	bool m_dragging;
	int m_prevX = 0;
	int m_prevY = 0;
};
}
}