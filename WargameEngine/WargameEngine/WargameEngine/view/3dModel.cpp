#include "3dModel.h"
#define GLEW_STATIC
#include <GL/glew.h>
#include "gl.h"
#include "GameView.h"
#include "../model/Object.h"

C3DModel::C3DModel(double scale, double rotateX, double rotateY, double rotateZ):m_scale(scale), m_rotX(rotateX), m_rotY(rotateY), m_rotZ(rotateZ) {}

C3DModel::C3DModel(std::vector<CVector3f> & vertices, std::vector<CVector2f> & textureCoords, std::vector<CVector3f> & normals, std::vector<unsigned int> & indexes,
				   CMaterialManager & materials, std::vector<sMesh> & meshes, double scale):m_scale(scale)
{
	SetModel(vertices, textureCoords, normals, indexes, materials, meshes);
}

C3DModel::~C3DModel()
{
	if (m_vbo) glDeleteBuffersARB(1, &m_vbo);
	for (auto i = m_lists.begin(); i != m_lists.end(); ++i)
	{
		glDeleteLists(i->second, 1);
	}
}

void C3DModel::SetModel(std::vector<CVector3f> & vertices, std::vector<CVector2f> & textureCoords, std::vector<CVector3f> & normals, std::vector<unsigned int> & indexes,
	CMaterialManager & materials, std::vector<sMesh> & meshes)
{
	m_vbo = NULL;
#ifdef _USE_VBO
	if (GLEW_ARB_vertex_buffer_object)
	{
		glGenBuffersARB(1, &m_vbo);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vbo);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB, vertices.size() * 3 * sizeof(float) + normals.size() * 3 * sizeof(float) + textureCoords.size() * 2 * sizeof(float), NULL, GL_STATIC_DRAW_ARB);
		glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, vertices.size() * 3 * sizeof(float), &vertices[0]);
		glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, vertices.size() * 3 * sizeof(float), normals.size() * 3 * sizeof(float), &normals[0]);
		glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, vertices.size() * 3 * sizeof(float)+normals.size() * 3 * sizeof(float), textureCoords.size() * 2 * sizeof(float), &textureCoords[0]);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	}
#endif
	m_vertices.swap(vertices);
	m_textureCoords.swap(textureCoords);
	m_normals.swap(normals);
	m_count = (indexes.empty())?vertices.size():indexes.size();
	m_indexes.swap(indexes);
	std::swap(m_materials, materials);
	m_meshes.swap(meshes);
	for (auto i = m_lists.begin(); i != m_lists.end(); ++i)
	{
		glDeleteLists(i->second, 1);
	}
	m_lists.clear();
}

void C3DModel::SetAnimation(std::vector<unsigned int> & weightCount, std::vector<unsigned int> & weightIndexes, std::vector<float> & weights, std::vector<sJoint> & skeleton, std::vector<sAnimation> & animations)
{
	m_weightsCount.swap(weightCount);
	m_weightsIndexes.swap(weightIndexes);
	m_weights.swap(weights);
	m_skeleton.swap(skeleton);
	m_animations.swap(animations);
	for (auto i = m_lists.begin(); i != m_lists.end(); ++i)
	{
		glDeleteLists(i->second, 1);
	}
	m_lists.clear();
}

void SetMaterial(const sMaterial * material, const std::vector<sTeamColor> * teamcolor, const std::map<std::string, std::string> * replaceTextures = nullptr)
{
	if(!material)
	{
		return;
	}
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT, material->ambient);
	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE, material->diffuse);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR, material->specular);
	glMaterialf(GL_FRONT,GL_SHININESS, material->shininess);
	CTextureManager * texManager = CTextureManager::GetInstance();
	std::string texture = material->texture;
	if (replaceTextures && replaceTextures->find(texture) != replaceTextures->end())
	{
		texture = replaceTextures->at(texture);
	}
	texManager->SetTexture(texture, teamcolor);
	texManager->SetTexture(material->specularMap, CTextureManager::eSpecular);
	texManager->SetTexture(material->bumpMap, CTextureManager::eBump);
}

