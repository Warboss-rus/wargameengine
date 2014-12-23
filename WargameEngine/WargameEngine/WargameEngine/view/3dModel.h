#include <vector>
#include <memory>
#include <string>
#include <set>
#include <map>
#include "MaterialManager.h"
#include "Bounding.h"
#pragma once

class IObject;

struct sMesh
{
	std::string name;
	std::string materialName;
	size_t polygonIndex;
	bool operator== (sMesh const& other) { return name == other.name && materialName == other.materialName && polygonIndex == other.polygonIndex; }
	bool operator!= (sMesh const& other) { return !operator==(other); }
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
	unsigned int boneIndex;
	std::vector<unsigned int> children;
	float duration;
	enum eLoopMode {
		NONLOOPING,
		LOOPING,
		HOLDEND
	};
};

struct sModelCallListKey
{
	std::set<std::string> hiddenMeshes;
	bool vertexOnly;
	std::vector<sTeamColor> teamcolor;
	std::map<std::string, std::string> replaceTextures;
};

bool operator< (sModelCallListKey const& one, sModelCallListKey const& two);

class C3DModel
{
public:
	C3DModel(std::shared_ptr<IBounding> bounding, double scale);
	C3DModel(std::vector<CVector3f> & vertices, std::vector<CVector2f> & textureCoords, std::vector<CVector3f> & normals, std::vector<unsigned int> & indexes,
		CMaterialManager & materials, std::vector<sMesh> & meshes, std::shared_ptr<IBounding> bounding, double scale);
	~C3DModel();
	void SetModel(std::vector<CVector3f> & vertices, std::vector<CVector2f> & textureCoords, std::vector<CVector3f> & normals, std::vector<unsigned int> & indexes,
		CMaterialManager & materials, std::vector<sMesh> & meshes);
	void SetAnimation(std::vector<unsigned int> & weightCount, std::vector<unsigned int> & weightIndexes, std::vector<float> & weights, std::vector<sJoint> & skeleton, std::vector<sAnimation> & animations);
	void Draw(std::shared_ptr<IObject> object, bool vertexOnly, bool gpuSkinning);
	std::shared_ptr<IBounding> GetBounding() const { return m_bounding; }
	void SetBounding(std::shared_ptr<IBounding> bounding, double scale);
	void PreloadTextures() const;
	std::vector<std::string> GetAnimations() const;
private:
	void DrawModel(const std::set<std::string> * hideMeshes, bool vertexOnly, std::vector<CVector3f> const& vertices, std::vector<CVector3f> const& normals, bool useGPUrendering = false, const std::vector<sTeamColor> * teamcolor = nullptr, const std::map<std::string, std::string> * replaceTextures = nullptr);
	void CalculateGPUWeights();
	bool DrawSkinned(const std::set<std::string> * hideMeshes, bool vertexOnly, std::string const& animationToPlay, sAnimation::eLoopMode loop, float time, bool gpuSkinning, const std::vector<sTeamColor> * teamcolor = nullptr, const std::map<std::string, std::string> * replaceTextures = nullptr);
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
	std::shared_ptr<IBounding> m_bounding;
	std::map<sModelCallListKey, unsigned int> m_lists;
	double m_scale;
	unsigned int m_vbo;
	int m_count;
	std::vector<int> m_gpuWeightIndexes;
	std::vector<float> m_gpuWeight;
	std::vector<float> m_gpuInverseMatrices;
};

void MultiplyVectorToMatrix(CVector3f & vect, float * matrix);