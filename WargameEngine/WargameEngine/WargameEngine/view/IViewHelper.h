#pragma once
#include "IRenderer.h"

class IFrameBuffer
{
public:
	virtual ~IFrameBuffer() {}
	virtual void Bind() const = 0;
	virtual void UnBind() const = 0;
	virtual void AssignTexture(ICachedTexture & texture, CachedTextureType type) = 0;
};

enum class LightningType
{
	DIFFUSE,
	AMBIENT,
	SPECULAR
};

class IViewHelper
{
public:
	virtual void WindowCoordsToWorldVector(int x, int y, CVector3d & start, CVector3d & end) const = 0;
	virtual void WorldCoordsToWindowCoords(CVector3d const& worldCoords, int& x, int& y) const = 0;
	virtual std::unique_ptr<IFrameBuffer> CreateFramebuffer() const = 0;
	virtual void EnableLight(size_t index, bool enable) = 0;
	virtual void SetLightColor(size_t index, LightningType type, float * values) = 0;

	virtual ~IViewHelper() {}
};