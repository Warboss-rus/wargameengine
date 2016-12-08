#pragma once
#include <memory>
#include <functional>
#include "Matrix4.h"

class ICamera;
class ICachedTexture;
class IOcclusionQuery;
class IBaseObject;

class IViewport
{
public:
	virtual ~IViewport() {}

	virtual ICamera& GetCamera() = 0;
	virtual ICamera const& GetCamera() const = 0;
	virtual void SetCamera(std::unique_ptr<ICamera> && camera) = 0;

	typedef std::function<void(bool depthOnly, bool drawUI)> DrawFunc;
	virtual void Draw(DrawFunc const& draw) = 0;

	virtual ICachedTexture const& GetTexture() const= 0;

	virtual IOcclusionQuery & GetOcclusionQuery(const IBaseObject* object) = 0;

	virtual Matrix4F GetProjectionMatrix() const = 0;
	virtual Matrix4F GetViewMatrix() const = 0;

	virtual void Resize(int width, int height) = 0;

	virtual void SetPolygonOffset(bool enable, float factor = 0.0f, float units = 0.0f) = 0;

	virtual void SetClippingPlanes(double near = 1.0, double far = 1000.0) = 0;

	virtual bool PointIsInViewport(int x, int y) const = 0;

	virtual int GetX() const = 0;
	virtual int GetY() const = 0;
	virtual int GetWidth() const = 0;
	virtual int GetHeight() const = 0;

	virtual void SetShadowViewport(IViewport* viewport) = 0;
	virtual void SetUpShadowMap() const = 0;
	virtual IViewport* GetShadowViewport() const = 0;
};
