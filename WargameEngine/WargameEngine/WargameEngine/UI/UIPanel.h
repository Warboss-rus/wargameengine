#include "UIElement.h"

namespace wargameEngine
{
namespace ui
{
class UIPanel : public UIElement
{
public:
	UIPanel(int x, int y, int height, int width, IUIElement* parent, view::ITextWriter& textWriter);
	void Draw(view::IRenderer& renderer) const override;
};
}
}