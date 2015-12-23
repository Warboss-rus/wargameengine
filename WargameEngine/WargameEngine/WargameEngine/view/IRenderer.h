#pragma once
#include "Vector3.h"
#include "ITextureHelper.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "IShaderManager.h"
#include "../model/TeamColor.h"

enum class RenderMode
{
	TRIANGLES,
	TRIANGLE_STRIP,
	RECTANGLES,
	LINES,
	LINE_LOOP
};

enum class CachedTextureType
{
	RGBA,
	ALPHA,
	DEPTH
};

class IDrawingList
{
public:
	virtual void Draw() const = 0;

	virtual ~IDrawingList() {}
};

class IVertexBuffer
{
public:
	virtual void Bind() const = 0;
	virtual void DrawIndexes(unsigned int * indexPtr, size_t count) = 0;
	virtual void DrawAll(size_t count) = 0;
	virtual void UnBind() const = 0;

	virtual ~IVertexBuffer() {}
};

class IRenderer
{
public:
	virtual void RenderArrays(RenderMode mode, std::vector<CVector3f> const& vertices, std::vector<CVector3f> const& normals, std::vector<CVector2f> const& texCoords) = 0;
	virtual void RenderArrays(RenderMode mode, std::vector<CVector3d> const& vertices, std::vector<CVector3d> const& normals, std::vector<CVector2d> const& texCoords) = 0;
	virtual void RenderArrays(RenderMode mode, std::vector<CVector2f> const& vertices, std::vector<CVector2f> const& texCoords) = 0;
	virtual void RenderArrays(RenderMode mode, std::vector<CVector2i> const& vertices, std::vector<CVector2f> const& texCoords) = 0;
	virtual void SetColor(const float r, const float g, const float b) = 0;
	virtual void SetColor(const int r, const int g, const int b) = 0;
	virtual void SetColor(const float * color) = 0;
	virtual void SetColor(const int * color) = 0;

	virtual void PushMatrix() = 0;
	virtual void PopMatrix() = 0;
	virtual void Translate(const float dx, const float dy, const float dz) = 0;
	virtual void Translate(const double dx, const double dy, const double dz) = 0;
	virtual void Translate(const int dx, const int dy, const int dz) = 0;
	virtual void Rotate(const double angle, const double x, const double y, const double z) = 0;
	virtual void Scale(const double scale) = 0;
	virtual void GetViewMatrix(float * matrix) const = 0;
	virtual void ResetViewMatrix() = 0;
	virtual void LookAt(CVector3d const& position, CVector3d const& direction, CVector3d const& up) = 0;

	virtual void SetTexture(std::string const& texture, bool forceLoadNow = false, int flags = 0) = 0;
	virtual void SetTexture(std::string const& texture, TextureSlot slot, int flags = 0) = 0;
	virtual void SetTexture(std::string const& texture, const std::vector<sTeamColor> * teamcolor, int flags = 0) = 0;
	virtual std::unique_ptr<ICachedTexture> RenderToTexture(std::function<void()> const& func, unsigned int width, unsigned int height) = 0;
	virtual std::unique_ptr<ICachedTexture> CreateTexture(const void * data, unsigned int width, unsigned int height, CachedTextureType type = CachedTextureType::RGBA) = 0;

	virtual void SetMaterial(const float * ambient, const float * diffuse, const float * specular, const float shininess) = 0;

	virtual std::unique_ptr<IDrawingList> CreateDrawingList(std::function<void()> const& func) = 0;

	virtual std::unique_ptr<IVertexBuffer> CreateVertexBuffer(const float * vertex = nullptr, const float * normals = nullptr, const float * texcoords = nullptr, size_t size = 0) = 0;

	virtual std::unique_ptr<IShaderManager> CreateShaderManager() const = 0;

	virtual std::string GetName() const = 0;

	virtual ~IRenderer() {}
};