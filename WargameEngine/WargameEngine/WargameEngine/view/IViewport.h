#pragma once
#include <memory>
#include <functional>
#include "Matrix4.h"

class ICamera;
class ICachedTexture;

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

	virtual Matrix4F GetProjectionMatrix() const = 0;
	virtual Matrix4F GetViewMatrix() const = 0;

	virtual void Resize(int width, int height) = 0;

	virtual void SetPolygonOffset(bool enable, float factor = 0.0f, float units = 0.0f) = 0;

	virtual void SetClippingPlanes(double near = 1.0, double far = 1000.0) = 0;
};
