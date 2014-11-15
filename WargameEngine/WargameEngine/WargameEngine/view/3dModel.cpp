#include "3dModel.h"
#define GLEW_STATIC
#include <GL/glew.h>
#include "gl.h"
#include "GameView.h"
#include "../model/Object.h"

C3DModel::C3DModel(std::shared_ptr<IBounding> bounding, double scale)
{ 
	m_bounding = bounding; 
	m_scale = scale; 
}

C3DModel::C3DModel(std::vector<CVector3f> & vertices, std::vector<CVector2f> & textureCoords, std::vector<CVector3f> & normals, std::vector<unsigned int> & indexes,
				   CMaterialManager & materials, std::vector<sMesh> & meshes, std::shared_ptr<IBounding> bounding, double scale)
{
	SetModel(vertices, textureCoords, normals, indexes, materials, meshes);
	m_bounding = bounding;
	m_scale = scale;
}

void DeleteList(std::map<std::set<std::string>, unsigned int> const& list)
{
	for (auto i = list.begin(); i != list.end(); ++i)
	{
		glDeleteLists(i->second, 1);
	}
}

C3DModel::~C3DModel()
{
	if (m_vbo) glDeleteBuffersARB(1, &m_vbo);
	DeleteList(m_lists);
	DeleteList(m_vertexLists);
}

void C3DModel::SetModel(std::vector<CVector3f> & vertices, std::vector<CVector2f> & textureCoords, std::vector<CVector3f> & normals, std::vector<unsigned int> & indexes,
	CMaterialManager & materials, std::vector<sMesh> & meshes)
{
	m_vbo = NULL;
	if (NULL && GLEW_ARB_vertex_buffer_object)//will be needed for animation
	{
		glGenBuffersARB(1, &m_vbo);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vbo);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB, vertices.size() * 3 * sizeof(float) + normals.size() * 3 * sizeof(float) + textureCoords.size() * 2 * sizeof(float), NULL, GL_STATIC_DRAW_ARB);
		glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, vertices.size() * 3 * sizeof(float), &vertices[0]);
		glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, vertices.size() * 3 * sizeof(float), normals.size() * 3 * sizeof(float), &normals[0]);
		glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, vertices.size() * 3 * sizeof(float)+normals.size() * 3 * sizeof(float), textureCoords.size() * 2 * sizeof(float), &textureCoords[0]);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	}
	m_vertices.swap(vertices);
	m_textureCoords.swap(textureCoords);
	m_normals.swap(normals);
	m_count = (indexes.empty())?vertices.size():indexes.size();
	m_indexes.swap(indexes);
	std::swap(m_materials, materials);
	m_meshes.swap(meshes);
	DeleteList(m_lists);
	DeleteList(m_vertexLists);
	m_lists.clear();
	m_vertexLists.clear();
}

void C3DModel::SetAnimation(std::vector<unsigned int> & weightCount, std::vector<unsigned int> & weightIndexes, std::vector<float> & weights, std::vector<sJoint> & skeleton, std::vector<sAnimation> & animations)
{
	m_weightsCount.swap(weightCount);
	m_weightsIndexes.swap(weightIndexes);
	m_weights.swap(weights);
	m_skeleton.swap(skeleton);
	m_animations.swap(animations);
	DeleteList(m_lists);
	DeleteList(m_vertexLists);
	m_lists.clear();
	m_vertexLists.clear();
}

void C3DModel::SetBounding(std::shared_ptr<IBounding> bounding, double scale)
{
	m_bounding = bounding;
	m_scale = scale;
}

void SetMaterial(const sMaterial * material)
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
	texManager->SetTexture(material->texture);
}

