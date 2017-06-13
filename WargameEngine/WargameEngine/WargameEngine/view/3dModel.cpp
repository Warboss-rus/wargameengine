#include "3dModel.h"
#include "../model/Object.h"
#include "IRenderer.h"
#include <float.h>
#include "Matrix4.h"
#include "IShaderManager.h"
#include "TextureManager.h"

namespace wargameEngine
{
namespace view
{

C3DModel::C3DModel(float scale, const CVector3f& rotations) :m_scale(scale), m_rotation(rotations), m_count(0) {}

C3DModel::~C3DModel()
{
}

void C3DModel::SetModel(std::vector<CVector3f> & vertices, std::vector<CVector2f> & textureCoords, std::vector<CVector3f> & normals, std::vector<unsigned int> & indexes,
	MaterialManager & materials, std::vector<sMesh> & meshes)
{
	m_vertices.swap(vertices);
	m_textureCoords.swap(textureCoords);
	m_normals.swap(normals);
	m_count = (indexes.empty()) ? vertices.size() : indexes.size();
	m_indexes.swap(indexes);
	std::swap(m_materials, materials);
	m_meshes.swap(meshes);
	m_vertexBuffer.reset();
	for (size_t i = 0; i < m_meshes.size(); ++i)
	{
		auto& mesh = m_meshes[i];
		mesh.material = m_materials.GetMaterial(mesh.materialName);
		mesh.end = (i == m_meshes.size() - 1) ? m_indexes.size() + 1 : m_meshes[i + 1].begin;
	}
}

void C3DModel::SetAnimation(std::vector<unsigned int> & weightCount, std::vector<unsigned int> & weightIndexes, std::vector<float> & weights, std::vector<sJoint> & skeleton, std::vector<sAnimation> & animations)
{
	m_weightsCount.swap(weightCount);
	m_weightsIndexes.swap(weightIndexes);
	m_weights.swap(weights);
	m_skeleton.swap(skeleton);
	m_animations.swap(animations);
}

void C3DModel::SetVertexColors(std::vector<math::vec4>&& colors)
{
	m_vertexColors = std::move(colors);
}

void C3DModel::GetModelMeshes(IRenderer& renderer, TextureManager& textureManager, MeshList& meshesVec, const std::set<std::string>* hideMeshes,
	IVertexBuffer* vertexBuffer, const std::vector<model::TeamColor>* teamcolor, const std::unordered_map<Path, Path>* replaceTextures, 
	const std::shared_ptr<std::vector<float>>& skeleton, const std::shared_ptr<TempMeshBuffer>& tempBuffer) const
{
	renderer.PushMatrix();
	renderer.Rotate(m_rotation);
	renderer.Scale(m_scale);
	const Matrix4F modelMatrix = renderer.GetModelMatrix();
	renderer.PopMatrix();
	const bool indexed = !m_indexes.empty();
	for (const sMesh& mesh : m_meshes)
	{
		if (hideMeshes && hideMeshes->find(mesh.name) != hideMeshes->end())
		{
			continue;
		}
		Material* material = mesh.material;
		ICachedTexture* texture = GetTexturePtr(material, replaceTextures, textureManager, teamcolor);
		auto& prev = meshesVec.back();
		if (prev.buffer == vertexBuffer && material == prev.material && texture == prev.texturePtr && prev.start + prev.count == mesh.begin)
		{
			prev.count += mesh.end - mesh.begin;
		}
		else
		{
			meshesVec.emplace_back(DrawableMesh{ nullptr, texture, material, vertexBuffer, modelMatrix * mesh.meshTransform, tempBuffer, skeleton, mesh.begin, mesh.end - mesh.begin, indexed });
		}
	}
}

//GPU skinning is limited to 4 weights per vertex (no more, no less). So we will add some empty weights if there is less or delete exceeding if there is more
void C3DModel::CalculateGPUWeights(IRenderer & renderer)
{
	std::vector<int> gpuWeightIndexes;
	std::vector<float> gpuWeight;
	gpuWeightIndexes.reserve(m_weightsCount.size() * 4);
	gpuWeight.reserve(m_weightsCount.size() * 4);
	size_t k = 0;
	for (size_t i = 0; i < m_weightsCount.size(); ++i)
	{
		unsigned int j = 0;
		float sum = 0.0f;
		for (; j < m_weightsCount[i]; ++j, ++k)
		{
			if (j < 4)
			{
				gpuWeight.push_back(m_weights[k]);
				gpuWeightIndexes.push_back(m_weightsIndexes[k]);
			}
			sum += m_weights[k];
		}
		for (; j < 4; ++j)
		{
			gpuWeight.push_back(0.0f);
			gpuWeightIndexes.push_back(0);
		}
		for (j = 0; j < 4; ++j)
		{
			gpuWeight[i * 4 + j] /= sum;
		}
	}
	renderer.AddVertexAttribute(*m_vertexBuffer, "weights", 4, gpuWeight.size() / 4, IShaderManager::Format::Float32, gpuWeight.data());
	renderer.AddVertexAttribute(*m_vertexBuffer, "weightIndices", 4, gpuWeightIndexes.size() / 4, IShaderManager::Format::SInt32, gpuWeightIndexes.data());
}

void MultiplyVectorToMatrix(CVector3f & vect, float * matrix)
{
	if (!matrix) return;
	float result[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result[i] += matrix[i * 4 + j] * ((j == 3) ? 1.0f : vect[j]);
		}
	}
	if (fabs(result[3]) > FLT_EPSILON)
	{
		for (int i = 0; i < 3; ++i)
		{
			result[i] /= result[3];
		}
	}
	vect = CVector3f(result);
}


void MultiplyMatrices(float * a, float * b)
{
	float c[16];
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			c[i * 4 + j] = 0;
			for (int k = 0; k < 4; k++)
			{
				c[i * 4 + j] = c[i * 4 + j] + a[i * 4 + k] * b[k * 4 + j];
			}
		}
	}
	memcpy(a, c, sizeof(float) * 16);
}

