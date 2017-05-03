#include "3dModel.h"
#include "../model/Object.h"
#include "IRenderer.h"
#include <float.h>
#include "Matrix4.h"
#include <algorithm>
#include "IShaderManager.h"

C3DModel::C3DModel(double scale, double rotateX, double rotateY, double rotateZ):m_scale(scale), m_rotation(rotateX, rotateY, rotateZ), m_count(0) {}

C3DModel::C3DModel(C3DModel const& other)
	: m_vertices(other.m_vertices), m_textureCoords(other.m_textureCoords), m_normals(other.m_normals), m_indexes(other.m_indexes), m_weightsCount(other.m_weightsCount)
	, m_weightsIndexes(other.m_weightsIndexes), m_weights(other.m_weights), m_skeleton(other.m_skeleton)
	, m_animations(other.m_animations), m_meshes(other.m_meshes), m_materials(other.m_materials), m_scale(other.m_scale), m_rotation(other.m_rotation), m_count(other.m_count)
{
}

C3DModel::~C3DModel()
{
}

void C3DModel::SetModel(std::vector<CVector3f> & vertices, std::vector<CVector2f> & textureCoords, std::vector<CVector3f> & normals, std::vector<unsigned int> & indexes,
	CMaterialManager & materials, std::vector<sMesh> & meshes)
{
	m_vertices.swap(vertices);
	m_textureCoords.swap(textureCoords);
	m_normals.swap(normals);
	m_count = (indexes.empty())?vertices.size():indexes.size();
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
	std::sort(m_meshes.begin(), m_meshes.end(), [](sMesh const& first, sMesh const& second) {
		return first.materialName < second.materialName;
	});
}

void C3DModel::SetAnimation(std::vector<unsigned int> & weightCount, std::vector<unsigned int> & weightIndexes, std::vector<float> & weights, std::vector<sJoint> & skeleton, std::vector<sAnimation> & animations)
{
	m_weightsCount.swap(weightCount);
	m_weightsIndexes.swap(weightIndexes);
	m_weights.swap(weights);
	m_skeleton.swap(skeleton);
	m_animations.swap(animations);
}

void SetMaterial(IRenderer & renderer, sMaterial * material, const std::vector<sTeamColor> * teamcolor, const std::map<Path, Path> * replaceTextures = nullptr)
{
	if(!material)
	{
		renderer.SetTexture(Path());
		return;
	}
	sMaterial& mat = *material;
	renderer.SetMaterial(mat.ambient, mat.diffuse, mat.specular, mat.shininess);
	if (!replaceTextures && !teamcolor)
	{
		if (!mat.texturePtr)
		{
			mat.texturePtr = renderer.GetTexturePtr(mat.texture);
		}
		renderer.SetTexture(*mat.texturePtr);
	}
	else 
	{
		Path texture = material->texture;
		if (replaceTextures && replaceTextures->find(texture) != replaceTextures->end())
		{
			texture = replaceTextures->at(texture);
		}
		renderer.SetTexture(texture, teamcolor);
	}
	if (!mat.specularMap.empty()) renderer.SetTexture(mat.specularMap, TextureSlot::eSpecular);
	if (!mat.bumpMap.empty()) renderer.SetTexture(mat.bumpMap, TextureSlot::eBump);
}

void C3DModel::DrawModel(IRenderer & renderer, const std::set<std::string> * hideMeshes, bool vertexOnly, IVertexBuffer & vertexBuffer,
	bool useGPUskinning, const std::vector<sTeamColor> * teamcolor, const std::map<Path, Path> * replaceTextures)
{
	auto& shaderManager = renderer.GetShaderManager();
	if (useGPUskinning && m_skeleton.size() > 0)
	{
		shaderManager.SetVertexAttribute("weights", *m_weightsCache, 4, m_weightsCount.size() * 4, IShaderManager::TYPE::FLOAT32);
		shaderManager.SetVertexAttribute("weightIndices", *m_weightIndiciesCache, 4, m_weightsCount.size() * 4, IShaderManager::TYPE::FLOAT32);
	}
	renderer.PushMatrix();
	renderer.Rotate(m_rotation.x, 1.0, 0.0, 0.0);//causes transparent models
	renderer.Rotate(m_rotation.y, 0.0, 1.0, 0.0);
	renderer.Rotate(m_rotation.z, 0.0, 0.0, 1.0); 
	renderer.Scale(m_scale);
	if (!m_indexes.empty()) //Draw by meshes;
	{
		sMaterial * material = nullptr;
		for (size_t i = 0; i < m_meshes.size(); ++i)
		{
			sMesh& mesh = m_meshes[i];
			if (hideMeshes && hideMeshes->find(mesh.name) != hideMeshes->end())
			{
				continue;
			}
			if (!vertexOnly && mesh.material != material)
			{
				material = mesh.material;
				SetMaterial(renderer, material, teamcolor, replaceTextures);
			}
			renderer.DrawIndexes(vertexBuffer, mesh.begin, mesh.end - mesh.begin);
		}
	}
	else //Draw in a row
	{
		renderer.DrawAll(vertexBuffer, m_count);
	}
	if (useGPUskinning)
	{
		float def[] = { 0.0f };
		shaderManager.DisableVertexAttribute("weights", 1, def);
		int idef = 0;
		shaderManager.DisableVertexAttribute("weightIndices", 1, &idef);
	}
	renderer.PopMatrix();
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
	m_weightsCache = renderer.GetShaderManager().CreateVertexAttribCache(gpuWeight.size() * sizeof(float), gpuWeight.data());
	m_weightIndiciesCache = renderer.GetShaderManager().CreateVertexAttribCache(gpuWeightIndexes.size() * sizeof(int), gpuWeightIndexes.data());
}