void C3DModel::DrawModel(const std::set<std::string> * hideMeshes, bool vertexOnly, std::vector<CVector3f> const& vertices, std::vector<CVector3f> const& normals, bool useGPUskinning, const std::vector<sTeamColor> * teamcolor, const std::map<std::string, std::string> * replaceTextures)
{
	if (useGPUskinning && m_skeleton.size() > 0)
	{
		const CShaderManager * shader = CGameView::GetInstance().lock()->GetShaderManager();
		shader->SetUniformMatrix4("invBindMatrices", m_skeleton.size(), &m_gpuInverseMatrices[0]);
		shader->SetVertexAttrib4(CShaderManager::eUniformIndex::WEIGHT, &m_gpuWeight[0]);
		shader->SetVertexAttrib4(CShaderManager::eUniformIndex::WEIGHT_INDEX, &m_gpuWeightIndexes[0]);
	}
	if (m_vbo)
	{
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vbo);
		if (!vertices.empty())
		{
			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(3, GL_FLOAT, 0, 0);
		}
		if (!normals.empty() && !vertexOnly)
		{
			glEnableClientState(GL_NORMAL_ARRAY);
			glNormalPointer(GL_FLOAT, 0, (void*)(vertices.size() * 3 * sizeof(float)));
		}
		if (!m_textureCoords.empty() && !vertexOnly)
		{
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, GL_FLOAT, 0, (void*)(vertices.size() * 3 * sizeof(float)+normals.size() * 3 * sizeof(float)));
		}
	}
	else
	{
		if (!vertices.empty())
		{
			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(3, GL_FLOAT, 0, &vertices[0]);
		}
		if (!normals.empty() && !vertexOnly)
		{
			glEnableClientState(GL_NORMAL_ARRAY);
			glNormalPointer(GL_FLOAT, 0, &normals[0]);
		}
		if (!m_textureCoords.empty() && !vertexOnly)
		{
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, GL_FLOAT, 0, &m_textureCoords[0]);
		}
	}
	glPushMatrix();
	glRotated(m_rotX, 1.0, 0.0, 0.0);//causes transparent models
	glRotated(m_rotY, 0.0, 1.0, 0.0);
	glRotated(m_rotZ, 0.0, 0.0, 1.0); 
	glScaled(m_scale, m_scale, m_scale);
	if (!m_indexes.empty()) //Draw by meshes;
	{
		unsigned int begin = 0;
		unsigned int end;
		for (size_t i = 0; i < m_meshes.size(); ++i)
		{
			if (hideMeshes && hideMeshes->find(m_meshes[i].name) != hideMeshes->end())
			{
				end = m_meshes[i].polygonIndex;
				glDrawElements(GL_TRIANGLES, end - begin, GL_UNSIGNED_INT, &m_indexes[begin]);
				SetMaterial(m_materials.GetMaterial(m_meshes[i].materialName), teamcolor, replaceTextures);
				begin = (i + 1 == m_meshes.size()) ? m_count : m_meshes[i + 1].polygonIndex;
				continue;
			}
			if (vertexOnly || (i > 0 && m_meshes[i].materialName == m_meshes[i - 1].materialName))
			{
				continue;
			}
			end = m_meshes[i].polygonIndex;
			glDrawElements(GL_TRIANGLES, end - begin, GL_UNSIGNED_INT, &m_indexes[begin]);
			if (!vertexOnly) SetMaterial(m_materials.GetMaterial(m_meshes[i].materialName), teamcolor, replaceTextures);
			begin = end;
		}
		end = m_count;
		if (begin != end)
		{
			glDrawElements(GL_TRIANGLES, end - begin, GL_UNSIGNED_INT, &m_indexes[begin]);
		}
	}
	else //Draw in a row
	{
		glDrawArrays(GL_TRIANGLES, 0, m_count);
	}
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	sMaterial empty;
	SetMaterial(&empty, nullptr);
	glPopMatrix();
	if (m_vbo) glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	if (useGPUskinning)
	{
		const CShaderManager * shader = CGameView::GetInstance().lock()->GetShaderManager();
		float def[1] = { 0.0f };
		shader->DisableVertexAttrib4(16, def);
		int idef = 0;
		shader->DisableVertexAttrib4(17, &idef);
	}
}

