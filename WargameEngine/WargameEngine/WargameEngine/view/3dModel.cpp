#include "3dModel.h"
#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/glut.h>

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

void C3DModel::SetBounding(std::shared_ptr<IBounding> bounding, double scale)
{
	m_bounding = bounding;
	m_scale = scale;
}

void SetMaterial(sMaterial * material)
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

void C3DModel::NewList(unsigned int & list, const std::set<std::string> * hideMeshes, bool vertexOnly)
{
	list = glGenLists(1);
	glNewList(list, GL_COMPILE);
	glPushMatrix();
	glScaled(m_scale, m_scale, m_scale);
	if (m_vbo)
	{
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vbo);
		if (!m_vertices.empty())
		{
			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(3, GL_FLOAT, 0, 0);
		}
		if (!m_normals.empty() && !vertexOnly)
		{
			glEnableClientState(GL_NORMAL_ARRAY);
			glNormalPointer(GL_FLOAT, 0, (void*)(m_vertices.size() * 3 * sizeof(float)));
		}
		if (!m_textureCoords.empty() && !vertexOnly)
		{
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, GL_FLOAT, 0, (void*)(m_vertices.size() * 3 * sizeof(float)+m_normals.size() * 3 * sizeof(float)));
		}
	}
	else
	{
		if (!m_vertices.empty())
		{
			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(3, GL_FLOAT, 0, &m_vertices[0]);
		}
		if (!m_normals.empty() && !vertexOnly)
		{
			glEnableClientState(GL_NORMAL_ARRAY);
			glNormalPointer(GL_FLOAT, 0, &m_normals[0]);
		}
		if (!m_textureCoords.empty() && !vertexOnly)
		{
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, GL_FLOAT, 0, &m_textureCoords[0]);
		}
	}
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
	if (m_vbo) glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	sMaterial empty;
	SetMaterial(&empty);
	glPopMatrix();
	glEndList();
}

void C3DModel::Draw(const std::set<std::string> * hideMeshes, bool vertexOnly)
{
	if (vertexOnly && m_vertexLists.find(*hideMeshes) == m_vertexLists.end())
	{
		NewList(m_vertexLists[*hideMeshes], hideMeshes, true);
	}
	if (!vertexOnly && m_lists.find(*hideMeshes) == m_lists.end())
	{
		NewList(m_lists[*hideMeshes], hideMeshes, false);
	}
	if (vertexOnly) glCallList(m_vertexLists[*hideMeshes]); else glCallList(m_lists[*hideMeshes]); 
}