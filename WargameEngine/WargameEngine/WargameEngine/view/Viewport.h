#pragma once
#include "ViewportBase.h"

class IViewHelper;

class CViewport : public CViewportBase
{
public:
	CViewport(int x, int y, int width, int height, float fieldOfView, IViewHelper & renderer, bool resize = true);

	virtual void Draw(DrawFunc const& draw) override;

	virtual void Resize(int width, int height) override;
private:
	bool m_resize;
};