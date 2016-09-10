#pragma once
#include "IViewport.h"
#include "IViewHelper.h"
#include "ICamera.h"

class COffscreenViewport : public IViewport
{
public:
	COffscreenViewport(CachedTextureType type, int width, int height, float fieldOfView, IViewHelper & viewHelper, int textureIndex = -1);

	virtual ICamera& GetCamera() override;

	virtual ICamera const& GetCamera() const override;

	virtual void SetCamera(std::unique_ptr<ICamera> && camera) override;

	virtual void Draw(DrawFunc const& draw) override;

	virtual ICachedTexture const& GetTexture() const override;

	virtual Matrix4F GetProjectionMatrix() const override;
	virtual Matrix4F GetViewMatrix() const override;

	virtual void Resize(int width, int height) override;

	virtual void SetPolygonOffset(bool enable, float factor = 0.0f, float units = 0.0f) override;

	virtual void SetClippingPlanes(double near = 1.0, double far = 1000.0) override;

	virtual bool PointIsInViewport(int x, int y) const override;

	virtual int GetX() const override { return 0; }
	virtual int GetY() const override { return 0; }
	virtual int GetWidth() const override { return m_width; }
	virtual int GetHeight() const override { return m_height; }

private:
	bool m_depthOnly;
	int m_width;
	int m_height;
	float m_fieldOfView;
	IViewHelper & m_viewHelper;
	std::unique_ptr<ICamera> m_camera;
	std::unique_ptr<ICachedTexture> m_texture;
	std::unique_ptr<IFrameBuffer> m_FBO;
	Matrix4F m_projectionMatrix;
	Matrix4F m_viewMatrix;
	float m_polygonOffsetFactor = 0.0f;
	float m_polygonOffsetUnits = 0.0f;
	double m_nearPane = 1.0;
	double m_farPane = 1000.0;
};