void AddAllChildren(std::vector<sAnimation> const& anims, size_t current, std::vector<size_t> & set)
{
	set.push_back(current);
	auto& children = anims[current].children;
	for (size_t i = 0; i < children.size(); ++i)
	{
		AddAllChildren(anims, children[i], set);
	}
}

void InterpolateMatrices(float * m1, const float * m2, float t)//works bad if matrices are strongly differ
{
	for (size_t i = 0; i < 16; ++i)
	{
		m1[i] = m1[i] * t + m2[i] * (1.0f - t);
	}
}

std::vector<float> CalculateJointMatrices(std::vector<sJoint> const& skeleton, std::vector<sAnimation> const& animations, std::string const& animationToPlay, model::AnimationLoop loop, float time)
{
	//copy all matrices
	std::vector<float> jointMatrices;
	jointMatrices.resize(skeleton.size() * 16);
	for (size_t i = 0; i < skeleton.size(); ++i)
	{
		memcpy(&jointMatrices[i * 16], skeleton[i].matrix, sizeof(float) * 16);
	}
	//apply animations
	if (!animationToPlay.empty())
	{
		//get animations that are need to be played
		std::vector<size_t> animsToPlay;
		for (size_t i = 0; i < animations.size(); ++i)
		{
			if (animations[i].id == animationToPlay)
			{
				AddAllChildren(animations, i, animsToPlay);
				if (time > animations[i].duration)
				{
					if (loop == model::AnimationLoop::Looping)
					{
						time = fmod(time, animations[i].duration);
					}
					else if (loop == model::AnimationLoop::HoldEnd)
					{
						time = animations[i].duration;
					}
				}
				break;
			}
		}
		//replace affected joints with animation matrices
		for (size_t i = 0; i < animsToPlay.size(); ++i)
		{
			const sAnimation * anim = &animations[animsToPlay[i]];
			size_t k;
			for (k = 0; k < anim->keyframes.size(); ++k)
			{
				if (time <= anim->keyframes[k] && (k == 0 || time > anim->keyframes[k - 1]))
				{
					break;
				}
			}
			if (k < anim->keyframes.size())
			{
				for (size_t j = 0; j < 16; ++j)
				{
					jointMatrices[anim->boneIndex * 16 + j] = anim->matrices[k * 16 + j];
				}
				if (k > 0)
				{
					InterpolateMatrices(&jointMatrices[anim->boneIndex * 16], &anim->matrices[(k - 1) * 16], (time - anim->keyframes[k - 1]) / (anim->keyframes[k] - anim->keyframes[k - 1]));
				}
			}
		}
	}
	//cycle through all joints and multiply them to their parents
	for (size_t i = 0; i < skeleton.size(); ++i)
	{
		if (skeleton[i].parentIndex != -1)
		{
			float parent[16];
			memcpy(parent, &jointMatrices[skeleton[i].parentIndex * 16], sizeof(float) * 16);
			MultiplyMatrices(parent, &jointMatrices[i * 16]);
			memcpy(&jointMatrices[i * 16], parent, sizeof(float) * 16);
		}
	}
	//multiply all matrices by invBindMatrices
	for (size_t i = 0; i < skeleton.size(); ++i)
	{
		Matrix4F m(skeleton[i].invBindMatrix);
		m *= &jointMatrices[i * 16];
		memcpy(&jointMatrices[i * 16], m, sizeof(float) * 16);
	}
	return jointMatrices;
}

