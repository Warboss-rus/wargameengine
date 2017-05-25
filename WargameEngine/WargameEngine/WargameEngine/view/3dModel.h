#pragma once
#include "../Typedefs.h"
#include "../model/Animation.h"
#include "../model/TeamColor.h"
#include "DrawableMesh.h"
#include "MaterialManager.h"
#include "Vector3.h"
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace wargameEngine
{
namespace model
{
class IObject;
}

namespace view
{
class IShaderManager;
class IRenderer;
class IVertexBuffer;
class IVertexAttribCache;
class TextureManager;
class ICachedTexture;

struct sMesh
{
	std::string name;
	std::string materialName;
	size_t begin;
	size_t end;
	Material* material;
};

struct sJoint
{
	std::string bone; //needed for collada loader
	int parentIndex;
	float matrix[16];
	float invBindMatrix[16];
	std::string id; //needed for collada loader
};

struct sAnimation
{
	std::string id;
	std::vector<float> keyframes;
	std::vector<float> matrices;
	size_t boneIndex;
	std::vector<size_t> children;
	float duration;
};

class C3DModel
{
public:
	C3DModel(float scale, const CVector3f& rotations);
	~C3DModel();
	C3DModel(C3DModel const& other) = delete;
	C3DModel(C3DModel && other) = default;
	C3DModel& operator=(C3DModel const& other) = delete;
	C3DModel& operator=(C3DModel&& other) = default;

	void SetModel(std::vector<CVector3f>& vertices, std::vector<CVector2f>& textureCoords, std::vector<CVector3f>& normals, std::vector<unsigned int>& indexes,
		MaterialManager& materials, std::vector<sMesh>& meshes);
	void SetAnimation(std::vector<unsigned int>& weightCount, std::vector<unsigned int>& weightIndexes, std::vector<float>& weights, std::vector<sJoint>& skeleton, std::vector<sAnimation>& animations);
	void PreloadTextures(TextureManager& textureManager) const;
	std::vector<std::string> GetAnimations() const;
	void GetMeshes(IRenderer& renderer, TextureManager& textureManager, model::IObject* object, bool gpuSkinning, std::vector<DrawableMesh>& meshesVec);

	float GetScale() const;
	CVector3f GetRotation() const;

private:
	void GetModelMeshes(IRenderer& renderer, TextureManager& textureManager, std::vector<DrawableMesh>& meshesVec, const std::set<std::string>* hideMeshes,
		IVertexBuffer* vertexBuffer, const std::vector<model::TeamColor>* teamcolor, const std::map<Path, Path>* replaceTextures,
		const std::shared_ptr<std::vector<float>>& skeleton, const std::shared_ptr<TempMeshBuffer>& tempBuffer) const;
	ICachedTexture* GetTexturePtr(Material* material, const std::map<Path, Path>* replaceTextures, TextureManager& textureManager, const std::vector<model::TeamColor>* teamcolor) const;
	void CalculateGPUWeights(IRenderer& renderer);
	void GetMeshesSkinned(IRenderer& renderer, TextureManager& textureManager, std::vector<DrawableMesh>& meshesVec, const std::set<std::string>* hideMeshes,
		std::string const& animationToPlay, model::AnimationLoop loop, float time, bool gpuSkinning, const std::vector<model::TeamColor>* teamcolor = nullptr,
		const std::map<Path, Path>* replaceTextures = nullptr);

	std::vector<CVector3f> m_vertices;
	std::vector<CVector2f> m_textureCoords;
	std::vector<CVector3f> m_normals;
	std::vector<unsigned int> m_indexes;
	std::vector<unsigned int> m_weightsCount;
	std::vector<unsigned int> m_weightsIndexes;
	std::vector<float> m_weights;
	std::vector<sJoint> m_skeleton;
	std::vector<sAnimation> m_animations;
	std::vector<sMesh> m_meshes;
	MaterialManager m_materials;
	float m_scale;
	CVector3f m_rotation;
	size_t m_count;
	std::unique_ptr<IVertexBuffer> m_vertexBuffer;
};

void MultiplyVectorToMatrix(CVector3f& vect, float* matrix);
}
}