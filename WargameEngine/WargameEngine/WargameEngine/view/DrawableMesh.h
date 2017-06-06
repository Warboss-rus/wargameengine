#pragma once
#include <memory>
#include <vector>
#include "Matrix4.h"
#include "Vector3.h"

namespace wargameEngine
{
namespace view
{
class IVertexBuffer;
class IShaderProgram;
class ICachedTexture;
struct Material;

struct TempMeshBuffer
{
	std::vector<CVector3f> vertices;
	std::vector<CVector3f> normals;
	const CVector2f* texCoords;
	const unsigned* indexes;
	const size_t indexesCount;
};

struct DrawableMesh
{
	IShaderProgram* shader = nullptr;
	ICachedTexture* texturePtr = nullptr;
	Material* material = nullptr;
	IVertexBuffer * buffer = nullptr;
	Matrix4F modelMatrix;
	std::shared_ptr<TempMeshBuffer> tempBuffer;
	std::shared_ptr<std::vector<float>> skeleton;
	size_t start = 0;
	size_t count = 0;
	bool indexed = false;

	DrawableMesh(const DrawableMesh& other) = delete;
	DrawableMesh(DrawableMesh&& other) = default;
	DrawableMesh& operator=(const DrawableMesh& other) = delete;
	DrawableMesh& operator=(DrawableMesh&& other) = default;
};

using MeshList = std::vector<DrawableMesh>;
}
}