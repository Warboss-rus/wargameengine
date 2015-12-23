#pragma once
#include "UIElement.h"

class CUIWindow : public CUIElement
{
public:
	CUIWindow(int width, int height, std::wstring const& headerText, IUIElement * parent, IRenderer & renderer, ITextWriter & textWriter);

	virtual void Draw() const override;
	virtual bool LeftMouseButtonDown(int x, int y) override;
	virtual bool LeftMouseButtonUp(int x, int y) override;
	virtual void OnMouseMove(int x, int y) override;
	virtual int GetHeight() const override;
	virtual int GetX() const override;
	virtual int GetY() const override;
private:
	std::wstring m_headerText;
	bool m_dragging;
	int m_prevX;
	int m_prevY;
};