void C3DModel::DrawModel(const std::set<std::string> * hideMeshes, bool vertexOnly, std::vector<CVector3f> const& vertices, std::vector<CVector3f> const& normals, bool useGPUskinning)
{
	if (useGPUskinning)
	{
		const CShaderManager * shader = CGameView::GetInstance().lock()->GetShaderManager();
		shader->SetUniformMatrix4("invBindMatrices", m_skeleton.size(), &m_gpuInverseMatrices[0]);
		shader->SetVertexAttrib4(16, &m_gpuWeight[0]);
		shader->SetVertexAttrib4(17, &m_gpuWeightIndexes[0]);
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
	glScaled(m_scale, m_scale, m_scale);
	if (!m_indexes.empty()) //Draw by meshes;
	{
		unsigned int begin = 0;
		unsigned int end;
		for (unsigned int i = 0; i < m_meshes.size(); ++i)
		{
			if (hideMeshes && hideMeshes->find(m_meshes[i].name) != hideMeshes->end())
			{
				end = m_meshes[i].polygonIndex;
				glDrawElements(GL_TRIANGLES, end - begin, GL_UNSIGNED_INT, &m_indexes[begin]);
				SetMaterial(m_materials.GetMaterial(m_meshes[i].materialName));
				begin = (i + 1 == m_meshes.size()) ? m_count : m_meshes[i + 1].polygonIndex;
				continue;
			}
			if (vertexOnly || (i > 0 && m_meshes[i].materialName == m_meshes[i - 1].materialName))
			{
				continue;
			}
			end = m_meshes[i].polygonIndex;
			glDrawElements(GL_TRIANGLES, end - begin, GL_UNSIGNED_INT, &m_indexes[begin]);
			if (!vertexOnly) SetMaterial(m_materials.GetMaterial(m_meshes[i].materialName));
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
	SetMaterial(&empty);
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
	for (unsigned int i = 0; i < m_weightsCount.size(); ++i)
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
	for (unsigned int i = 0; i < m_skeleton.size(); ++i)
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
	for (unsigned int i = 0; i < anims[current].children.size(); ++i)
	{
		AddAllChildren(anims, anims[current].children[i], set);
	}
}

void InterpolateMatrices(float * m1, const float * m2, float t)//works bad if matrices are strongly differ
{
	for (unsigned int i = 0; i < 16; ++i)
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
	for (unsigned int i = 0; i < skeleton.size(); ++i)
	{
		memcpy(&jointMatrices[i * 16], skeleton[i].matrix, sizeof(float) * 16);
	}
	//apply animations
	if (!animationToPlay.empty())
	{
		//get animations that are need to be played
		std::vector<unsigned int> animsToPlay;
		for (unsigned int i = 0; i < animations.size(); ++i)
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
		for (unsigned int i = 0; i < animsToPlay.size(); ++i)
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
				for (unsigned int j = 0; j < 16; ++j)
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
	for (unsigned int i = 0; i < skeleton.size(); ++i)
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

bool C3DModel::DrawSkinned(const std::set<std::string> * hideMeshes, bool vertexOnly, std::string const& animationToPlay, sAnimation::eLoopMode loop, float time, bool gpuSkinning)
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
		DrawModel(hideMeshes, vertexOnly, m_vertices, m_normals, true);
	}
	else
	{
		std::vector<CVector3f> vertices;
		std::vector<CVector3f> normals;
		vertices.resize(m_vertices.size());
		normals.resize(m_normals.size());
		unsigned int k = 0;
		for (unsigned int i = 0; i < m_vertices.size(); ++i)
		{
			//recalculate vertex using bones
			for (unsigned int j = 0; j < m_weightsCount[i]; ++j, ++k)
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
		DrawModel(hideMeshes, vertexOnly, vertices, normals);
	}
	return result;
}

void C3DModel::Draw(std::shared_ptr<IObject> object, bool vertexOnly, bool gpuSkinning)
{
	const std::set<std::string> * hideMeshes = &object->GetHiddenMeshes();
	unsigned int k = 0;//current index of weight and weightIndex arrays
	if (!m_weightsCount.empty())//object needs to be skinned
	{
		if (object->GetAnimation().empty())//no animation is playing, default pose
		{
			if (vertexOnly && m_vertexLists.find(*hideMeshes) == m_vertexLists.end())
			{
				unsigned int id = glGenLists(1);
				glNewList(id, GL_COMPILE);
				DrawSkinned(hideMeshes, true, "", sAnimation::NONLOOPING, 0.0f, gpuSkinning);
				glEndList();
				m_vertexLists[*hideMeshes] = id;
			}
			if (!vertexOnly && m_lists.find(*hideMeshes) == m_lists.end())
			{
				unsigned int id = glGenLists(1);
				glNewList(id, GL_COMPILE);
				DrawSkinned(hideMeshes, false, "", sAnimation::NONLOOPING, 0.0f, gpuSkinning);
				glEndList();
				m_lists[*hideMeshes] = id;
			}
			if (vertexOnly)
			{
				glCallList(m_vertexLists[*hideMeshes]);
			}
			else
			{
				glCallList(m_lists[*hideMeshes]);
			}
		}
		else//animation is playing, full computation
		{
			if (DrawSkinned(hideMeshes, false, object->GetAnimation(), object->GetAnimationLoop(), object->GetAnimationTime() / object->GetAnimationSpeed(), gpuSkinning))
			{
				object->PlayAnimation("");
			}
		}
	}
	else//static object
	{
		if (vertexOnly && m_vertexLists.find(*hideMeshes) == m_vertexLists.end())
		{
			unsigned int id = glGenLists(1);
			glNewList(id, GL_COMPILE);
			DrawModel(hideMeshes, true, m_vertices, m_normals);
			glEndList();
			m_vertexLists[*hideMeshes] = id;
		}
		if (!vertexOnly && m_lists.find(*hideMeshes) == m_lists.end())
		{
			unsigned int id = glGenLists(1);
			glNewList(id, GL_COMPILE);
			DrawModel(hideMeshes, false, m_vertices, m_normals);
			glEndList();
			m_lists[*hideMeshes] = id;
		}
		if (vertexOnly)
		{
			glCallList(m_vertexLists[*hideMeshes]);
		}
		else
		{
			glCallList(m_lists[*hideMeshes]);
		}
	}
}

void C3DModel::PreloadTextures() const
{
	CTextureManager * texManager = CTextureManager::GetInstance();
	for (unsigned int i = 0; i < m_meshes.size(); ++i)
	{
		if (!m_materials.GetMaterial(m_meshes[i].materialName)) continue;
		texManager->SetTexture(m_materials.GetMaterial(m_meshes[i].materialName)->texture);
	}
}

std::vector<std::string> C3DModel::GetAnimations() const
{
	std::vector<std::string> result;
	for (unsigned int i = 0; i < m_animations.size(); ++i)
	{
		result.push_back(m_animations[i].id);
	}
	return result;
}