#pragma once
#include "IViewport.h"
#include <map>
#include <vector>
#include "IViewHelper.h"

class IViewHelper;
class IOcclusionQuery;
class IObject;

class CViewportBase : public IViewport
{
public:
	CViewportBase(int x, int y, int width, int height, float fieldOfView, IViewHelper & renderer, bool onScreen, bool resize);
	~CViewportBase();

	//IViewport
	const ICachedTexture & GetTexture(size_t index) const override;
	const float* GetProjectionMatrix() const override;
	const float* GetViewMatrix() const override;
	int GetX() const override { return m_x; }
	int GetY() const override { return m_y; }
	int GetWidth() const override { return m_width; }
	int GetHeight() const override { return m_height; }

	ICamera& GetCamera();
	ICamera const& GetCamera() const;
	void SetCamera(std::unique_ptr<ICamera> && camera);
	void AttachNewTexture(CachedTextureType type, int textureIndex = -1);
	void Bind();
	void Unbind();
	bool IsDepthOnly() const;
	bool DrawUI() const;
	IOcclusionQuery & GetOcclusionQuery(const IBaseObject* object);
	void SetPolygonOffset(bool enable, float factor = 0.0f, float units = 0.0f);
	void SetClippingPlanes(float near = 1.0, float far = 1000.0);
	bool PointIsInViewport(int x, int y) const;
	void Resize(int width, int height);
	void SetShadowViewport(IViewport* viewport) { m_shadowMapViewport = viewport; }
	void SetUpShadowMap() const;
	IViewport* GetShadowViewport() const { return m_shadowMapViewport; }

private:
	std::unique_ptr<ICamera> m_camera;
	int m_x;
	int m_y;
	int m_width;
	int m_height;
	float m_fieldOfView;
	IViewHelper & m_renderer;
	float m_projectionMatrix[16];
	float m_viewMatrix[16];
	float m_polygonOffsetFactor = 0.0f;
	float m_polygonOffsetUnits = 0.0f;
	float m_nearPane = 1.0f;
	float m_farPane = 1000.0f;
	IViewport* m_shadowMapViewport = nullptr;
	std::map<const IBaseObject*, std::unique_ptr<IOcclusionQuery>> m_occlusionQueries;
	std::unique_ptr<IFrameBuffer> m_FBO;
	std::vector<std::unique_ptr<ICachedTexture>> m_textures;
	bool m_depthOnly;
	bool m_resize;
};