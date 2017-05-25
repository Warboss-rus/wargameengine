#pragma once
#include "../Typedefs.h"
#include "../array_view.h"
#include "../model/TeamColor.h"
#include "Vector3.h"
#include <functional>
#include <memory>
#include <string>
#include "IShaderManager.h"

namespace wargameEngine
{
namespace view
{

class ICachedTexture
{
public:
	virtual ~ICachedTexture() {}
};

class IVertexBuffer
{
public:
	virtual ~IVertexBuffer() {}
};

class IRenderer
{
public:
	enum class RenderMode
	{
		Triangles,
		TriangleStrip,
		Lines,
		LineLoop
	};

	enum class CachedTextureType
	{
		RGBA,
		Alpha,
		Depth,
		RenderTarget
	};

	enum class TextureSlot
	{
		Diffuse = 0,
		ShadowMap = 1,
		Specular = 2,
		Bump = 3,
	};

	enum class Feature
	{
		Instancing,
	};

	virtual void RenderArrays(RenderMode mode, array_view<CVector3f> const& vertices, array_view<CVector3f> const& normals, array_view<CVector2f> const& texCoords) = 0;
	virtual void RenderArrays(RenderMode mode, array_view<CVector2i> const& vertices, array_view<CVector2f> const& texCoords) = 0;
	virtual void Draw(IVertexBuffer& buffer, size_t count, size_t begin = 0, size_t instances = 0) = 0;
	virtual void DrawIndexed(IVertexBuffer& buffer, size_t count, size_t begin = 0, size_t instances = 0) = 0;
	virtual void SetIndexBuffer(IVertexBuffer& buffer, const unsigned int* indexPtr, size_t indexesSize) = 0;
	virtual void AddVertexAttribute(IVertexBuffer& buffer, const std::string& attribute, int elementSize, size_t count, IShaderManager::Format type, const void* values, bool perInstance = false) = 0;

	virtual void PushMatrix() = 0;
	virtual void PopMatrix() = 0;
	virtual void Translate(const CVector3f& delta) = 0;
	virtual void Translate(int dx, int dy, int dz = 0) = 0;
	virtual void Rotate(float angle, const CVector3f& axis) = 0;
	virtual void Rotate(const CVector3f& rotations) = 0;
	virtual void Scale(float scale) = 0;
	virtual const float* GetViewMatrix() const = 0;
	virtual const float* GetModelMatrix() const = 0;
	virtual void SetModelMatrix(const float* matrix) = 0;
	virtual void LookAt(const CVector3f& position, const CVector3f& direction, const CVector3f& up) = 0;

	virtual void SetTexture(const Path& texture, bool forceLoadNow = false, int flags = 0) = 0;
	virtual void SetTexture(const ICachedTexture& texture, TextureSlot slot = TextureSlot::Diffuse) = 0;
	virtual void UnbindTexture(TextureSlot slot = TextureSlot::Diffuse) = 0;
	virtual void RenderToTexture(const std::function<void()>& func, ICachedTexture& texture, unsigned int width, unsigned int height) = 0;
	virtual std::unique_ptr<ICachedTexture> CreateTexture(const void* data, unsigned int width, unsigned int height, CachedTextureType type = CachedTextureType::RGBA) = 0;

	virtual void SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 0xff) = 0;
	virtual void SetColor(const float* color) = 0;
	virtual void SetMaterial(const float* ambient, const float* diffuse, const float* specular, float shininess) = 0;

	virtual std::unique_ptr<IVertexBuffer> CreateVertexBuffer(const float* vertex = nullptr, const float* normals = nullptr, const float* texcoords = nullptr, size_t size = 0, bool temp = false) = 0;

	virtual std::string GetName() const = 0;
	virtual bool SupportsFeature(Feature feature) const = 0;

	virtual IShaderManager& GetShaderManager() = 0;

	virtual ~IRenderer() {}
};

}
}