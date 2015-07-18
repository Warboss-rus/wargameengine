#pragma once
#include "IRenderer.h"

class COpenGLRenderer : public IRenderer
{
public:
	virtual void SetTexture(std::string const& texture) override;

	virtual void RenderArrays(RenderMode mode, std::vector<CVector3f> const& vertices, std::vector<CVector3f> const& normals, std::vector<CVector2f> const& texCoords) override;
	virtual void RenderArrays(RenderMode mode, std::vector<CVector3d> const& vertices, std::vector<CVector3d> const& normals, std::vector<CVector2d> const& texCoords) override;
	virtual void RenderArrays(RenderMode mode, std::vector<CVector2f> const& vertices, std::vector<CVector2f> const& texCoords) override;
	virtual void RenderArrays(RenderMode mode, std::vector<CVector2i> const& vertices, std::vector<CVector2f> const& texCoords) override;

	virtual void PushMatrix() override;

	virtual void PopMatrix() override;

	virtual void Translate(float dx, float dy, float dz) override;
	virtual void Translate(double dx, double dy, double dz) override;
	virtual void Translate(int dx, int dy, int dz) override;

	virtual void SetColor(float r, float g, float b) override;
	virtual void SetColor(int r, int g, int b) override;
private:
	std::string m_lastTexture;
};