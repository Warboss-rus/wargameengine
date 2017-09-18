#pragma once
#include "Camera.h"
#include "IViewHelper.h"
#include "IViewport.h"
#include "Matrix4.h"
#include <vector>
#include <unordered_map>

namespace wargameEngine
{
namespace model
{
class IBaseObject;
}

namespace view
{
class IInput;

class Viewport : public IViewport
{
public:
	Viewport(int x, int y, int width, int height, float fieldOfView, IViewHelper& renderer, IInput& input, bool onScreen, bool resize);
	Viewport(const Viewport& other) = delete;
	Viewport(Viewport&& other) = default;
	Viewport& operator=(const Viewport& other) = delete;
	Viewport& operator=(Viewport&& other) = default;
	~Viewport();

	//IViewport
	const ICachedTexture& GetTexture(size_t index) const override;
	const float* GetProjectionMatrix() const override;
	const float* GetViewMatrix() const override;
	int GetX() const override { return m_x; }
	int GetY() const override { return m_y; }
	int GetWidth() const override { return m_width; }
	int GetHeight() const override { return m_height; }

	Camera& GetCamera();
	Camera const& GetCamera() const;
	void AttachNewTexture(IRenderer::CachedTextureType type, int textureIndex = -1);
	void Bind();
	void Unbind();
	bool IsDepthOnly() const;
	bool DrawUI() const;
	IOcclusionQuery& GetOcclusionQuery(const model::IBaseObject* object);
	void SetPolygonOffset(bool enable, float factor = 0.0f, float units = 0.0f);
	void SetClippingPlanes(float near = 1.0, float far = 1000.0);
	bool PointIsInViewport(int x, int y) const;
	void Resize(int width, int height);
	void SetShadowViewport(IViewport* viewport) { m_shadowMapViewport = viewport; }
	void SetUpShadowMap() const;
	IViewport* GetShadowViewport() const { return m_shadowMapViewport; }
	bool NeedsFrustumCulling() const { return m_frustumCulling; }
	void EnableFrustumCulling(bool enable) { m_frustumCulling = enable; }
	IShaderProgram* GetShaderProgram() const { return m_shaderProgram.get(); }
	void SetShaders(const Path& vertexShader = Path(), const Path& fragmentShader = Path(), const Path& geometryShader = Path());

private:
	Camera m_camera;
	int m_x;
	int m_y;
	int m_width;
	int m_height;
	float m_fieldOfView;
	IViewHelper* m_renderer;
	Matrix4F m_projectionMatrix;
	Matrix4F m_viewMatrix;
	float m_polygonOffsetFactor = 0.0f;
	float m_polygonOffsetUnits = 0.0f;
	float m_nearPane = 1.0f;
	float m_farPane = 1000.0f;
	IViewport* m_shadowMapViewport = nullptr;
	std::unordered_map<const model::IBaseObject*, std::unique_ptr<IOcclusionQuery>> m_occlusionQueries;
	std::unique_ptr<IFrameBuffer> m_FBO;
	std::unique_ptr<IShaderProgram> m_shaderProgram;
	std::vector<std::unique_ptr<ICachedTexture>> m_textures;
	bool m_depthOnly;
	bool m_resize;
	bool m_frustumCulling = true;
};
}
}