//GPU skinning is limited to 4 weights per vertex (no more, no less). So we will add some empty weights if there is less or delete exceeding if there is more
void C3DModel::CalculateGPUWeights()
{
	unsigned int k = 0;
	for (size_t i = 0; i < m_weightsCount.size(); ++i)
	{
		unsigned int j = 0;
		float sum = 0.0f;
		for (j; j < m_weightsCount[i]; ++j, ++k)
		{
			if (j < 4)
			{
				m_gpuWeight.push_back(m_weights[k]);
				m_gpuWeightIndexes.push_back(m_weightsIndexes[k]);
			}
			sum += m_weights[k];
		}
		for (j; j < 4; ++j)
		{
			m_gpuWeight.push_back(0.0f);
			m_gpuWeightIndexes.push_back(0);
		}
		for (j = 0; j < 4; ++j)
		{
			m_gpuWeight[i * 4 + j] /= sum;
		}
	}
	m_gpuInverseMatrices.resize(m_skeleton.size() * 16);
	for (size_t i = 0; i < m_skeleton.size(); ++i)
	{
		memcpy(&m_gpuInverseMatrices[i * 16], m_skeleton[i].invBindMatrix, sizeof(float) * 16);
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
	if (result[3] != 0.0f)
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

std::vector<float> CalculateJointMatrices(std::vector<sJoint> const& skeleton, std::vector<sAnimation> const& animations, std::string const& animationToPlay, sAnimation::eLoopMode loop, float time, bool & animationIsEnded)
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
				AddAllChildren(animations, i, animsToPlay);
				if (time > animations[i].duration)
				{
					if (loop == sAnimation::LOOPING)
					{
						time = fmod(time, animations[i].duration);
					}
					else if (loop == sAnimation::HOLDEND)
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
	return jointMatrices;
}

//returns if animations is ended
bool C3DModel::DrawSkinned(const std::set<std::string> * hideMeshes, bool vertexOnly, std::string const& animationToPlay, sAnimation::eLoopMode loop, float time, bool gpuSkinning, const std::vector<sTeamColor> * teamcolor, const std::map<std::string, std::string> * replaceTextures)
{
	bool result;
	std::vector<float> jointMatrices = CalculateJointMatrices(m_skeleton, m_animations, animationToPlay, loop, time, result);
	if (gpuSkinning)
	{
		if (m_gpuWeight.empty())
		{
			CalculateGPUWeights();
		}
		CGameView::GetInstance().lock()->GetShaderManager()->SetUniformMatrix4("joints", m_skeleton.size(), &jointMatrices[0]);
		DrawModel(hideMeshes, vertexOnly, m_vertices, m_normals, true, teamcolor, replaceTextures);
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
		DrawModel(hideMeshes, vertexOnly, vertices, normals, false, teamcolor, replaceTextures);
	}
	return result;
}

void C3DModel::Draw(std::shared_ptr<IObject> object, bool vertexOnly, bool gpuSkinning)
{
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
				unsigned int id = glGenLists(1);
				glNewList(id, GL_COMPILE);
				DrawSkinned(&key.hiddenMeshes, vertexOnly, "", sAnimation::NONLOOPING, 0.0f, gpuSkinning, &key.teamcolor, &key.replaceTextures);
				glEndList();
				m_lists[key] = id;
			}
			else glCallList(m_lists[key]);
		}
		else//animation is playing, full computation
		{
			if (DrawSkinned(&key.hiddenMeshes, vertexOnly, object->GetAnimation(), object->GetAnimationLoop(), object->GetAnimationTime() / object->GetAnimationSpeed(), gpuSkinning, &key.teamcolor, &key.replaceTextures))
			{
				object->PlayAnimation("");
			}
		}
	}
	else//static object
	{
		if (m_lists.find(key) == m_lists.end())
		{
			unsigned int id = glGenLists(1);
			glNewList(id, GL_COMPILE);
			DrawModel(&key.hiddenMeshes, false, m_vertices, m_normals, false, &key.teamcolor, &key.replaceTextures);
			glEndList();
			m_lists[key] = id;
		}
		else
		{
			glCallList(m_lists[key]);
		}
	}
}

void C3DModel::PreloadTextures() const
{
	return;
	CTextureManager * texManager = CTextureManager::GetInstance();
	for (size_t i = 0; i < m_meshes.size(); ++i)
	{
		if (!m_materials.GetMaterial(m_meshes[i].materialName)) continue;
		texManager->SetTexture(m_materials.GetMaterial(m_meshes[i].materialName)->texture);
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
