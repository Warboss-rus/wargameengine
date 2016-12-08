#pragma once
#include "ViewportBase.h"
#include "IViewHelper.h"
#include "ICamera.h"

class COffscreenViewport : public CViewportBase
{
public:
	COffscreenViewport(CachedTextureType type, int width, int height, float fieldOfView, IViewHelper & viewHelper, int textureIndex = -1);

	virtual void Draw(DrawFunc const& draw) override;

	virtual ICachedTexture const& GetTexture() const override;

	virtual bool PointIsInViewport(int x, int y) const override;

private:
	bool m_depthOnly;
	std::unique_ptr<ICachedTexture> m_texture;
	std::unique_ptr<IFrameBuffer> m_FBO;
};
