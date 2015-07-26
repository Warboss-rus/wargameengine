#pragma once
#include "Vector3.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>

enum class RenderMode
{
	TRIANGLES,
	TRIANGLE_STRIP,
	RECTANGLES,
	LINES
};

enum class CachedTextureType
{
	RGBA,
	ALPHA
};

class ICachedTexture
{
public:
	virtual void Bind() const = 0;

	virtual ~ICachedTexture() {}
};

class IRenderer
{
public:
	virtual void SetTexture(std::string const& texture) = 0;
	virtual void RenderArrays(RenderMode mode, std::vector<CVector3f> const& vertices, std::vector<CVector3f> const& normals, std::vector<CVector2f> const& texCoords) = 0;
	virtual void RenderArrays(RenderMode mode, std::vector<CVector3d> const& vertices, std::vector<CVector3d> const& normals, std::vector<CVector2d> const& texCoords) = 0;
	virtual void RenderArrays(RenderMode mode, std::vector<CVector2f> const& vertices, std::vector<CVector2f> const& texCoords) = 0;
	virtual void RenderArrays(RenderMode mode, std::vector<CVector2i> const& vertices, std::vector<CVector2f> const& texCoords) = 0;
	virtual void SetColor(float r, float g, float b) = 0;
	virtual void SetColor(int r, int g, int b) = 0;
	virtual void PushMatrix() = 0;
	virtual void PopMatrix() = 0;
	virtual void Translate(float dx, float dy, float dz) = 0;
	virtual void Translate(double dx, double dy, double dz) = 0;
	virtual void Translate(int dx, int dy, int dz) = 0;

	virtual std::unique_ptr<ICachedTexture> RenderToTexture(std::function<void()> const& func, unsigned int width, unsigned int height) = 0;
	virtual std::unique_ptr<ICachedTexture> CreateTexture(void * data, unsigned int width, unsigned int height, CachedTextureType type = CachedTextureType::RGBA) = 0;

	virtual ~IRenderer() {}
};