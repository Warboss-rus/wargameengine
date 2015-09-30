#pragma once
#include "IRenderer.h"
#include "TextureManager.h"
#include "IViewHelper.h"

class COpenGLRenderer : public IRenderer, public IViewHelper
{
public:
	COpenGLRenderer() {}

	virtual void RenderArrays(RenderMode mode, std::vector<CVector3f> const& vertices, std::vector<CVector3f> const& normals, std::vector<CVector2f> const& texCoords) override;
	virtual void RenderArrays(RenderMode mode, std::vector<CVector3d> const& vertices, std::vector<CVector3d> const& normals, std::vector<CVector2d> const& texCoords) override;
	virtual void RenderArrays(RenderMode mode, std::vector<CVector2f> const& vertices, std::vector<CVector2f> const& texCoords) override;
	virtual void RenderArrays(RenderMode mode, std::vector<CVector2i> const& vertices, std::vector<CVector2f> const& texCoords) override;

	virtual void PushMatrix() override;
	virtual void PopMatrix() override;
	virtual void Translate(const float dx, const float dy, const float dz) override;
	virtual void Translate(const double dx, const double dy, const double dz) override;
	virtual void Translate(const int dx, const int dy, const int dz) override;
	virtual void Rotate(const double angle, const double x, const double y, const double z) override;
	virtual void SetColor(const float r, const float g, const float b) override;
	virtual void SetColor(const int r, const int g, const int b) override;
	virtual void SetColor(const float * color) override;
	virtual void SetColor(const int * color) override;
	virtual void Scale(const double scale) override;
	virtual void GetViewMatrix(float * matrix) const override;
	virtual void ResetViewMatrix() override;
	virtual void LookAt(CVector3d const& position, CVector3d const& direction, CVector3d const& up) override;

	virtual void SetTexture(std::string const& texture, bool forceLoadNow = false, int flags = 0) override;

	virtual void SetTexture(std::string const& texture, TextureSlot slot, int flags = 0) override;

	virtual void SetTexture(std::string const& texture, const std::vector<sTeamColor> * teamcolor, int flags = 0) override;

	virtual std::unique_ptr<ICachedTexture> RenderToTexture(std::function<void() > const& func, unsigned int width, unsigned int height) override;
	virtual std::unique_ptr<ICachedTexture> CreateTexture(const void * data, unsigned int width, unsigned int height, CachedTextureType type = CachedTextureType::RGBA) override;

	virtual void SetMaterial(const float * ambient, const float * diffuse, const float * specular, const float shininess) override;

	virtual std::unique_ptr<IDrawingList> CreateDrawingList(std::function<void() > const& func) override;

	virtual std::unique_ptr<IVertexBuffer> CreateVertexBuffer(const float * vertex = nullptr, const float * normals = nullptr, const float * texcoords = nullptr) override;

	virtual std::unique_ptr<IFrameBuffer> CreateFramebuffer() const override;

	CTextureManager & GetTextureManager();
	CTextureManager const& GetTextureManager() const;

	virtual void WindowCoordsToWorldVector(int x, int y, CVector3d & start, CVector3d & end) const override;
	virtual void WorldCoordsToWindowCoords(CVector3d const& worldCoords, int& x, int& y) const override;
	virtual void EnableLight(size_t index, bool enable) override;
	virtual void SetLightColor(size_t index, LightningType type, float * values) override;
	virtual void SetLightPosition(size_t index, float* pos) override;
	virtual float GetMaximumAnisotropyLevel() const override;
private:
	CTextureManager m_textureManager;
};