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

class CTextureManager;
class IViewport;

class IViewHelper : public ITextureHelper, public IRenderer
{
public:
	virtual void WindowCoordsToWorldVector(IViewport & viewport, int x, int y, CVector3f & start, CVector3f & end) const = 0;
	virtual void WorldCoordsToWindowCoords(IViewport & viewport, CVector3f const& worldCoords, int& x, int& y) const = 0;
	virtual std::unique_ptr<IFrameBuffer> CreateFramebuffer() const = 0;
	virtual void EnableLight(size_t index, bool enable) = 0;
	virtual void SetLightColor(size_t index, LightningType type, float * values) = 0;
	virtual void SetLightPosition(size_t index, float* pos) = 0;
	virtual float GetMaximumAnisotropyLevel() const = 0;
	virtual void EnableVertexLightning(bool enable) = 0;
	virtual void GetProjectionMatrix(float * matrix) const = 0;
	virtual void EnableDepthTest(bool enable) = 0;
	virtual void EnableBlending(bool enable) = 0;
	virtual void SetUpViewport(unsigned int viewportX, unsigned int viewportY, unsigned int viewportWidth, unsigned int viewportHeight, double viewingAngle, double nearPane = 1.0, double farPane = 1000.0) = 0;
	virtual void DrawIn2D(std::function<void()> const& drawHandler) = 0;
	virtual void EnablePolygonOffset(bool enable, float factor = 0.0f, float units = 0.0f) = 0;
	virtual void ClearBuffers(bool color = true, bool depth = true) = 0;
	virtual void SetTextureManager(CTextureManager & textureManager) = 0;

	virtual ~IViewHelper() {}
};