//returns if animations is ended
void C3DModel::GetMeshesSkinned(IRenderer & renderer, TextureManager& textureManager, MeshList& meshesVec, const std::set<std::string> * hideMeshes,
	std::string const& animationToPlay, model::AnimationLoop loop, float time, bool gpuSkinning, const std::vector<model::TeamColor> * teamcolor, 
	const std::unordered_map<Path, Path> * replaceTextures)
{
	const auto jointMatrices = std::make_shared<std::vector<float>>(CalculateJointMatrices(m_skeleton, m_animations, animationToPlay, loop, time));
	if (gpuSkinning)
	{
		return GetModelMeshes(renderer, textureManager, meshesVec, hideMeshes, m_vertexBuffer.get(), teamcolor, replaceTextures, jointMatrices, nullptr);
	}
	else
	{
		std::vector<CVector3f> vertices;
		std::vector<CVector3f> normals;
		vertices.resize(m_vertices.size());
		normals.reserve(m_normals.size() * 3);
		size_t k = 0;
		for (size_t i = 0; i < m_vertices.size(); ++i)
		{
			//recalculate vertex using bones
			for (size_t j = 0; j < m_weightsCount[i]; ++j, ++k)
			{
				CVector3f vertex = m_vertices[i];
				sJoint * joint = &m_skeleton[m_weightsIndexes[k]];
				MultiplyVectorToMatrix(vertex, joint->invBindMatrix);
				MultiplyVectorToMatrix(vertex, &(*jointMatrices)[m_weightsIndexes[k] * 16]);
				vertex *= m_weights[k];
				vertices[i] += vertex;
				CVector3f normal = m_normals[i];
				MultiplyVectorToMatrix(normal, joint->invBindMatrix);
				MultiplyVectorToMatrix(normal, &(*jointMatrices)[m_weightsIndexes[k] * 16]);
				normal *= m_weights[k];
				normals[i] = normal;
			}
		}
		auto tempVertices = std::make_shared<TempMeshBuffer>(TempMeshBuffer{vertices, normals, m_textureCoords.data(), m_indexes.data(), m_indexes.size()});
		return GetModelMeshes(renderer, textureManager, meshesVec, hideMeshes, m_vertexBuffer.get(), teamcolor, replaceTextures, nullptr, tempVertices);
	}
}

void C3DModel::GetMeshes(IRenderer & renderer, TextureManager& textureManager, model::IObject* object, bool gpuSkinning, MeshList& meshesVec)
{
	if (!m_vertexBuffer && !m_vertices.empty())
	{
		m_vertexBuffer = renderer.CreateVertexBuffer(&m_vertices.data()->x, &m_normals.data()->x, &m_textureCoords.data()->x, m_vertices.size(), false);
		if (!m_indexes.empty())
		{
			renderer.SetIndexBuffer(*m_vertexBuffer, m_indexes.data(), m_indexes.size());
		}
		if (!m_vertexColors.empty())
		{
			renderer.AddVertexAttribute(*m_vertexBuffer, "VertexColor", 4, m_vertexColors.size(), IShaderManager::Format::Float32, m_vertexColors.data());
		}
		if (!m_weightsCount.empty())
		{
			CalculateGPUWeights(renderer);
		}
	}

	auto* hiddenMeshes = (object && !object->GetHiddenMeshes().empty()) ? &object->GetHiddenMeshes() : nullptr;
	auto* teamcolor = (object && !object->GetTeamColor().empty()) ? &object->GetTeamColor() : nullptr;
	auto* replaceTextures = (object && !object->GetReplaceTextures().empty()) ? &object->GetReplaceTextures() : nullptr;
	if (!m_weightsCount.empty() && object)//object needs to be skinned
	{
		return GetMeshesSkinned(renderer, textureManager, meshesVec, hiddenMeshes, object->GetAnimation(), object->GetAnimationLoop(), object->GetAnimationTime() / object->GetAnimationSpeed(),
			gpuSkinning, teamcolor, replaceTextures);
	}
	else//static object
	{
		return GetModelMeshes(renderer, textureManager, meshesVec, hiddenMeshes, m_vertexBuffer.get(), teamcolor, replaceTextures, nullptr, nullptr);
	}
}

float C3DModel::GetScale() const
{
	return m_scale;
}

CVector3f C3DModel::GetRotation() const
{
	return m_rotation;
}

void C3DModel::PreloadTextures(TextureManager& textureManager) const
{
	for (size_t i = 0; i < m_meshes.size(); ++i)
	{
		if (m_meshes[i].material)
		{
			textureManager.GetTexturePtr(m_meshes[i].material->texture);
		}
	}
}

std::vector<std::string> C3DModel::GetAnimations() const
{
	std::vector<std::string> result;
	for (size_t i = 0; i < m_animations.size(); ++i)
	{
		result.push_back(m_animations[i].id);
	}
	return result;
}

ICachedTexture* C3DModel::GetTexturePtr(Material* material, const std::unordered_map<Path, Path>* replaceTextures, TextureManager &textureManager, const std::vector<model::TeamColor>* teamcolor) const
{
	if (material && !material->texture.empty())
	{
		if (replaceTextures && replaceTextures->find(material->texture) != replaceTextures->end())
		{
			return textureManager.GetTexturePtr(replaceTextures->at(material->texture), teamcolor);
		}
		if (teamcolor)
		{
			return textureManager.GetTexturePtr(material->texture, teamcolor);
		}
		if (!material->texturePtr)
		{
			material->texturePtr = textureManager.GetTexturePtr(material->texture);
		}
		return material->texturePtr;
	}
	return nullptr;
}

}
}