void MultiplyVectorToMatrix(CVector3f & vect, float * matrix)
{
	float result[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result[i] += matrix[i * 4 + j] * ((j == 3)?1.0f:vect[j]);
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

std::vector<float> CalculateJointMatrices(std::vector<sJoint> const& skeleton, std::vector<sAnimation> const& animations, std::string const& animationToPlay, eAnimationLoopMode loop, 
	float time, bool & animationIsEnded)
{
	animationIsEnded = false;
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
					if (loop == eAnimationLoopMode::LOOPING)
					{
						time = fmod(time, animations[i].duration);
					}
					else if (loop == eAnimationLoopMode::HOLDEND)
					{
						time = animations[i].duration;
					}
					else
					{
						animationIsEnded = true;
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
bool C3DModel::DrawSkinned(IRenderer & renderer, const std::set<std::string> * hideMeshes, bool vertexOnly, std::string const& animationToPlay, eAnimationLoopMode loop, float time, 
	bool gpuSkinning, const std::vector<sTeamColor> * teamcolor, const std::map<Path, Path> * replaceTextures)
{
	bool result;
	std::vector<float> jointMatrices = CalculateJointMatrices(m_skeleton, m_animations, animationToPlay, loop, time, result);
	if (gpuSkinning)
	{
		if (!m_weightsCache)
		{
			CalculateGPUWeights(renderer);
		}
		renderer.GetShaderManager().SetUniformValue("joints", 16, m_skeleton.size(), jointMatrices.data());
		DrawModel(renderer, hideMeshes, vertexOnly, *m_vertexBuffer, true, teamcolor, replaceTextures);
	}
	else
	{
		std::vector<CVector3f> vertices;
		std::vector<CVector3f> normals;
		vertices.resize(m_vertices.size());
		normals.resize(m_normals.size());
		size_t k = 0;
		for (size_t i = 0; i < m_vertices.size(); ++i)
		{
			//recalculate vertex using bones
			for (size_t j = 0; j < m_weightsCount[i]; ++j, ++k)
			{
				CVector3f vertex = m_vertices[i];
				sJoint * joint = &m_skeleton[m_weightsIndexes[k]];
				MultiplyVectorToMatrix(vertex, joint->invBindMatrix);
				MultiplyVectorToMatrix(vertex, &jointMatrices[m_weightsIndexes[k] * 16]);
				vertex *= m_weights[k];
				vertices[i] += vertex;
				CVector3f normal = m_normals[i];
				MultiplyVectorToMatrix(normal, joint->invBindMatrix);
				MultiplyVectorToMatrix(normal, &jointMatrices[m_weightsIndexes[k] * 16]);
				normal *= m_weights[k];
				normals[i] += normal;
			}
		}
		auto vertexBuffer = renderer.CreateVertexBuffer(&vertices.data()->x, &normals.data()->x, &m_textureCoords.data()->x, vertices.size(), true);
		vertexBuffer->SetIndexBuffer(m_indexes.data(), m_indexes.size());
		DrawModel(renderer, hideMeshes, vertexOnly, *vertexBuffer, false, teamcolor, replaceTextures);
	}
	return result;
}

void C3DModel::Draw(IRenderer & renderer, IObject* object, bool vertexOnly, bool gpuSkinning)
{
	if (!m_vertexBuffer)
	{
		m_vertexBuffer = renderer.CreateVertexBuffer(&m_vertices.data()->x, &m_normals.data()->x, &m_textureCoords.data()->x, m_vertices.size(), false);
		m_vertexBuffer->SetIndexBuffer(m_indexes.data(), m_indexes.size());
	}

	auto* hiddenMeshes = (object && !object->GetHiddenMeshes().empty()) ? &object->GetHiddenMeshes() : nullptr;
	auto* teamcolor = (object && !object->GetTeamColor().empty()) ? &object->GetTeamColor() : nullptr;
	auto* replaceTextures = (object && !object->GetReplaceTextures().empty()) ? &object->GetReplaceTextures() : nullptr;
	if (!m_weightsCount.empty() && object)//object needs to be skinned
	{
		if (object->GetAnimation().empty())//no animation is playing, default pose
		{
			DrawSkinned(renderer, hiddenMeshes, vertexOnly, "", eAnimationLoopMode::NONLOOPING, 0.0f, gpuSkinning, teamcolor, replaceTextures);
		}
		else//animation is playing, full computation
		{
			if (DrawSkinned(renderer, hiddenMeshes, vertexOnly, object->GetAnimation(), object->GetAnimationLoop(), object->GetAnimationTime() / object->GetAnimationSpeed(),
				gpuSkinning, teamcolor, replaceTextures))
			{
				object->PlayAnimation("");
			}
		}
	}
	else//static object
	{
		DrawModel(renderer, hiddenMeshes, vertexOnly, *m_vertexBuffer, false, teamcolor, replaceTextures);
	}
}

void C3DModel::PreloadTextures(IRenderer & renderer) const
{
	for (size_t i = 0; i < m_meshes.size(); ++i)
	{
		if (m_meshes[i].material) continue;
		renderer.SetTexture(m_meshes[i].material->texture);
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
