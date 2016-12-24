#include "3dModel.h"
#include "../model/Object.h"
#include "IRenderer.h"
#include <float.h>
#include "Matrix4.h"

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
	m_lists.clear();
	m_vertexBuffer.reset();
}

void C3DModel::SetAnimation(std::vector<unsigned int> & weightCount, std::vector<unsigned int> & weightIndexes, std::vector<float> & weights, std::vector<sJoint> & skeleton, std::vector<sAnimation> & animations)
{
	m_weightsCount.swap(weightCount);
	m_weightsIndexes.swap(weightIndexes);
	m_weights.swap(weights);
	m_skeleton.swap(skeleton);
	m_animations.swap(animations);
	m_lists.clear();
}

void SetMaterial(IRenderer & renderer, const sMaterial * material, const std::vector<sTeamColor> * teamcolor, const std::map<std::wstring, std::wstring> * replaceTextures = nullptr)
{
	if(!material)
	{
		return;
	}
	renderer.SetMaterial(material->ambient, material->diffuse, material->specular, material->shininess);
	std::wstring texture = material->texture;
	if (replaceTextures && replaceTextures->find(texture) != replaceTextures->end())
	{
		texture = replaceTextures->at(texture);
	}
	renderer.SetTexture(texture, teamcolor);
	renderer.SetTexture(material->specularMap, TextureSlot::eSpecular);
	renderer.SetTexture(material->bumpMap, TextureSlot::eBump);
}

void C3DModel::DrawModel(IRenderer & renderer, const std::set<std::string> * hideMeshes, bool vertexOnly, IVertexBuffer & vertexBuffer,
	bool useGPUskinning, const std::vector<sTeamColor> * teamcolor, const std::map<std::wstring, std::wstring> * replaceTextures)
{
	auto& shaderManager = renderer.GetShaderManager();
	vertexBuffer.Bind();
	if (useGPUskinning && m_skeleton.size() > 0)
	{
		shaderManager.SetVertexAttribute("weights", *m_weightsCache);
		shaderManager.SetVertexAttribute("weightIndices", *m_weightIndiciesCache);
	}
	renderer.PushMatrix();
	renderer.Rotate(m_rotation.x, 1.0, 0.0, 0.0);//causes transparent models
	renderer.Rotate(m_rotation.y, 0.0, 1.0, 0.0);
	renderer.Rotate(m_rotation.z, 0.0, 0.0, 1.0); 
	renderer.Scale(m_scale);
	if (!m_indexes.empty()) //Draw by meshes;
	{
		size_t begin = 0;
		size_t end;
		for (size_t i = 0; i < m_meshes.size(); ++i)
		{
			if (hideMeshes && hideMeshes->find(m_meshes[i].name) != hideMeshes->end())
			{
				end = m_meshes[i].polygonIndex;
				vertexBuffer.DrawIndexes(begin, end - begin);
				if (!vertexOnly) SetMaterial(renderer, m_materials.GetMaterial(m_meshes[i].materialName), teamcolor, replaceTextures);
				begin = (i + 1 == m_meshes.size()) ? m_count : m_meshes[i + 1].polygonIndex;
				continue;
			}
			if (vertexOnly || (i > 0 && m_meshes[i].materialName == m_meshes[i - 1].materialName))
			{
				continue;
			}
			end = m_meshes[i].polygonIndex;
			vertexBuffer.DrawIndexes(begin, end - begin);
			if (!vertexOnly) SetMaterial(renderer, m_materials.GetMaterial(m_meshes[i].materialName), teamcolor, replaceTextures);
			begin = end;
		}
		end = m_count;
		if (begin != end)
		{
			vertexBuffer.DrawIndexes(begin, end - begin);
		}
	}
	else //Draw in a row
	{
		vertexBuffer.DrawAll(m_count);
	}
	if (useGPUskinning)
	{
		float def[] = { 0.0f };
		shaderManager.DisableVertexAttribute("weights", 1, def);
		int idef = 0;
		shaderManager.DisableVertexAttribute("weightIndices", 1, &idef);
	}
	vertexBuffer.UnBind();
	sMaterial empty;
	SetMaterial(renderer, &empty, nullptr);
	renderer.PopMatrix();
}

