#pragma once
#include "IRenderer.h"
#include "ITextureHelper.h"

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

class IViewHelper : public ITextureHelper
{
public:
	virtual void WindowCoordsToWorldVector(int x, int y, CVector3d & start, CVector3d & end) const = 0;
	virtual void WorldCoordsToWindowCoords(CVector3d const& worldCoords, int& x, int& y) const = 0;
	virtual std::unique_ptr<IFrameBuffer> CreateFramebuffer() const = 0;
	virtual void EnableLight(size_t index, bool enable) = 0;
	virtual void SetLightColor(size_t index, LightningType type, float * values) = 0;
	virtual void SetLightPosition(size_t index, float* pos) = 0;
	virtual float GetMaximumAnisotropyLevel() const = 0;
	virtual void EnableVertexLightning(bool enable) = 0;
	virtual void GetProjectionMatrix(float * matrix) const = 0;
	virtual void EnableDepthTest(bool enable) = 0;
	virtual void EnableBlending(bool enable) = 0;
	virtual void SetUpViewport(CVector3d const& position, CVector3d const& target, unsigned int viewportWidth, unsigned int viewportHeight, double viewingAngle, double nearPane = 1.0, double farPane = 1000.0) = 0;
	virtual void RestoreViewport() = 0;
	virtual void EnablePolygonOffset(bool enable, float factor = 0.0f, float units = 0.0f) = 0;
	virtual void ClearBuffers(bool color = true, bool depth = true) = 0;

	virtual ~IViewHelper() {}
};