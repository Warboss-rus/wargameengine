#pragma once
#include "IRenderer.h"

class COpenGlCachedTexture : public ICachedTexture
{
public:
	COpenGlCachedTexture();
	~COpenGlCachedTexture();

	virtual void Bind() const override;

	operator unsigned int();
private:
	unsigned int m_id;
};

class COpenGLRenderer : public IRenderer
{
public:
	virtual void SetTexture(std::string const& texture, bool forceLoadNow = false) override;

	virtual void RenderArrays(RenderMode mode, std::vector<CVector3f> const& vertices, std::vector<CVector3f> const& normals, std::vector<CVector2f> const& texCoords) override;
	virtual void RenderArrays(RenderMode mode, std::vector<CVector3d> const& vertices, std::vector<CVector3d> const& normals, std::vector<CVector2d> const& texCoords) override;
	virtual void RenderArrays(RenderMode mode, std::vector<CVector2f> const& vertices, std::vector<CVector2f> const& texCoords) override;
	virtual void RenderArrays(RenderMode mode, std::vector<CVector2i> const& vertices, std::vector<CVector2f> const& texCoords) override;

	virtual void PushMatrix() override;

	virtual void PopMatrix() override;

	virtual void Translate(float dx, float dy, float dz) override;
	virtual void Translate(double dx, double dy, double dz) override;
	virtual void Translate(int dx, int dy, int dz) override;
	virtual void Rotate(double angle, double x, double y, double z) override;

	virtual void SetColor(float r, float g, float b) override;
	virtual void SetColor(int r, int g, int b) override;

	virtual std::unique_ptr<ICachedTexture> RenderToTexture(std::function<void() > const& func, unsigned int width, unsigned int height) override;

	virtual std::unique_ptr<ICachedTexture> CreateTexture(void * data, unsigned int width, unsigned int height, CachedTextureType type = CachedTextureType::RGBA) override;
};