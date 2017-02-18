#pragma once
#include <vector>
#include <memory>
#include <string>
#include <set>
#include <map>
#include "MaterialManager.h"
#include "Vector3.h"
#include "../model/Animation.h"
#include "../model/TeamColor.h"

class IObject;
class IShaderManager;
class IRenderer;
class IVertexBuffer;
class IVertexAttribCache;

struct sMesh
{
	std::string name;
	std::string materialName;
	size_t begin;
	size_t end;
	sMaterial * material;
};

struct sJoint
{
	std::string bone;//needed for collada loader
	int parentIndex;
	float matrix[16];
	float invBindMatrix[16];
	std::string id;//needed for collada loader
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
	C3DModel(double scale, double rotateX, double rotateY, double rotateZ);
	~C3DModel();
	C3DModel(C3DModel const& other);
	C3DModel& operator=(C3DModel const& other) = default;
	void SetModel(std::vector<CVector3f> & vertices, std::vector<CVector2f> & textureCoords, std::vector<CVector3f> & normals, std::vector<unsigned int> & indexes,
		CMaterialManager & materials, std::vector<sMesh> & meshes);
	void SetAnimation(std::vector<unsigned int> & weightCount, std::vector<unsigned int> & weightIndexes, std::vector<float> & weights, std::vector<sJoint> & skeleton, std::vector<sAnimation> & animations);
	void Draw(IRenderer & renderer, IObject* object, bool vertexOnly, bool gpuSkinning);
	void PreloadTextures(IRenderer & renderer) const;
	std::vector<std::string> GetAnimations() const;
private:
	void DrawModel(IRenderer & renderer, const std::set<std::string> * hideMeshes, bool vertexOnly, IVertexBuffer & vertexBuffer, bool useGPUrendering = false, const std::vector<sTeamColor> * teamcolor = nullptr, const std::map<std::wstring, std::wstring> * replaceTextures = nullptr);
	void CalculateGPUWeights(IRenderer & renderer);
	bool DrawSkinned(IRenderer & renderer, const std::set<std::string> * hideMeshes, bool vertexOnly, std::string const& animationToPlay, eAnimationLoopMode loop, float time, bool gpuSkinning, const std::vector<sTeamColor> * teamcolor = nullptr, const std::map<std::wstring, std::wstring> * replaceTextures = nullptr);
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
	CMaterialManager m_materials;
	double m_scale;
	CVector3d m_rotation;
	size_t m_count;
	std::unique_ptr<IVertexBuffer> m_vertexBuffer;
	std::unique_ptr<IVertexAttribCache> m_weightsCache;
	std::unique_ptr<IVertexAttribCache> m_weightIndiciesCache;
};

void MultiplyVectorToMatrix(CVector3f & vect, float * matrix);