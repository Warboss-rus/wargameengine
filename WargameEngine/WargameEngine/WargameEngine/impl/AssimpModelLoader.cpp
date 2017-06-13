#include "AssimpModelLoader.h"
#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp\scene.h>
#include "..\view\3dModel.h"
#include "..\Utils.h"
#include "..\array_view.h"
#include "..\LogWriter.h"

using namespace Assimp;
using namespace wargameEngine;
using namespace view;

struct CAssimpModelLoader::Impl
{
public:
	std::unique_ptr<C3DModel> LoadModel(unsigned char * data, size_t size, const C3DModel & dummyModel)
	{
		Importer importer;
		const aiScene* scene = importer.ReadFileFromMemory(data, size,
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType | 
			aiProcess_LimitBoneWeights |
			aiProcess_GenUVCoords | 
			aiProcess_TransformUVCoords |
			aiProcess_PreTransformVertices | //TODO: remove, use mesh matrices instead
			aiProcess_OptimizeMeshes);
		if (!scene) throw std::runtime_error("Cannot process model");

		std::unique_ptr<C3DModel> result = std::make_unique<C3DModel>(dummyModel.GetScale(), dummyModel.GetRotation());
		MaterialManager materialManager;
		std::vector<CVector3f> vertices;
		std::vector<CVector3f> normals;
		std::vector<CVector2f> texCoords;
		std::vector<math::vec4> colors;
		std::vector<unsigned int> indices;
		std::vector<sMesh> meshes;
		std::vector<std::string> materialNames;
		std::vector<unsigned int> weightsCount;
		std::vector<unsigned int> weightsIndices;
		std::vector<float> weights;
		std::vector<sJoint> skeleton;
		std::vector<sAnimation> animations;
		bool hasNormals = false;
		bool hasTexCoords = false;
		bool hasWeights = false;
		bool hasColors = false;

		if (scene->HasMaterials())
		{
			array_view<aiMaterial*> materials(scene->mMaterials, scene->mNumMaterials);
			for (aiMaterial* material : materials)
			{
				auto processed = ProcessMaterial(material);
				materialManager.AddMaterial(processed.first, processed.second);
				materialNames.push_back(processed.first);
			}
		}

		if (scene->HasMeshes())
		{
			array_view<aiMesh*> meshesView(scene->mMeshes, scene->mNumMeshes);
			for (aiMesh* mesh : meshesView)
			{
				size_t verticesOffset = vertices.size();
				size_t verticesCount = mesh->mNumVertices / 3;
				vertices.resize(verticesOffset + verticesCount);
				normals.resize(verticesOffset + verticesCount);
				texCoords.resize(verticesOffset + verticesCount);
				weightsCount.resize(verticesOffset + verticesCount);
				memcpy(vertices.data() + verticesOffset, mesh->mVertices, sizeof(float) * verticesCount * 3);
				if (mesh->HasNormals())
				{
					memcpy(normals.data() + verticesOffset, mesh->mNormals, sizeof(float) * verticesCount * 3);
					hasNormals = true;
				}
				if (mesh->HasTextureCoords(0))
				{
					for (size_t j = 0; j < verticesCount; ++j)
					{
						auto& tc = mesh->mTextureCoords[0][j];
						texCoords[j + verticesOffset] = CVector2f(tc.x, tc.y);
					}
					hasTexCoords = true;
				}
				if (mesh->HasVertexColors(0))
				{
					colors.resize(verticesOffset + verticesCount);
					memcpy(colors.data() + verticesOffset, mesh->mColors[0], sizeof(aiColor4D));
					hasColors = true;
				}
				size_t indicesOffset = indices.size();
				size_t indicesCount = 0;
				if (mesh->HasFaces())
				{
					array_view<aiFace> faces(mesh->mFaces, mesh->mNumFaces);
					indices.reserve(indicesOffset + mesh->mNumFaces * 3);
					for (const aiFace& face : faces)
					{
						if (face.mNumIndices == 3)//is triangle
						{
							for (size_t k = 0; k < face.mNumIndices; ++k)
							{
								indices.push_back(face.mIndices[k]);
							}
							indicesCount += face.mNumIndices;
						}
					}
				}
				if (mesh->HasBones())
				{
					array_view<aiBone*> bones(mesh->mBones, mesh->mNumBones);
					std::vector<std::vector<std::pair<unsigned, float>>> meshWeights(verticesCount);
					for (aiBone* bone : bones)
					{
						array_view<aiVertexWeight> weightsView(bone->mWeights, bone->mNumWeights);
						const unsigned newJointIndex = static_cast<unsigned>(skeleton.size());
						for (const aiVertexWeight& weight : weightsView)
						{
							meshWeights[weight.mVertexId].push_back(std::make_pair(newJointIndex, weight.mWeight));
						}

						sJoint joint;
						joint.bone = bone->mName.C_Str();
						memcpy(joint.matrix, &bone->mOffsetMatrix, sizeof(float) * 16);
						aiMatrix4x4 identity;
						memcpy(joint.invBindMatrix, &identity, sizeof(float) * 16);
						skeleton.push_back(joint);
					}

					for (size_t j = 0; j < verticesCount; ++j)
					{
						weightsCount[verticesCount + j] = meshWeights[j].size();
						for (auto& pair : meshWeights[j])
						{
							weights.push_back(pair.second);
							weightsIndices.push_back(pair.first);
						}
					}
				}
				sMesh resultMesh;
				resultMesh.name = mesh->mName.C_Str();
				resultMesh.begin = indicesOffset;
				resultMesh.end = indicesOffset + indicesCount;
				resultMesh.materialName = materialNames[mesh->mMaterialIndex];
				meshes.push_back(resultMesh);
			}
		}

		if (scene->HasAnimations())
		{
			array_view<aiAnimation*> animationsView(scene->mAnimations, scene->mNumAnimations);
			for (aiAnimation* animation : animationsView)
			{
				sAnimation anim;
				anim.id = animation->mName.C_Str();
				anim.duration = static_cast<float>(animation->mDuration / (animation->mTicksPerSecond > DBL_EPSILON ? animation->mTicksPerSecond : 1.0f));
				//TODO: process animations
			}
		}

		ProcessNodes(scene->mRootNode, meshes);

		if (!hasNormals)
		{
			normals.clear();
		}
		if (!hasTexCoords)
		{
			texCoords.clear();
		}
		vertices.shrink_to_fit();
		normals.shrink_to_fit();
		texCoords.shrink_to_fit();
		indices.shrink_to_fit();
		weightsCount.shrink_to_fit();
		weights.shrink_to_fit();
		weightsIndices.shrink_to_fit();

		result->SetModel(vertices, texCoords, normals, indices, materialManager, meshes);
		if (hasWeights)
		{
			result->SetAnimation(weightsCount, weightsIndices, weights, skeleton, animations);
		}
		if (hasColors)
		{
			result->SetVertexColors(std::move(colors));
		}
		return result;
	}