//GPU skinning is limited to 4 weights per vertex (no more, no less). So we will add some empty weights if there is less or delete exceeding if there is more
void C3DModel::CalculateGPUWeights()
{
	unsigned int k = 0;
	for (size_t i = 0; i < m_weightsCount.size(); ++i)
	{
		unsigned int j = 0;
		float sum = 0.0f;
		for (; j < m_weightsCount[i]; ++j, ++k)
		{
			if (j < 4)
			{
				m_gpuWeight.push_back(m_weights[k]);
				m_gpuWeightIndexes.push_back(m_weightsIndexes[k]);
			}
			sum += m_weights[k];
		}
		for (; j < 4; ++j)
		{
			m_gpuWeight.push_back(0.0f);
			m_gpuWeightIndexes.push_back(0);
		}
		for (j = 0; j < 4; ++j)
		{
			m_gpuWeight[i * 4 + j] /= sum;
		}
	}
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

void AddAllChildren(std::vector<sAnimation> const& anims, unsigned int current, std::vector<unsigned int> & set)
{
	set.push_back(current);
	for (size_t i = 0; i < anims[current].children.size(); ++i)
	{
		AddAllChildren(anims, anims[current].children[i], set);
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
		std::vector<unsigned int> animsToPlay;
		for (size_t i = 0; i < animations.size(); ++i)
		{
			if (animations[i].id == animationToPlay)
			{
				AddAllChildren(animations, static_cast<unsigned>(i), animsToPlay);
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
			unsigned int k;
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
	bool gpuSkinning, const std::vector<sTeamColor> * teamcolor, const std::map<std::wstring, std::wstring> * replaceTextures)
{
	bool result;
	std::vector<float> jointMatrices = CalculateJointMatrices(m_skeleton, m_animations, animationToPlay, loop, time, result);
	if (gpuSkinning)
	{
		if (m_gpuWeight.empty())
		{
			CalculateGPUWeights();
			m_weightsCache = renderer.GetShaderManager().CreateVertexAttribCache(4, m_gpuWeight.size() / 4, m_gpuWeight.data());
			m_weightIndiciesCache = renderer.GetShaderManager().CreateVertexAttribCache(4, m_gpuWeightIndexes.size() / 4, m_gpuWeightIndexes.data());
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
		unsigned int k = 0;
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

	sModelCallListKey key;
	if (object)
	{
		key.hiddenMeshes = object->GetHiddenMeshes();
		key.teamcolor = object->GetTeamColor();
		key.replaceTextures = object->GetReplaceTextures();
	}
	key.vertexOnly = vertexOnly;
	if (!m_weightsCount.empty() && object)//object needs to be skinned
	{
		if (object->GetAnimation().empty())//no animation is playing, default pose
		{
			if (m_lists.find(key) == m_lists.end())
			{
				m_lists[key] = renderer.CreateDrawingList([&] {
					DrawSkinned(renderer, &key.hiddenMeshes, vertexOnly, "", eAnimationLoopMode::NONLOOPING, 0.0f, gpuSkinning, &key.teamcolor, &key.replaceTextures);
				});
			}
			m_lists.at(key)->Draw();
		}
		else//animation is playing, full computation
		{
			if (DrawSkinned(renderer, &key.hiddenMeshes, vertexOnly, object->GetAnimation(), object->GetAnimationLoop(), object->GetAnimationTime() / object->GetAnimationSpeed(), 
				gpuSkinning, &key.teamcolor, &key.replaceTextures))
			{
				object->PlayAnimation("");
			}
		}
	}
	else//static object
	{
		if (m_lists.find(key) == m_lists.end())
		{
			m_lists[key] = renderer.CreateDrawingList([&] {
				DrawModel(renderer, &key.hiddenMeshes, vertexOnly, *m_vertexBuffer, false, &key.teamcolor, &key.replaceTextures);
			});
		}
		m_lists.at(key)->Draw();
	}
}

void C3DModel::PreloadTextures(IRenderer & renderer) const
{
	for (size_t i = 0; i < m_meshes.size(); ++i)
	{
		if (!m_materials.GetMaterial(m_meshes[i].materialName)) continue;
		renderer.SetTexture(m_materials.GetMaterial(m_meshes[i].materialName)->texture);
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

bool operator< (sModelCallListKey const& one, sModelCallListKey const& two) 
{ 
	if (one.hiddenMeshes < two.hiddenMeshes) return true;
	if (one.hiddenMeshes > two.hiddenMeshes) return false;
	if (one.teamcolor < two.teamcolor) return true;
	if (one.teamcolor > two.teamcolor) return false;
	if (one.replaceTextures < two.replaceTextures) return true;
	if (one.replaceTextures > two.replaceTextures) return false;
	return one.vertexOnly < two.vertexOnly;
}
