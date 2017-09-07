#pragma once
#include "UIElement.h"

namespace wargameEngine
{
namespace ui
{
class UICachedElement : public UIElement
{
public:
	using UIElement::UIElement;
	void Draw(IUIRenderer& renderer) const final;
protected:
	virtual void DoPaint(IUIRenderer& renderer) const = 0;
};
}
}