	bool ModelIsSupported(unsigned char *, size_t)
	{
		return true;
	}

private:
	std::pair<std::string,Material> ProcessMaterial(aiMaterial* material)
	{
		aiString name;
		material->Get(AI_MATKEY_NAME, name);
		aiString mainTexture;
		material->GetTexture(aiTextureType_DIFFUSE, 0, &mainTexture);
		aiString specularMap;
		material->GetTexture(aiTextureType_SPECULAR, 0, &specularMap);
		aiColor3D diffuse(0.f, 0.f, 0.f);
		material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
		aiColor3D ambient(0.f, 0.f, 0.f);
		material->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
		aiColor3D specular(0.f, 0.f, 0.f);
		material->Get(AI_MATKEY_COLOR_SPECULAR, specular);
		float shineness = 0.0f;
		material->Get(AI_MATKEY_SHININESS, shineness);
		Material mat;
		memcpy(mat.ambient, &ambient, sizeof(float) * 3);
		memcpy(mat.diffuse, &diffuse, sizeof(float) * 3);
		memcpy(mat.specular, &specular, sizeof(float) * 3);
		mat.shininess = shineness;
		mat.texture = make_path(mainTexture.C_Str());
		mat.specularMap = make_path(specularMap.C_Str());
		return std::make_pair(name.C_Str(), mat);
	}

	void ProcessNodes(aiNode * node, std::vector<sMesh>& meshes)
	{
		array_view<unsigned> meshesView(node->mMeshes, node->mNumMeshes);
		for (unsigned meshIndex : meshesView)
		{
			memcpy(meshes[meshIndex].meshTransform, &node->mTransformation.a1, sizeof(Matrix4F));
		}
		//set skeleton parentNode indices based on nodes hierarchy
		array_view<aiNode*> childNodes(node->mChildren, node->mNumChildren);
		for (aiNode* child : childNodes)
		{
			ProcessNodes(child, meshes);
		}
	}
};

CAssimpModelLoader::CAssimpModelLoader()
	: m_pImpl(std::make_shared<Impl>())
{
}

CAssimpModelLoader::~CAssimpModelLoader() = default;

bool CAssimpModelLoader::ModelIsSupported(unsigned char * data, size_t size, const Path&) const
{
	return m_pImpl->ModelIsSupported(data, size);
}

std::unique_ptr<C3DModel> CAssimpModelLoader::LoadModel(unsigned char * data, size_t size, const C3DModel & dummyModel, const Path&)
{
	return m_pImpl->LoadModel(data, size, dummyModel);
}
